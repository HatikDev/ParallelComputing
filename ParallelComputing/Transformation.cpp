#include "Constants.h"
#include "Serialization.h"
#include "Transformation.h"

#include <chrono>

void transformData(size_t N, std::string inFileName, std::string outFileName)
{
    VecMatrix inMatrixes(batchSize, Matrix());
    VecMatrix outMatrixes(batchSize, Matrix());

    uint8_t result;
    size_t done = 0;
    size_t duration = 0;
    FileWriter writer(outFileName);
    writer.cleanFile();
    FileReader reader(inFileName);

    writer.startWriting();
    reader.startReading();

    while (done < N)
    {
        reader.deserializeDataBatch(inMatrixes.begin(), inMatrixes.end());
        auto startTimeStamp = std::chrono::steady_clock::now();

#pragma omp parallel
        {
#pragma omp for
            for (int i = 0; i < batchSize * M; ++i)
            {
                for (size_t k = 0; k < M; ++k)
                {
                    result = 0;
                    for (size_t j = 0; j < M; ++j)
                        result = plus(result, mult(coeffs[k][j], inMatrixes[i / M][j][i % M]));

                    outMatrixes[i / M][k][i % M] = result;
                }
            }
        }

        auto stopTimeStamp = std::chrono::steady_clock::now();
        writer.serializeDataBatch(outMatrixes.begin(), outMatrixes.end());
        done += batchSize;
        duration += static_cast<size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(stopTimeStamp - startTimeStamp).count());
    }
    writer.serializeTimeStamp(duration);

    writer.finishWriting();
    reader.finishReading();
}