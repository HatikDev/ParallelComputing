#include "Generator.h"
#include "Serialization.h"
#include "Util.h"

void generateDataBatch(size_t batchSize, VecMatrix& matrixes)
{
    for (auto& matrix : matrixes)
    {
        for (size_t i = 0; i < M; ++i)
        {
            for (size_t j = 0; j < M; ++j)
            {
                matrix[i][j] = static_cast<uint8_t>(getRand(0, 0xff));
            }
        }
    }
}

void generateData(size_t N, std::string inFileName)
{
    size_t done = 0;
    VecMatrix matrixes(batchSize, Matrix());
    cleanFile(inFileName);

    while (done < N)
    {
        size_t count = std::min(batchSize, N - done);
        generateDataBatch(count, matrixes);
        serializeDataBatch(inFileName, matrixes.begin(), matrixes.begin() + count);
        done += batchSize;
    }
}