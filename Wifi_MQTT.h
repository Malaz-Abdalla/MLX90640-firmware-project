#pragma once

#include "MRI.h"
#include <WiFi.h>
#include <PubSubClient.h>

// #include "analysis.h"

void connectToWiFi();
void connectToMQTT();
void syncTime();
// char* generateTimestamp();

void generateTimestamp( mostRecentinfo& someStruct);
void LoadingPayloadAndPublishTask(void *pvParameters);

// Wi-Fi and MQTT clients
extern PubSubClient client;
