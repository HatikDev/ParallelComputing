#include "Serialization.h"

#include <iomanip>
#include <iostream>
#include <fstream>

void cleanFile(std::string fileName)
{
    std::ofstream file(fileName, std::ios::trunc);
    file.close();
}

void serializeTimeStamp(std::string fileName, size_t timeStamp)
{
    std::ofstream file(fileName, std::ofstream::app);
    if (!file)
        throw std::logic_error("Can't open file");

    file << std::endl << std::endl << "Time: " << timeStamp;
    file.close();
}

void serializeDataBatch(std::string inFileName, VecMatrixIt begin, VecMatrixIt end)
{
    std::ofstream file(inFileName, std::ofstream::app);
    if (!file)
        throw std::logic_error("Can't open file");

    for (auto it = begin; it != end; ++it)
    {
        for (size_t i = 0; i < M; ++i)
        {
            for (size_t j = 0; j < M; ++j)
            {
                file << std::setw(2) << std::setfill('0') << std::hex << (int)(*it)[i][j];
            }
        }
    }

    file.close();
}

void deserializeDataBatch(std::string inFileName, VecMatrixIt begin, VecMatrixIt end)
{
    std::ifstream file(inFileName);

    char temp[] = "--";
    for (auto it = begin; it != end; ++it)
    {
        for (size_t i = 0; i < M; ++i)
        {
            for (size_t j = 0; j < M; ++j)
            {
                file >> temp[0] >> temp[1];
                (*it)[i][j] = static_cast<uint8_t>(strtoul(temp, NULL, 16));
            }
        }
    }

    file.close();
}
