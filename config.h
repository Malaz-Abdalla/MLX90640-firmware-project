#pragma once

#define SECONDS_to_MS 1000

#define completeDuration 10 //in seconds 

#define comfortThreshold_Ta 23
#define coldThreshold_Ta 20
#define hotThreshold_Ta 25

#define comfortThreshold 32
#define coldThreshold 30
#define hotThreshold 33

#define bodyFaceDiff 4

extern float Ta;
extern float threshold;
extern unsigned int captureDelay;

extern TaskHandle_t TaskMLX;
extern TaskHandle_t TaskMQTTPublish;
extern TaskHandle_t TaskMQTTPublish;

