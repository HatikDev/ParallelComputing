#include "Constants.h"
#include "Serialization.h"
#include "Transformation.h"

#include <chrono>

namespace
{
using std::chrono::steady_clock;
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
    VecMatrix inMatrixes(batchSize, Matrix());
    VecMatrix outMatrixes(batchSize, Matrix());
    size_t done = 0;
    size_t duration = 0;
    FileWriter writer(outFileName);
    writer.cleanFile();
    FileReader reader(inFileName);

    writer.startWriting();
    reader.startReading();

    while (done < N)
    {
        size_t count = std::min(batchSize, N - done);

        reader.deserializeDataBatch(inMatrixes.begin(), inMatrixes.begin() + count);
        auto startTimeStamp = steady_clock::now();
        transformDataBatch(inMatrixes.begin(), inMatrixes.begin() + count, outMatrixes.begin(), outMatrixes.begin() + count);
        auto stopTimeStamp = steady_clock::now();
        writer.serializeDataBatch(outMatrixes.begin(), outMatrixes.begin() + count);

        done += count;
        duration += static_cast<size_t>(duration_cast<milliseconds>(stopTimeStamp - startTimeStamp).count());
    }

    writer.serializeTimeStamp(duration);

    writer.finishWriting();
    reader.finishReading();
}