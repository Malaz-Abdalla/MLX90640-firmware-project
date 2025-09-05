#include "MLX90640_camera.h"
// #include "USB.h"

Adafruit_MLX90640 mlx;
float frame[768];
int16_t frameComp[768];

void initSensor() {
  if (!mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
    while (1) delay(10);
  }
  mlx.setMode(MLX90640_CHESS);
  mlx.setResolution(MLX90640_ADC_18BIT);
  mlx.setRefreshRate(MLX90640_2_HZ);
}

bool captureFrame() {
  if (mlx.getFrame(frame) != 0) {
    Serial.println("Frame read error");
    return 0;
  }
  for (int i = 0; i < 768; i++) {
    frameComp[i] = (int16_t)(frame[i] * 10.0);
 //   Serial.print(frameComp[i]);   
  }
  return 1;
}

