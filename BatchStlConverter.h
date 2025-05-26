#ifndef BATCHSTLCONVERTER_H
#define BATCHSTLCONVERTER_H

#include <string>
#include <vector>
#include <functional> // For std::function

// Forward declarations
class LDrawModelViewer;

// Includes for classes used in BatchStlConverter.cpp methods
#include "TCFoundation/TCAlertManager.h" // As per subtask instructions
#include "LDLib/LDConsoleAlertHandler.h" // As per subtask instructions


class BatchStlConverter {
public:
    typedef std::function<void(const std::string& type, const std::string& message)> BatchLogSink;

    BatchStlConverter(const std::string& inputDir, const std::string& outputDir);
    ~BatchStlConverter();

    bool runConversion();
    void setLogger(BatchLogSink logger);

private:
    bool scanInputDirectory();
    bool processFile(const std::string& datFilePath);
    std::string getOutputFilePath(const std::string& datFilePath) const;
    void logMessage(const std::string& type, const std::string& message);

    // Helper methods for directory operations
    bool directoryExists(const std::string& path) const;
    bool createDirectory(const std::string& path) const;

    std::string m_inputDir;
    std::string m_outputDir;
    std::vector<std::string> m_datFiles;
    BatchLogSink m_logger;
};

#endif // BATCHSTLCONVERTER_H
