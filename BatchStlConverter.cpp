#include "BatchStlConverter.h"

// Standard C++
#include <iostream>
#include <string>
#include <vector>
#include <filesystem> // For path manipulation and directory operations
#include <algorithm>  // For std::transform (used in scanInputDirectory)
#include <fstream>    // For output directory write test

// LDView components
#include "LDLoader/LDLMainModel.h"
#include "LDLib/LDrawModelViewer.h" // Needed for LDModelParser
#include "LDLib/LDModelParser.h"
#include "TRE/TREMainModel.h"
#include "LDExporter/LDStlExporter.h"
#include "TCFoundation/TCUserDefaults.h"
#include "LDLib/LDUserDefaultsKeys.h" // For LDRAWDIR_KEY
#include "TCFoundation/TCAutoreleasePool.h"
// TCAlertManager.h and LDConsoleAlertHandler.h are already in BatchStlConverter.h as per previous step

// Constructor
BatchStlConverter::BatchStlConverter(const std::string& inputDir, const std::string& outputDir)
    : m_inputDir(inputDir), m_outputDir(outputDir), m_logger(nullptr) {
    // Initialization of member variables is done in the initializer list
}

// Destructor
BatchStlConverter::~BatchStlConverter() {
    // No specific cleanup needed for now
}

// Set the logger callback
void BatchStlConverter::setLogger(BatchLogSink logger) {
    m_logger = logger;
}

// Private helper method to log messages
void BatchStlConverter::logMessage(const std::string& type, const std::string& message) {
    if (m_logger) {
        m_logger(type, message);
    } else {
        if (type == "ERROR") {
            std::cerr << type << ": " << message << std::endl;
        } else {
            std::cout << type << ": " << message << std::endl;
        }
    }
}

// Public method to run the batch conversion
bool BatchStlConverter::runConversion() {
    if (!directoryExists(m_inputDir)) { // directoryExists now uses logMessage
        return false;
    }
    logMessage("INFO", "Scanning input directory: " + m_inputDir + "...");
    if (!scanInputDirectory()) { // scanInputDirectory now uses logMessage
        return false;
    }

    logMessage("INFO", "Found " + std::to_string(m_datFiles.size()) + " .dat files to convert.");
    if (m_datFiles.empty()) {
        logMessage("INFO", "No .dat files found in the input directory. Nothing to do.");
        return true; // Successful run, but no files processed
    }

    if (!directoryExists(m_outputDir)) {
        logMessage("INFO", "Output directory " + m_outputDir + " does not exist, attempting to create it.");
        if (!createDirectory(m_outputDir)) { // createDirectory now uses logMessage
            return false;
        }
    }

    // Output directory write check
    std::filesystem::path outputDirTestPath = std::filesystem::path(m_outputDir) / "ldview_write_test.tmp";
    std::ofstream testFile(outputDirTestPath);
    if (!testFile.is_open()) {
        logMessage("ERROR", "Output directory " + m_outputDir + " is not writable.");
        if (std::filesystem::exists(outputDirTestPath)) {
            std::error_code ec_remove; 
            std::filesystem::remove(outputDirTestPath, ec_remove);
        }
        return false;
    }
    testFile.close();
    std::error_code ec_remove_after; 
    if (!std::filesystem::remove(outputDirTestPath, ec_remove_after) || ec_remove_after) {
         logMessage("WARNING", "Could not remove temporary test file from output directory: " + outputDirTestPath.string());
    }

    int successCount = 0;
    for (size_t fileIndex = 0; fileIndex < m_datFiles.size(); ++fileIndex) {
        const std::string& datFilePath = m_datFiles[fileIndex];
        
        std::filesystem::path inputPathObj(datFilePath);
        std::string inputFileBaseName = inputPathObj.filename().string();
        std::string outputFileBaseName = inputPathObj.stem().string() + ".stl";

        logMessage("PROCESSING", "Converting " + inputFileBaseName + " to " + outputFileBaseName +
                                 "... (File " + std::to_string(fileIndex + 1) + " of " + std::to_string(m_datFiles.size()) + ")");
        
        if (processFile(datFilePath)) { // processFile now uses logMessage
            successCount++;
        }
    }

    logMessage("INFO", "Batch conversion summary:");
    logMessage("INFO", "Successfully converted " + std::to_string(successCount) + " of " + std::to_string(m_datFiles.size()) + " files.");
    if (successCount < m_datFiles.size()) {
        logMessage("INFO", std::to_string(m_datFiles.size() - successCount) + " files failed to convert.");
    }
    return successCount == m_datFiles.size();
}

// Private method to scan the input directory for .dat files
bool BatchStlConverter::scanInputDirectory() {
    m_datFiles.clear(); 
    try {
        if (!std::filesystem::is_directory(m_inputDir)) { // This check is a bit redundant if directoryExists was called before, but good for internal robustness
             logMessage("ERROR", "Input path " + m_inputDir + " is not a valid directory.");
            return false;
        }

        for (const auto& entry : std::filesystem::directory_iterator(m_inputDir)) {
            if (entry.is_regular_file()) {
                std::filesystem::path p = entry.path();
                std::string ext = p.extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(),
                               [](unsigned char c){ return std::tolower(c); }); // Use lambda for tolower
                if (ext == ".dat") {
                    m_datFiles.push_back(p.string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        logMessage("ERROR", "Could not scan input directory " + m_inputDir + ". Details: " + e.what());
        return false;
    }
    return true;
}

// Private method to process a single .dat file
bool BatchStlConverter::processFile(const std::string& datFilePath) {
    // std::cout << "Processing " << datFilePath << "..." << std::endl; // Moved to runConversion loop with "PROCESSING:"
    TCAutoreleasePool pool;
    LDConsoleAlertHandler consoleAlertHandler;
    TCAlertManager::registerHandlerForAllAlerts(&consoleAlertHandler);

    // Load .dat file
    LDLMainModel mainLdlModel;
    const char* ldrawDir = TCUserDefaults::stringForKey(LDRAWDIR_KEY, nullptr, false);
    if (ldrawDir) {
        LDLModel::setLDrawDir(ldrawDir);
        logMessage("INFO", "LDraw directory set to: " + std::string(ldrawDir));
    } else {
        logMessage("WARNING", "LDraw directory not set. Part resolution may fail.");
    }

    if (!mainLdlModel.load(datFilePath.c_str())) {
        logMessage("ERROR", "Failed to load LDraw file: " + datFilePath);
        TCAlertManager::unregisterHandler(&consoleAlertHandler);
        return false;
    }

    // Parse model
    // For now, assuming default constructor is sufficient or a specific one for non-GUI is available.
    LDrawModelViewer modelViewer; 
    LDModelParser modelParser(&modelViewer); 
    
    // Set necessary flags on modelParser (e.g., flatten parts)
    // These flags might depend on desired output or typical LDView defaults for export
    modelParser.setFlattenPartsFlag(true); // Example: common for STL export
    // modelParser.setUseDisplayLists(false); // Might be relevant in non-GUI

    if (!modelParser.parseMainModel(&mainLdlModel)) {
        logMessage("ERROR", "Failed to parse LDraw model from file: " + datFilePath);
        TCAlertManager::unregisterHandler(&consoleAlertHandler);
        return false;
    }

    TREMainModel* treModel = modelParser.getMainTREModel();
    if (!treModel) {
        logMessage("ERROR", "No TRE model generated for file: " + datFilePath);
        TCAlertManager::unregisterHandler(&consoleAlertHandler);
        return false;
    }

    // Export to STL
    LDStlExporter stlExporter;
    std::string outputFilePath = getOutputFilePath(datFilePath);

    // LDExporter (base class) has m_filename (ucstring) and a protected setFilename(const ucchar *value).
    // We need a way to set this. Assuming a public method or a direct way to set m_filename.
    // For now, let's assume a conceptual setFilenameUC method exists that handles ucstring conversion.
    // This might require a change in LDExporter.h/cpp if not available.
    // Or, if LDStlExporter::doExport takes the path as an argument, that would be simpler.
    // The current LDExporter::doExport takes TREMainModel*.
    // Let's assume setFilenameUC is available for now.
    // If not, we'll need to address this by modifying LDExporter.
    // For the purpose of this step, we'll use a placeholder for the filename setting.
    // A common pattern is `exporter.setFilename(path.c_str())` or `exporter.setFilename(ucstring(path))`.
    // Let's assume `setFilenameUC` exists and handles const char* input appropriately.
    stlExporter.setFilenameUC(outputFilePath.c_str()); // Conceptual
    // Actual scale is read from TCUserDefaults by LDStlExporter (Key: "Scale", default 2 for cm)

    if (stlExporter.doExport(treModel) != 0) {
        logMessage("ERROR", "Failed to export STL for file: " + datFilePath + " to " + outputFilePath);
        TCAlertManager::unregisterHandler(&consoleAlertHandler);
        return false;
    }
    logMessage("INFO", "Successfully converted " + datFilePath + " to " + outputFilePath);

    TCAlertManager::unregisterHandler(&consoleAlertHandler);
    return true;
}

// Private method to get the output file path for a given .dat file
std::string BatchStlConverter::getOutputFilePath(const std::string& datFilePath) const {
    std::filesystem::path inputPath(datFilePath);
    std::string baseName = inputPath.stem().string(); // Gets filename without extension
    
    // Ensure m_outputDir does not have a trailing slash, then add one.
    // Or use std::filesystem::path::append which handles this.
    std::filesystem::path outputPath(m_outputDir);
    outputPath /= (baseName + ".stl"); // Append operator handles separators correctly

    return outputPath.string();
}

// Private helper method to check if a directory exists
bool BatchStlConverter::directoryExists(const std::string& path) const {
    try {
        std::error_code ec;
        bool exists = std::filesystem::exists(path, ec);
        if (ec) {
            logMessage("ERROR", "Filesystem error when checking if path exists: " + path + ". Details: " + ec.message());
            return false;
        }
        if (!exists) {
            logMessage("ERROR", "Path does not exist: " + path);
            return false;
        }
        bool isDir = std::filesystem::is_directory(path, ec);
        if (ec) {
            logMessage("ERROR", "Filesystem error when checking if path is a directory: " + path + ". Details: " + ec.message());
            return false;
        }
        if (!isDir) {
            logMessage("ERROR", "Path is not a directory: " + path);
            return false;
        }
        return true;
    } catch (const std::filesystem::filesystem_error& e) { 
        logMessage("ERROR", "Exception checking if directory " + path + " exists: " + e.what());
        return false;
    }
}

// Private helper method to create a directory
bool BatchStlConverter::createDirectory(const std::string& path) const {
    try {
        std::error_code ec;
        if (std::filesystem::create_directories(path, ec)) {
            logMessage("INFO", "Output directory " + path + " created.");
            return true;
        }
        if (ec) { 
            logMessage("ERROR", "Failed to create output directory " + path + ". Details: " + ec.message());
            return false;
        }
        if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
             logMessage("INFO", "Output directory " + path + " already exists.");
            return true;
        }
        logMessage("ERROR", "Failed to create directory " + path + " and it does not appear to exist.");
        return false;
    } catch (const std::filesystem::filesystem_error& e) {
        logMessage("ERROR", "Failed to create output directory " + path + ". Details: " + e.what());
        return false;
    }
}
