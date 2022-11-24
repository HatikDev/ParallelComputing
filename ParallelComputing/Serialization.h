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

class ProcessReader
{
public:
    ProcessReader(std::string fileName, int beginPos, int endPos);
    ~ProcessReader();

    void readData(VecMatrixIt begin, VecMatrixIt end);
private:
    std::string m_fileName;
    int m_beginPos;
    int m_endPos;
};