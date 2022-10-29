#pragma once

#include "Constants.h"

#include <string>

void startWriting(std::string fileName);

void finishWriting();

void startReading(std::string fileName);

void finishReading();

void cleanFile(std::string fileName);

void serializeTimeStamp(std::string fileName, size_t timeStamp);

void serializeDataBatch(VecMatrixIt begin, VecMatrixIt end);

void deserializeDataBatch(VecMatrixIt begin, VecMatrixIt end);
