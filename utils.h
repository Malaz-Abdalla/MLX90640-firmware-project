#pragma once
#include <vector>

float calculateThreshold(float Ta);
float computeAverage(const std::vector<int>& indices);
float completeWallAverage();
void completeWallMinMax();
float calculateStandardDeviation (std::vector <float>& prevAvgs);
