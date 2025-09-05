#pragma once
#include "MRI.h"


void frameAndBlobTask(void *pvParameters);
// void analyzeBlob();
void completeDurationTask(void *pvParameters);

int analyzeBlob();


extern mostRecentinfo MRFI;
extern mostRecentinfo MRWI;

extern float wall, wallAvgsAcc, wallMin, wallMinAcc, wallMax, wallMaxAcc;
extern float face, faceAvgsAcc, faceMin, faceMinAcc, faceMax, faceMaxAcc;
extern float faceStandardDeviation , wallStandardDeviation ;
