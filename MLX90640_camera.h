#pragma once

#include <Adafruit_MLX90640.h>
#include <Wire.h>


extern Adafruit_MLX90640 mlx;
extern float frame[768];
extern int16_t frameComp[768];

void initSensor();
bool captureFrame();
