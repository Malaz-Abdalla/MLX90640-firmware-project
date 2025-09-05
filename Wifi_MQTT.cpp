#include <string>
#include "Wifi_MQTT.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "analysis.h"
#include "config.h"
#include "MRI.h"

// Wi-Fi credentials
const char* ssid = "Oasi";
const char* password = "Capriolo-2020";

// MQTT configuration
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "test/malaz/AvgFaceTemp";

// Wi-Fi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);


void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");
}

void connectToMQTT() {
  client.setServer(mqtt_server, mqtt_port);
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32_FaceTempPublisher",
                       mqtt_topic, 0, true,
                       "ESP32 disconnected")) {
      Serial.println("Connected to MQTT!");
    } else {
      Serial.print("Failed. State=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void syncTime() { // a configuration method that lives in the set up
  configTzTime("CET-1CEST,M3.5.0/2,M10.5.0/3", "pool.ntp.org");
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)){
      Serial.println("Waiting for time...");
    delay(1000);
  }
  Serial.println ("time synced");
}

void generateTimestamp( mostRecentinfo& someStruct){
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)){
      Serial.println("Failed to obtain time");
    return;
  }
  strftime(someStruct.mostRecentTimestamp, sizeof(someStruct.mostRecentTimestamp), "%Y-%m-%d %H:%M:%S", &timeinfo);
  }

// void LoadingPayloadAndPublish(){

//   generateTimestamp();

//   StaticJsonDocument<1024> doc; //256

//   doc["FaceAvgTemp"] = MRFI.mostRecentAvg;
//   doc["FaceMinTemp"] = MRFI.mostRecentMin;
//   doc["FaceMaxTemp"] = MRFI.mostRecentMax;
//   doc["FaceStandardDiviation"] = MRFI.mostRecentSD;
//   doc["FaceTimeStamp"] = MRFI.mostRecentSD;

//   doc["WallAvgTemp"] = MRWI.mostRecentAvg;
//   doc["WallMinTemp"] = MRWI.mostRecentMin;
//   doc["WallMaxTemp"] = MRWI.mostRecentMax;
//   doc["WallStandardDiviation"] = MRWI.mostRecentSD;
//   doc["WallTimeStamp"] = MRWI.mostRecentSD;
//   char jsonBuffer[256];
//   serializeJson(doc, jsonBuffer); 
//   client.publish(mqtt_topic, jsonBuffer, true); // `true` = retain    
// }

void LoadingPayloadAndPublishTask(void* pvParameters){
  while (true){
    face = faceAvgsAcc / completeDuration;
    faceMin = faceMinAcc / completeDuration;
    faceMax = faceMaxAcc / completeDuration;

    wall = wallAvgsAcc / completeDuration;
    wallMin = wallMinAcc / completeDuration;
    wallMax = wallMaxAcc / completeDuration;
    
    MRFI.mostRecentAvg = face;
    MRFI.mostRecentMin = faceMin;
    MRFI.mostRecentMax = faceMax;
    MRFI.mostRecentSD = faceStandardDeviation;
    generateTimestamp(MRFI);

    //min max etc
    MRWI.mostRecentAvg = wall;
    MRWI.mostRecentMin = wallMin;
    MRWI.mostRecentMax = wallMax;
    MRWI.mostRecentSD = wallStandardDeviation;
    generateTimestamp(MRWI);

    StaticJsonDocument<1024> doc; //256

    doc["FaceAvg"] = MRFI.mostRecentAvg;
    doc["FaceMin"] = MRFI.mostRecentMin;
    doc["FaceMax"] = MRFI.mostRecentMax;
    doc["FaceStandardDiviation"] = MRFI.mostRecentSD;
    doc["FaceTimeStamp"] = MRFI.mostRecentTimestamp;

    doc["WallAvg"] = MRWI.mostRecentAvg;
    doc["WallMin"] = MRWI.mostRecentMin;
    doc["WallMax"] = MRWI.mostRecentMax;
    doc["WallStandardDiviation"] = MRWI.mostRecentSD;
    doc["WallTimeStamp"] = MRWI.mostRecentTimestamp;


    char jsonBuffer[1024];
    serializeJson(doc, jsonBuffer); 
    client.publish(mqtt_topic, jsonBuffer, true); // `true` = retain     

    faceAvgsAcc = wallAvgsAcc = face = faceMin = faceMax = wall = wallMin = wallMax = wallMinAcc = wallMaxAcc = faceMinAcc = faceMaxAcc = 0;
   
    vTaskDelay(pdMS_TO_TICKS(SECONDS_to_MS * completeDuration));  //match the frequency of the camera
  }
}
