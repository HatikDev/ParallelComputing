#include "Constants.h"
#include "Serialization.h"
#include "Transformation.h"

#include <mpi.h>
#include <chrono>
#include <utility>

using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

void transformData(int argc, char* argv[], size_t N, std::string inFileName, std::string outFileName)
{
    MPI_Init(&argc, &argv);

    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int thread;
    MPI_Comm_rank(MPI_COMM_WORLD, &thread);

    int dataSize = N / size;
    std::pair<int, int> interval = { dataSize * thread, dataSize * (thread + 1) };
    if (thread == size && interval.second < N)
        interval.second = N;

    VecMatrix inMatrixes(dataSize, Matrix());
    VecMatrix outMatrixes(dataSize, Matrix());

    ProcessReader reader(inFileName, interval.first * 16, interval.second * 16);
    reader.readData(inMatrixes.begin(), inMatrixes.end());

    auto startTimeInterval = steady_clock::now();
    uint8_t result;
    for (int i = 0; i < interval.second - interval.first; ++i)
    {
        for (size_t j = 0; j < M; ++j)
        {
            for (size_t k = 0; k < M; ++k)
            {
                result = 0;
                for (size_t l = 0; l < M; ++l)
                {
                    result = plus(result, mult(coeffs[j][l], inMatrixes[i][l][k]));
                }

                outMatrixes[i][j][k] = result;
            }

        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    auto stopTimeInterval = steady_clock::now();

    int value = 0; // useless value
    if (thread == 0)
    {
        FileWriter writer(outFileName);
        writer.cleanFile();

        writer.startWriting();
        writer.serializeDataBatch(outMatrixes.begin(), outMatrixes.end());
        writer.finishWriting();

        MPI_Send(&value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    }
    else if (thread == size - 1)
    {
        MPI_Status status;
        MPI_Recv(&value, 1, MPI_INT, thread - 1, 0, MPI_COMM_WORLD, &status);

        FileWriter writer(outFileName);
        writer.startWriting();
        writer.serializeDataBatch(outMatrixes.begin(), outMatrixes.end());

        auto duration = static_cast<size_t>(duration_cast<milliseconds>(stopTimeInterval - startTimeInterval).count());
        writer.serializeTimeStamp(duration);

        writer.finishWriting();
    }
    else
    {
        int previousProcessTime;
        MPI_Status status;
        MPI_Recv(&previousProcessTime, 1, MPI_INT, thread - 1, 0, MPI_COMM_WORLD, &status);

        FileWriter writer(outFileName);
        writer.startWriting();
        writer.serializeDataBatch(outMatrixes.begin(), outMatrixes.end());
        writer.finishWriting();

        MPI_Send(&value, 1, MPI_INT, thread + 1, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
}