#pragma once

#include "Constants.h"

#include <string>

void cleanFile(std::string fileName);

void serializeTimeStamp(std::string fileName, size_t timeStamp);

void serializeDataBatch(std::string inFileName, VecMatrixIt begin, VecMatrixIt end);

void deserializeDataBatch(std::string inFileName, VecMatrixIt begin, VecMatrixIt end);
