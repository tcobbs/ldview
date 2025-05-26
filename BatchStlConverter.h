#ifndef BATCHSTLCONVERTER_H
#define BATCHSTLCONVERTER_H

#include <string>
#include <vector>

// Forward declarations
class LDrawModelViewer; 

// Includes for classes used in BatchStlConverter.cpp methods
#include "TCFoundation/TCAlertManager.h" // As per subtask instructions
#include "LDLib/LDConsoleAlertHandler.h" // As per subtask instructions


class BatchStlConverter {
public:
    BatchStlConverter(const std::string& inputDir, const std::string& outputDir);
    ~BatchStlConverter();

    bool runConversion();

private:
    bool scanInputDirectory();
    bool processFile(const std::string& datFilePath);
    std::string getOutputFilePath(const std::string& datFilePath) const;

    // Helper methods for directory operations
    bool directoryExists(const std::string& path) const;
    bool createDirectory(const std::string& path) const;

    std::string m_inputDir;
    std::string m_outputDir;
    std::vector<std::string> m_datFiles;
};

#endif // BATCHSTLCONVERTER_H
