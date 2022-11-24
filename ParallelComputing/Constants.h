#pragma once

#include <array>
#include <vector>

constexpr size_t M = 4; // matrix dimension
constexpr size_t MATRIXWEIGHT = M * M; // size of 1 matrix in bytes
constexpr size_t batchSize = 1000;

using Matrix = std::array<std::array<uint8_t, M>, M>;
using VecMatrix = std::vector<Matrix>;
using VecMatrixIt = std::vector<Matrix>::iterator;

constexpr Matrix coeffs =
    { {
        { 2, 3, 1, 1 },
        { 1, 2, 3, 1 },
        { 1, 1, 2, 3 },
        { 3, 1, 1, 2 }
    } };