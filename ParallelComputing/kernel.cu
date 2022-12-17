
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>

#include "Constants.h"
#include "Generator.h"
#include "Serialization.h"

#include <sstream>
#include <chrono>

#include <mpi.h>

using std::chrono::steady_clock;

namespace
{
__device__ int dev_maxPower(uint16_t number)
{
    int powerArray = 16384;
    for (int i = 7; i >= 0; --i)
    {
        if (number & powerArray)
            return i + 8;
        powerArray >= 1;
    }

    return 0;
}
}

__device__ uint8_t dev_plus(uint8_t num1, uint8_t num2)
{
    return num1 ^ num2;
}

__device__ uint8_t dev_mult(uint8_t num1, uint8_t num2)
{
    uint16_t result = 0;

    for (int i = 0; i < 8; ++i)
    {
        if (!(num1 & 1))
        {
            num1 >>= 1;
            continue;
        }

        uint8_t num2_2 = num2;
        for (int j = 0; j < 8; ++j)
        {
            if (!(num2_2 & 1))
            {
                num2_2 >>= 1;
                continue;
            }

            result ^= (1 << (i + j));
            num2_2 >>= 1;
        }

        num1 >>= 1;
    }

    while (result > 255)
    {
        int power = dev_maxPower(result);
        result ^= 0x11b << (power - 8);
    }

    return static_cast<uint8_t>(result);
}

__global__ void transformMatrix(uint8_t* coeffs, uint8_t* dev_inMatrix, uint8_t* dev_outMatrix)
{
    int index = threadIdx.x;
    uint8_t* inMatrixBegin = dev_inMatrix + index * M * M;
    uint8_t* outMatrixBegin = dev_outMatrix + index * M * M;

    uint8_t sum = 0;
    for (size_t i = 0; i < M; ++i)
    {
        for (size_t j = 0; j < M; ++j)
        {
            sum = 0;
            for (size_t k = 0; k < M; ++k)
            {
                sum += dev_plus(sum, dev_mult(coeffs[i * M + k], dev_inMatrix[k * M + j]));
            }
            dev_outMatrix[i * M + j] = sum;
        }
    }
}

cudaError_t initCoeffsMatrix(uint8_t** coeffs)
{
    cudaError_t cudaStatus = cudaMalloc((void**)coeffs, 16 * sizeof(uint8_t));
    if (cudaStatus != cudaSuccess)
        return cudaStatus;

    uint8_t cudaArray[] = { 2, 3, 1, 1, 1, 2, 3, 1, 1, 1, 2, 3, 3, 1, 1, 2 };
    cudaStatus = cudaMemcpy(*coeffs, cudaArray, 16, cudaMemcpyHostToDevice);

    return cudaStatus;
}

cudaError_t cudaTransformData(size_t N, std::string inFileName, std::string outFileName)
{
    VecMatrix inMatrixes(N, Matrix());
    VecMatrix outMatrixes(N, Matrix());

    FileReader reader(inFileName);
    FileWriter writer(outFileName);

    reader.startReading();
    reader.readData(inMatrixes.begin(), inMatrixes.end());
    reader.finishReading();

    cudaError_t cudaStatus = cudaSetDevice(0);
    if (cudaStatus != cudaSuccess)
    {
        fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
        goto Error;
    }

    // create coeffs for multiplying matrixes

    uint8_t* dev_coeffs;
    cudaStatus = cudaMalloc((void**)&dev_coeffs, 16 * sizeof(uint8_t));
    if (cudaStatus != cudaSuccess)
    {
        fprintf(stderr, "cudaMalloc for coeffs failed!");
        return cudaStatus;
    }

    uint8_t cudaArray[] = { 2, 3, 1, 1, 1, 2, 3, 1, 1, 1, 2, 3, 3, 1, 1, 2 };
    cudaStatus = cudaMemcpy(dev_coeffs, cudaArray, 16, cudaMemcpyHostToDevice);

    // prepare cuda memory for input matrixes

    uint8_t* dev_inMatrixes;
    cudaStatus = cudaMalloc((void**)&dev_inMatrixes, M * M * N * sizeof(uint8_t));
    if (cudaStatus != cudaSuccess)
    {
        fprintf(stderr, "cudaMalloc for input matrixes failed!");
        goto Error;
    }

    // prepare cuda memory for output matrixes

    uint8_t* dev_outMatrixes;
    cudaStatus = cudaMalloc((void**)&dev_outMatrixes, M * M * N * sizeof(uint8_t));
    if (cudaStatus != cudaSuccess)
    {
        fprintf(stderr, "cudaMalloc for output matrixes failed!");
        goto Error;
    }

    // copy input matrixes into host memory

    uint8_t* matrixesCopy = (uint8_t*)malloc(M * M * N);
    uint8_t* pointer = matrixesCopy;
    for (auto& matrix : inMatrixes)
    {
        for (size_t i = 0; i < M; ++i)
        {
            for (size_t j = 0; j < M; ++j)
            {
                *pointer = matrix[i][j];
                ++pointer;
            }
        }
    }

    // copy input matrixes into cuda memory

    cudaStatus = cudaMemcpy(dev_inMatrixes, matrixesCopy, M * M * N * sizeof(uint8_t), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess)
    {
        fprintf(stderr, "cudaMemcpy matrixes failed!");
        goto Error;
    }

    // transform matrixes

    cudaEvent_t start;
    cudaEvent_t stop;
    float duration;

    cudaEventCreate(&start);
    cudaEventRecord(start);

    transformMatrix<<<1, N>>>(dev_coeffs, dev_inMatrixes, dev_outMatrixes);

    cudaEventCreate(&stop);
    cudaEventRecord(stop);
    cudaEventSynchronize(stop);

    cudaEventElapsedTime(&duration, start, stop);

    // copy output matrixes from cuda into host memory

    cudaStatus = cudaMemcpy(matrixesCopy, dev_inMatrixes, M * M * N * sizeof(uint8_t), cudaMemcpyDeviceToHost);
    if (cudaStatus != cudaSuccess)
    {
        fprintf(stderr, "cudaMemcpy matrixes failed!");
        goto Error;
    }

    // copy output matrixes into std object

    pointer = matrixesCopy;
    for (auto& matrix : outMatrixes)
    {
        for (size_t i = 0; i < M; ++i)
        {
            for (size_t j = 0; j < M; ++j)
            {
                matrix[i][j] = *pointer;
                ++pointer;
            }
        }
    }

    // write data in file

    writer.startWriting();
    writer.writeData(outMatrixes.begin(), outMatrixes.end());
    writer.finishWriting();

    MPI_Send(&duration, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);

Error:
    cudaFree(dev_coeffs);
    cudaFree(dev_inMatrixes);
    cudaFree(dev_outMatrixes);
    free(matrixesCopy);

    return cudaStatus;
}
