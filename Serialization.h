#pragma once

#include "Constants.h"

#include <memory>
#include <string>

class FileReader
{
public:
    FileReader(std::string fileName);
    ~FileReader();
    void startReading();

    void finishReading();

    void deserializeDataBatch(VecMatrixIt begin, VecMatrixIt end);

private:
    std::string m_fileName;
    std::unique_ptr<std::ifstream> m_inputFileStream;
    bool m_isInputFileOpen;
};

class FileWriter
{
public:
    FileWriter(std::string fileName);
    ~FileWriter();
    void cleanFile();

    void startWriting();

    void finishWriting();

    void serializeTimeStamp(size_t timeStamp);

    void serializeDataBatch(VecMatrixIt begin, VecMatrixIt end);

private:
    std::string m_fileName;
    std::unique_ptr<std::ofstream> m_outputFileStream;
    bool m_isOutputFileOpen;
};