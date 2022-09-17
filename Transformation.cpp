#include "Constants.h"
#include "Serialization.h"
#include "Transformation.h"

#include <chrono>

namespace
{
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

void transformMatrix(Matrix& inMatrix, Matrix& outMatrix)
{
    uint8_t result;
    for (size_t columnIndex = 0; columnIndex < M; ++columnIndex)
    {
        for (size_t i = 0; i < M; ++i)
        {
            result = 0;
            for (size_t j = 0; j < M; ++j)
            {
                result = plus(result, mult(coeffs[i][j], inMatrix[j][columnIndex]));
            }
            outMatrix[i][columnIndex] = result;
        }
    }
}

void transformDataBatch(VecMatrixIt inBegin, VecMatrixIt inEnd, VecMatrixIt outBegin, VecMatrixIt outEnd)
{
    for (auto in = inBegin, out = outBegin; in != inEnd; ++in, ++out)
        transformMatrix(*in, *out);
}
}

void transformData(size_t N, std::string inFileName, std::string outFileName)
{
    size_t batchSize = 10;
    VecMatrix inMatrixes(batchSize, Matrix());
    VecMatrix outMatrixes(batchSize, Matrix());
    size_t done = 0;
    cleanFile(outFileName);

    auto startTimeStamp = high_resolution_clock::now();

    while (done < N)
    {
        size_t count = std::min(batchSize, N - done);

        deserializeDataBatch(inFileName, inMatrixes.begin(), inMatrixes.begin() + count);
        transformDataBatch(inMatrixes.begin(), inMatrixes.begin() + count, outMatrixes.begin(), outMatrixes.begin() + count);
        serializeDataBatch(outFileName, outMatrixes.begin(), outMatrixes.begin() + count);

        done += count;
    }

    auto stopTimeStamp = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(stopTimeStamp - startTimeStamp);
    serializeTimeStamp(outFileName, duration.count());
}