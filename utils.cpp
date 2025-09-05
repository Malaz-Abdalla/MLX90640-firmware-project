#include "utils.h"
#include "MLX90640_camera.h"
#include "config.h"
#include "analysis.h"

float Ta = 0;
float threshold = 0;
unsigned int captureDelay = 10 * SECONDS_to_MS;

float calculateThreshold(float AmbientTemperature) {
  Ta = AmbientTemperature;
  if (Ta >= hotThreshold_Ta) return threshold = hotThreshold;
  if (Ta >= comfortThreshold_Ta) return threshold = comfortThreshold;
  return threshold =  coldThreshold;
}

float computeAverage(const std::vector<int>& indices) {
  if (indices.empty()) return 0;
  float sum = 0;
  for (int idx : indices) 
    sum += frameComp[idx] / 10.0;
  return sum / indices.size();
}

float completeWallAverage() {
  float wallSum = 0;
  int wallCount = 0;
  for (int i = 0; i < 768; i++) {
    wallSum += frameComp[i] / 10.0;
    wallCount++;
  }
    wallAvgsAcc += wallSum / wallCount;
  return wallCount ? wallSum / wallCount : 0;
}

void completeWallMinMax() {
  float temp=0, wallMinTemp = 1000.0 , wallMaxTemp =-1000.0 ;
  for (int i = 0; i < 768; i++) {
    temp = frameComp[i] / 10.0;    
    if (temp < wallMinTemp) wallMinTemp = temp;
    if (temp > wallMaxTemp) wallMaxTemp = temp;      
  }
    wallMinAcc += wallMinTemp/ 10.0;
    wallMaxAcc += wallMaxTemp/ 10.0;
    Serial.print("TempMin of the wall:");
    Serial.println(wallMinTemp);
    Serial.print("TempMax of the wall:");
    Serial.println(wallMaxTemp);  
}

float calculateStandardDeviation (std::vector <float>& prevAvgs){
  float SD =0;
  if (prevAvgs.size() == 3){
      float SDSum = 0;
      for (float t : prevAvgs){
        SDSum += t ;
      }
      float SDmean =  SDSum /3;    
      float variance =0;

      for (float t : prevAvgs){
        variance += (t - SDmean) * (t - SDmean) ;
      }   
      SD = sqrt (variance / 3);
    }
    return SD;  
}
