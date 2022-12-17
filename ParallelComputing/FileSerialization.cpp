#include "Serialization.h"
#include "Util.h"

#include <iomanip>
#include <fstream>

FileReader::FileReader(std::string fileName) : m_fileName{ fileName }, m_inputFileStream{ std::make_unique<std::ifstream>(fileName) }, m_isInputFileOpen{ false }
{}

FileReader::~FileReader()
{
    if (m_isInputFileOpen)
        m_inputFileStream->close();
}

void FileReader::startReading()
{
    if (m_isInputFileOpen)
        throw std::logic_error("File is already open");

    m_inputFileStream = std::make_unique<std::ifstream>(std::ifstream(m_fileName));
    if (!m_inputFileStream)
        throw std::logic_error("Can't open file");

    m_isInputFileOpen = true;
}

void FileReader::finishReading()
{
    m_inputFileStream->close();
    m_isInputFileOpen = false;
}

void FileReader::readData(VecMatrixIt begin, VecMatrixIt end)
{
    char temp[] = "--";
    for (auto it = begin; it != end; ++it)
    {
        for (size_t i = 0; i < M; ++i)
        {
            for (size_t j = 0; j < M; ++j)
            {
                *m_inputFileStream >> temp[0] >> temp[1];
                (*it)[i][j] = static_cast<uint8_t>(strtoul(temp, NULL, 16));
            }
        }
    }
}

FileWriter::FileWriter(std::string fileName) : m_fileName{ fileName }, m_outputFileStream{ std::make_unique<std::ofstream>("") }, m_isOutputFileOpen{ false }
{}

FileWriter::~FileWriter()
{
    if (m_isOutputFileOpen)
        m_outputFileStream->close();
}

void FileWriter::startWriting()
{
    if (m_isOutputFileOpen)
        throw std::logic_error("File is already open");

    m_outputFileStream = std::make_unique<std::ofstream>(m_fileName, std::ofstream::app);
    if (!m_outputFileStream)
        throw std::logic_error("Can't open file");

    m_isOutputFileOpen = true;
}

void FileWriter::finishWriting()
{
    m_outputFileStream->close();
    m_isOutputFileOpen = false;
}

void FileWriter::cleanFile()
{
    std::ofstream file(m_fileName, std::ios::trunc);
    file.close();
}

void FileWriter::serializeTimeStamp(std::string message, float timeStamp)
{
    *m_outputFileStream << std::endl << std::endl << message << std::dec << timeStamp;
}

void FileWriter::writeData(VecMatrixIt begin, VecMatrixIt end)
{
    for (auto it = begin; it != end; ++it)
    {
        for (size_t i = 0; i < M; ++i)
        {
            for (size_t j = 0; j < M; ++j)
            {
                *m_outputFileStream << std::setw(2) << std::setfill('0') << std::hex << (int)(*it)[i][j];
            }
        }
    }
}

ProcessReader::ProcessReader(std::string fileName, int beginPos, int endPos) : m_fileName{ fileName }, m_beginPos{ beginPos }, m_endPos{ endPos }
{}

ProcessReader::~ProcessReader()
{}

void ProcessReader::readData(VecMatrixIt begin, VecMatrixIt end)
{
    std::FILE* fp = std::fopen(m_fileName.c_str(), "r");
    if (!fp)
        throw std::exception();

    int beginPos = 2 * m_beginPos;
    int endPos = 2 * m_endPos;

    std::fseek(fp, beginPos, SEEK_SET);

    int oldBufferSize = (endPos - beginPos) * MATRIXWEIGHT;
    std::vector<char> oldBuffer(oldBufferSize, 0);
    std::fread(oldBuffer.data(), sizeof(char), oldBufferSize, fp);

    int newBufferSize = (m_endPos - m_beginPos) * MATRIXWEIGHT;
    std::vector<int> newBuffer(newBufferSize, 0);
    for (size_t i = 0; i < oldBuffer.size() - 1; i += 2)
        newBuffer[i / 2] = char2dec(oldBuffer[i], oldBuffer[i + 1]);

    size_t counter = 0;
    for (auto it = begin; it != end; ++it)
    {
        for (size_t i = 0; i < M; ++i)
        {
            for (size_t j = 0; j < M; ++j)
            {
                (*it)[i][j] = newBuffer[counter];
                ++counter;
            }
        }
    }

    std::fclose(fp);
}
