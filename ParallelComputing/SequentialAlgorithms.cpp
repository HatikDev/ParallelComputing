#include "Constants.h"
#include "Generator.h"
#include "Serialization.h"
#include "Transformation.h"

#include <iostream>
#include <sstream>

void processArgs(int argc, char* argv[])
{
    if (argc < 4)
        throw std::logic_error("Unexpected arguments");

    std::string comand = argv[1];
    std::string dataSize = argv[2];
    std::string inputFile = argv[3];
    size_t inputDataSize;
    std::istringstream(std::string(argv[2])) >> inputDataSize;

    if (comand == "generate")
    {
        generateData(inputDataSize * 1024 * 1024 / MATRIXWEIGHT, inputFile);
    }
    else if (comand == "transform")
    {
        if (argc != 5)
            throw std::logic_error("Unexpected arguments");

        std::string outputFile = argv[4];
        transformData(argc, argv, inputDataSize * 1024 * 1024 / MATRIXWEIGHT, inputFile, outputFile);
    }
    else
    {
        throw std::logic_error("Unexpected comand");
    }
}

int main(int argc, char* argv[])
{
    srand(static_cast<unsigned int>(time(NULL)));

    try
    {
        processArgs(argc, argv);
    }
    catch(std::exception e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
