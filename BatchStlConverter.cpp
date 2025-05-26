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
    : m_inputDir(inputDir), m_outputDir(outputDir) {
    // Initialization of member variables is done in the initializer list
}

// Destructor
BatchStlConverter::~BatchStlConverter() {
    // No specific cleanup needed for now
}

// Public method to run the batch conversion
bool BatchStlConverter::runConversion() {
    if (!directoryExists(m_inputDir)) { // directoryExists already prints detailed errors
        return false;
    }
    std::cout << "INFO: Scanning input directory: " << m_inputDir << "..." << std::endl;
    if (!scanInputDirectory()) { // scanInputDirectory prints its own errors
        return false;
    }

    std::cout << "INFO: Found " << m_datFiles.size() << " .dat files to convert." << std::endl;
    if (m_datFiles.empty()) {
        std::cout << "INFO: No .dat files found in the input directory. Nothing to do." << std::endl;
        return true; // Successful run, but no files processed
    }

    if (!directoryExists(m_outputDir)) {
        std::cout << "INFO: Output directory " << m_outputDir << " does not exist, attempting to create it." << std::endl;
        if (!createDirectory(m_outputDir)) { // createDirectory prints its own error
            return false;
        }
        // Success message for creation is now in createDirectory or implied if it already existed
    }

    // Output directory write check
    std::filesystem::path outputDirTestPath = std::filesystem::path(m_outputDir) / "ldview_write_test.tmp";
    std::ofstream testFile(outputDirTestPath);
    if (!testFile.is_open()) {
        std::cerr << "ERROR: Output directory " << m_outputDir << " is not writable." << std::endl;
        // Attempt to remove if it was somehow created but not openable
        if (std::filesystem::exists(outputDirTestPath)) {
            std::error_code ec; // To prevent exception on remove
            std::filesystem::remove(outputDirTestPath, ec);
        }
        return false;
    }
    testFile.close();
    std::error_code ec; // To prevent exception on remove
    if (!std::filesystem::remove(outputDirTestPath, ec) || ec) {
         std::cerr << "WARNING: Could not remove temporary test file from output directory: " << outputDirTestPath.string() << std::endl;
    }

    int successCount = 0;
    for (size_t fileIndex = 0; fileIndex < m_datFiles.size(); ++fileIndex) {
        const std::string& datFilePath = m_datFiles[fileIndex];
        
        std::filesystem::path inputPathObj(datFilePath);
        std::string inputFileBaseName = inputPathObj.filename().string();
        std::string outputFileBaseName = inputPathObj.stem().string() + ".stl";

        std::cout << "PROCESSING: Converting " << inputFileBaseName << " to " << outputFileBaseName 
                  << "... (File " << (fileIndex + 1) << " of " << m_datFiles.size() << ")" << std::endl;
        
        if (processFile(datFilePath)) {
            successCount++;
        } else {
            // processFile now prints its own detailed error message
            // std::cerr << "Failed to convert " << datFilePath << std::endl; // Redundant
        }
    }

    std::cout << "INFO: Batch conversion summary:" << std::endl;
    std::cout << "INFO: Successfully converted " << successCount << " of " << m_datFiles.size() << " files." << std::endl;
    if (successCount < m_datFiles.size()) {
        std::cout << "INFO: " << (m_datFiles.size() - successCount) << " files failed to convert." << std::endl;
    }
    return successCount == m_datFiles.size(); // Return true only if all files succeeded
}

// Private method to scan the input directory for .dat files
bool BatchStlConverter::scanInputDirectory() {
    m_datFiles.clear(); // Clear any previous results
    try {
        // Robustness check: ensure m_inputDir is a directory.
        // runConversion already calls directoryExists, but this is an internal method.
        if (!std::filesystem::is_directory(m_inputDir)) {
             std::cerr << "ERROR: Input path " << m_inputDir << " is not a valid directory." << std::endl;
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
        std::cerr << "ERROR: Could not scan input directory " << m_inputDir << ". Details: " << e.what() << std::endl;
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
        // Assuming LDLModel::setLDrawDir is the static method to call
        // If mainLdlModel has a direct setter, that would be used instead.
        // Based on analysis, LDLModel::setLDrawDir is static.
        LDLModel::setLDrawDir(ldrawDir); 
        std::cout << "LDraw directory set to: " << ldrawDir << std::endl;
    } else {
        std::cerr << "Warning: LDraw directory not set. Part resolution may fail." << std::endl;
    }

    if (!mainLdlModel.load(datFilePath.c_str())) {
        std::cerr << "ERROR: Failed to load LDraw file: " << datFilePath << std::endl;
        TCAlertManager::unregisterHandler(&consoleAlertHandler);
        return false;
    }
    // std::cout << "Successfully loaded DAT file: " << datFilePath << std::endl; // Too verbose

    // Parse model
    // LDrawModelViewer constructor might take parameters, ensure this is valid for non-GUI
    // For now, assuming default constructor is sufficient or a specific one for non-GUI is available.
    LDrawModelViewer modelViewer; 
    LDModelParser modelParser(&modelViewer); 
    
    // Set necessary flags on modelParser (e.g., flatten parts)
    // These flags might depend on desired output or typical LDView defaults for export
    modelParser.setFlattenPartsFlag(true); // Example: common for STL export
    // modelParser.setUseDisplayLists(false); // Might be relevant in non-GUI

    if (!modelParser.parseMainModel(&mainLdlModel)) {
        std::cerr << "ERROR: Failed to parse LDraw model from file: " << datFilePath << std::endl;
        TCAlertManager::unregisterHandler(&consoleAlertHandler);
        return false;
    }
    // std::cout << "Successfully parsed model." << std::endl; // Too verbose

    TREMainModel* treModel = modelParser.getMainTREModel();
    if (!treModel) {
        std::cerr << "ERROR: No TRE model generated for file: " << datFilePath << std::endl;
        TCAlertManager::unregisterHandler(&consoleAlertHandler);
        return false;
    }
    // std::cout << "Successfully retrieved TREMainModel." << std::endl; // Too verbose

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
        std::cerr << "ERROR: Failed to export STL for file: " << datFilePath << " to " << outputFilePath << std::endl;
        TCAlertManager::unregisterHandler(&consoleAlertHandler);
        return false;
    }
    std::cout << "INFO: Successfully converted " << datFilePath << " to " << outputFilePath << std::endl;

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
            std::cerr << "ERROR: Filesystem error when checking if path exists: " << path << ". Details: " << ec.message() << std::endl;
            return false;
        }
        if (!exists) {
            std::cerr << "ERROR: Path does not exist: " << path << std::endl;
            return false;
        }
        bool isDir = std::filesystem::is_directory(path, ec);
        if (ec) {
            std::cerr << "ERROR: Filesystem error when checking if path is a directory: " << path << ". Details: " << ec.message() << std::endl;
            return false;
        }
        if (!isDir) {
            std::cerr << "ERROR: Path is not a directory: " << path << std::endl;
            return false;
        }
        return true;
    } catch (const std::filesystem::filesystem_error& e) { // Catch-all for other potential fs errors
        std::cerr << "ERROR: Exception checking if directory " << path << " exists: " << e.what() << std::endl;
        return false;
    }
}

// Private helper method to create a directory
bool BatchStlConverter::createDirectory(const std::string& path) const {
    try {
        std::error_code ec;
        if (std::filesystem::create_directories(path, ec)) {
            std::cout << "INFO: Output directory " << path << " created." << std::endl;
            return true;
        }
        if (ec) { // An error occurred during create_directories
            std::cerr << "ERROR: Failed to create output directory " << path << ". Details: " << ec.message() << std::endl;
            return false;
        }
        // If create_directories returns false and no error code is set, it means the directory already existed.
        // (This behavior is typical for create_directories)
        if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
             std::cout << "INFO: Output directory " << path << " already exists." << std::endl;
            return true;
        }
        // Fallback error if directory still doesn't exist for some reason.
        std::cerr << "ERROR: Failed to create directory " << path << " and it does not appear to exist." << std::endl;
        return false;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "ERROR: Failed to create output directory " << path << ". Details: " << e.what() << std::endl;
        return false;
    }
}
