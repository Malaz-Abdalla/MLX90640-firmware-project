#include <Adafruit_MLX90640.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "Wifi_MQTT.h"
// #include <ArduinoJson.h>
// #include <time.h>

#include "config.h"
#include "MLX90640_camera.h"
#include "analysis.h"
#include "utils.h"
// #include "USB.h"

// // Time config
// const char* ntpServer = "pool.ntp.org";
// const long gmtOffset_sec = 3600;       // Change for your timezone
// const int daylightOffset_sec = 3600;   // DST

TaskHandle_t TaskMLX = NULL;
TaskHandle_t TaskComplete = NULL;
TaskHandle_t TaskMQTTPublish = NULL;


void setup() {
  Serial.begin(115200);
  // Serial1.begin(115200);    // Native USB port
  Wire.begin(1, 0, 400000);
  initSensor();
  
  client.setBufferSize(4096);
  connectToWiFi();          // 1. Connect to Wi-Fi 
  //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);  // 2. Setup NTP time
  syncTime();
  connectToMQTT();          // 3. Connect to MQTT broker
  //client.setCallback(callback);  // to subscribe

  xTaskCreatePinnedToCore(
    frameAndBlobTask,
    "FrameBlobTask",
    8192,
    NULL,
    1,
    &TaskMLX,
    1
  );

  // xTaskCreatePinnedToCore(
  //   completeDurationTask,
  //   "completeDurationTask",
  //   4096,
  //   NULL,
  //   1,
  //   &TaskComplete,
  //   1
  // );

  //Create the MQTT Publishing Task
  xTaskCreatePinnedToCore(
    LoadingPayloadAndPublishTask,
    "LoadingPayloadAndPublishTask",
    4096,
    NULL,
    1,
    &TaskMQTTPublish,
    0
  );
}

void loop() {
  if (!client.connected()) {
    connectToMQTT();
  }
  client.loop();

  //emissivity and Ta Shift input 
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');  // Read a line
    input.trim();  // Remove whitespace and newline

    if (input.startsWith("e")) {
      float evalue = input.substring(input.indexOf(' ') + 1).toFloat();   
      Serial.print("[DEBUG] Emissivity set to: ");
      Serial.println(evalue);
      mlx.setEmissivity(evalue);

    } else if (input.startsWith("t")) {

      // Set some variable here
      float tvalue = input.substring(input.indexOf(' ') + 1).toFloat();   
      Serial.print("[DEBUG] temperature shift set to: ");
      Serial.println(tvalue);
      mlx.setTrShift(tvalue);
    }
  }
  // Serial1.println("Hello");
  // delay (300);
}
