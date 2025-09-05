#include "analysis.h"
#include "MLX90640_camera.h"
#include "utils.h"
#include "config.h"
#include "Wifi_MQTT.h"
#include <vector>
#include <queue>
#include <algorithm>
#include <unordered_set>
#include "MRI.h"
#include "Wifi_MQTT.h"

uint8_t visited[768];

struct Point {
  int x, y;
};

std::vector<float> PrevFaceAvgs;
std::vector<float> PrevWallAvgs;

int analysis = 0;
float face = 0, faceAvgsAcc = 0, faceMin = 0, faceMinAcc = 0, faceMax = 0, faceMaxAcc = 0;
float wall = 0, wallAvgsAcc = 0, wallMin = 0, wallMinAcc = 0, wallMax = 0, wallMaxAcc = 0;
float faceStandardDeviation = 0, wallStandardDeviation = 0;

struct mostRecentinfo MRFI;
struct mostRecentinfo MRWI;

void floodFillIterative(int x, int y, std::vector<int>& blob) {
  std::queue<Point> q;
  q.push({ x, y });
  while (!q.empty()) {
    Point p = q.front();
    q.pop();
    if (p.x < 0 || p.x >= 32 || p.y < 0 || p.y >= 24) continue;
    int i = p.y * 32 + p.x;
    if (visited[i] || frame[i] <= threshold) continue;
    visited[i] = 1;
    blob.push_back(i);
    q.push({ p.x + 1, p.y });
    q.push({ p.x - 1, p.y });
    q.push({ p.x, p.y + 1 });
    q.push({ p.x, p.y - 1 });
  }
}

std::vector<int> detectFaceBlob() {
  memset(visited, 0, sizeof(visited));
  std::vector<int> largestBlob;
  int largestSize = 0;
  for (int i = 0; i < 768; i++) {
    if (!visited[i] && frameComp[i] / 10.0 > threshold) {
      std::vector<int> blob;
      floodFillIterative(i % 32, i / 32, blob);
      if (blob.size() > largestSize) {
        largestSize = blob.size();
        largestBlob = blob;
      }
    }
  }
  return largestBlob;
}

void classifyPixels(const std::vector<int>& faceBlob) {
  float faceSum = 0, wallSum = 0, bodySum = 0, wallMinTemp = 1000.0, wallMaxTemp = -1000.0;
  int wallCount = 0, bodyCount = 0;
  float bodyThreshold = threshold - bodyFaceDiff;

  for (int idx : faceBlob) faceSum += frameComp[idx] / 10.0;

  for (int i = 0; i < 768; i++) {
    if (std::find(faceBlob.begin(), faceBlob.end(), i) != faceBlob.end()) continue;
    float temp = frameComp[i] / 10.0;
    if (temp > bodyThreshold && temp <= threshold) {
      bodySum += temp;
      bodyCount++;
    } else if (temp <= bodyThreshold) {
      wallSum += temp;
      wallCount++;
      if (temp < wallMinTemp) wallMinTemp = temp;
      if (temp > wallMaxTemp) wallMaxTemp = temp;
    }
  }

  if (bodyCount > 0) {
    float bodyAvgTemp = (bodySum + faceSum) / (bodyCount + faceBlob.size());
    Serial.print("BodyAvgTemp: ");
    Serial.println(bodyAvgTemp);
  }

  if (wallCount > 0) {

    float wallAvgTemp = wallSum / wallCount;
    wallAvgsAcc += wallAvgTemp;
    wallMinAcc += wallMinTemp/ 10.0;
    wallMaxAcc += wallMaxTemp/ 10.0;
    //float wallStandardDeviation = 0;
    Serial.print("WallAvgTemp: ");
    Serial.println(wallAvgTemp);
    Serial.print("WallMinTemp: ");
    Serial.println(wallMinTemp);
    Serial.print("WallMaxTemp: ");
    Serial.println(wallMaxTemp);


    PrevWallAvgs.push_back(wallAvgTemp);
    if (PrevWallAvgs.size() > 3) {
      PrevWallAvgs.erase(PrevWallAvgs.begin());
    }
    wallStandardDeviation = calculateStandardDeviation(PrevWallAvgs);
    Serial.print("WallAvgTemp: ");
    Serial.println(wallAvgTemp);
    completeWallMinMax();

    if (PrevWallAvgs.size() == 3) {
      Serial.print("wall standard deviation: ");
      Serial.println(wallStandardDeviation);
    }
  }
}

int analyzeBlob() {
  auto largestBlob = detectFaceBlob();
  if (!largestBlob.empty()) {
    // float faceStandardDeviation = 0;
    float faceAvgTemp = computeAverage(largestBlob);
    float faceMinTemp = 1000.0 , faceMaxTemp = -1000.0;
    for (int idx : largestBlob){ 
      float temp = frameComp[idx] / 10.0;      
      if (temp < faceMinTemp) faceMinTemp = temp;
      if (temp > faceMaxTemp) faceMaxTemp = temp;
    }  
    faceAvgsAcc += faceAvgTemp;
    faceMinAcc += faceMinTemp;
    faceMaxAcc += faceMaxTemp;

    PrevFaceAvgs.push_back(faceAvgTemp);
    if (PrevFaceAvgs.size() > 3) {
      PrevFaceAvgs.erase(PrevFaceAvgs.begin());
    }
    faceStandardDeviation = calculateStandardDeviation(PrevFaceAvgs);

    Serial.print("faceAvgTemp: ");
    Serial.println(faceAvgTemp);
    Serial.print("faceMinTemp: ");
    Serial.println(faceMinTemp);
    Serial.print("faceMaxTemp: ");
    Serial.println(faceMaxTemp);
    if (PrevFaceAvgs.size() == 3) {
      Serial.print("face standard deviation: ");
      Serial.println(faceStandardDeviation);
    }

    classifyPixels(largestBlob);
    return 2;
  } else {
    // float wallStandardDeviation = 0;
    float wallAvgTemp = completeWallAverage();

    PrevWallAvgs.push_back(wallAvgTemp);
    if (PrevWallAvgs.size() > 3) {
      PrevWallAvgs.erase(PrevWallAvgs.begin());
    }
    wallStandardDeviation = calculateStandardDeviation(PrevWallAvgs);

    Serial.println("No face blob detected.");
    Serial.print("WallAvgTemp: ");
    Serial.println(wallAvgTemp);
    completeWallMinMax();



    if (PrevWallAvgs.size() == 3) {
      Serial.print("wall standard deviation: ");
      Serial.println(wallStandardDeviation);
    }
    return 1;
  }
  return 0;
}

void frameAndBlobTask(void* pvParameters) {
  while (true) {
    analysis = 0;
    threshold = calculateThreshold(25);  // or dynamic Ta
    if (captureFrame()) {
      analysis = analyzeBlob();
    } else {
      Serial.println("Failed to Capture Frame.");
    }
    vTaskDelay(pdMS_TO_TICKS(SECONDS_to_MS / 2));  //match the frequency of the camera
    // vTaskDelay(pdMS_TO_TICKS(sec * 10)); // every 10 seconds
  }
}

void completeDurationTask(void* pvParameters) {
  while (true) {
    //if ( readings >= completeDuration;)// if values reach and exceed 10 readings
    face = faceAvgsAcc / completeDuration;
    faceMin = faceMinAcc / completeDuration;
    faceMax = faceMaxAcc / completeDuration;

    wall = wallAvgsAcc / completeDuration;
    wallMin = wallMinAcc / completeDuration;
    wallMax = wallMaxAcc / completeDuration;

    if (analysis == 2) {
      // a face and a wall
      // Serial.print("Face and wall of 5 seconds = ");
      // Serial.println(face);
      // Serial.println(wall);

    } else if (analysis == 1) {
      // only wall
      wall = wallAvgsAcc / completeDuration;
      // Serial.print("wall of 5 seconds = ");
      // Serial.println(wall);
      //min max etc
      // MRWI.mostRecentAvg = wall;
      // MRWI.mostRecentMin = wallMin;
      // MRWI.mostRecentMax = wallMax;
      // MRWI.mostRecentSD = wallStandardDeviation;
      // MRWI.mostRecentTimestamp = generateTimestamp();

    } else {
      Serial.println("Failed to Capture Frame.");
    }
    //face = faceAvgsAcc = wall = wallAvgsAcc = 0;
    faceAvgsAcc = wallAvgsAcc = faceMin = faceMax = wall = wallMin = wallMax = 0;
    //generateTimestamp();
    vTaskDelay(pdMS_TO_TICKS(completeDuration * SECONDS_to_MS));  // every 5 seconds
  }
}
