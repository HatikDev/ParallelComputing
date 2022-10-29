#include "Serialization.h"

#include <iomanip>
#include <iostream>
#include <fstream>

namespace
{
    static std::ifstream inputFile;
    static std::ofstream outputFile;
    static bool isInputFileOpen = false;
    static bool isOutputFileOpen = false;
};

void startWriting(std::string fileName)
{
    if (isOutputFileOpen)
        throw std::logic_error("File is already open");

    outputFile = std::ofstream(fileName, std::ofstream::app);
    if (!outputFile)
        throw std::logic_error("Can't open file");

    isOutputFileOpen = true;
}

void finishWriting()
{
    outputFile.close();
    isOutputFileOpen = false;
}

void startReading(std::string fileName)
{
    if (isInputFileOpen)
        throw std::logic_error("File is already open");

    inputFile = std::ifstream(fileName);
    if (!inputFile)
        throw std::logic_error("Can't open file");

    isInputFileOpen = true;
}

void finishReading()
{
    inputFile.close();
    isInputFileOpen = false;
}

void cleanFile(std::string fileName)
{
    std::ofstream file(fileName, std::ios::trunc);
    file.close();
}

void serializeTimeStamp(std::string fileName, size_t timeStamp)
{
    outputFile << std::endl << std::endl << "Time: " << timeStamp;
}

void serializeDataBatch(VecMatrixIt begin, VecMatrixIt end)
{
    for (auto it = begin; it != end; ++it)
    {
        for (size_t i = 0; i < M; ++i)
        {
            for (size_t j = 0; j < M; ++j)
            {
                outputFile << std::setw(2) << std::setfill('0') << std::hex << (int)(*it)[i][j];
            }
        }
    }
}

void deserializeDataBatch(VecMatrixIt begin, VecMatrixIt end)
{
    char temp[] = "--";
    for (auto it = begin; it != end; ++it)
    {
        for (size_t i = 0; i < M; ++i)
        {
            for (size_t j = 0; j < M; ++j)
            {
                inputFile >> temp[0] >> temp[1];
                (*it)[i][j] = static_cast<uint8_t>(strtoul(temp, NULL, 16));
            }
        }
    }
}
