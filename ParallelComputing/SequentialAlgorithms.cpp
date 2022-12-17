#include "Constants.h"
#include "Generator.h"
#include "Serialization.h"
#include "Transformation.h"

#include <iostream>
#include <sstream>
#include <mpi.h>

#include "driver_types.h"

cudaError_t cudaTransformData(size_t N, std::string inFileName, std::string outFileName);

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

        MPI_Init(&argc, &argv);

        int thread;
        MPI_Comm_rank(MPI_COMM_WORLD, &thread);

        float cudaDuration;
        size_t mpiDuration;
        if (thread == 0)
        {
            size_t dataForCudaSize = inputDataSize * 1024 * 960 / MATRIXWEIGHT;
            cudaTransformData(dataForCudaSize, inputFile, outputFile);
        }

        if (thread != 0)
        {
            size_t dataForMPISize = inputDataSize * 1024 * 64 / MATRIXWEIGHT;
            mpiTransformData(argc, argv, dataForMPISize, inputFile, outputFile);
        }

        MPI_Finalize();
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
