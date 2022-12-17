#include "Constants.h"
#include "Serialization.h"
#include "Util.h"

#include <chrono>
#include <mpi.h>
#include <utility>

using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

void mpiTransformData(int argc, char* argv[], size_t N, std::string inFileName, std::string outFileName)
{
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int thread;
    MPI_Comm_rank(MPI_COMM_WORLD, &thread);

    int dataSize = N / (size - 1);
    std::pair<int, int> interval = { dataSize * thread, dataSize * (thread + 1) };
    if (thread == size - 1 && interval.second < N)
        interval.second = N;

    VecMatrix inMatrixes(interval.second - interval.first, Matrix());
    VecMatrix outMatrixes(interval.second - interval.first, Matrix());

    ProcessReader reader(inFileName, interval.first * MATRIXWEIGHT, interval.second * MATRIXWEIGHT);
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
                    result = host_plus(result, host_mult(coeffs[j][l], inMatrixes[i][l][k]));
                }

                outMatrixes[i][j][k] = result;
            }

        }
    }

    auto stopTimeInterval = steady_clock::now();
    auto mpiDuration = static_cast<size_t>(duration_cast<milliseconds>(stopTimeInterval - startTimeInterval).count());

    float cudaDuration = 0;
    if (thread == size - 1)
    {
        MPI_Status status;
        MPI_Recv(&cudaDuration, 1, MPI_FLOAT, thread - 1, 0, MPI_COMM_WORLD, &status);

        FileWriter writer(outFileName);
        writer.startWriting();
        writer.writeData(outMatrixes.begin(), outMatrixes.end());

        writer.serializeTimeStamp("MPI time: ", mpiDuration);
        writer.serializeTimeStamp("CUDA time: ", cudaDuration);

        writer.finishWriting();
    }
    else // for threads 1..(size - 2)
    {
        MPI_Status status;
        MPI_Recv(&cudaDuration, 1, MPI_FLOAT, thread - 1, 0, MPI_COMM_WORLD, &status);

        FileWriter writer(outFileName);
        writer.startWriting();
        writer.writeData(outMatrixes.begin(), outMatrixes.end());
        writer.finishWriting();

        MPI_Send(&cudaDuration, 1, MPI_FLOAT, thread + 1, 0, MPI_COMM_WORLD);
    }
}