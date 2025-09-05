# ESP32 Thermal Camera with MQTT Communication

This project implements a thermal imaging and IoT communication system using an ESP32 microcontroller and the MLX90640 thermal camera.
The ESP32 captures thermal data, processes it, and publishes results via MQTT for real-time monitoring.
---

## Features

- **Thermal Imaging** using the MLX90640_camera module (I²C interface).
- **Wi-Fi & MQTT Communication** through the Wifi_MQTT module.
- **System Monitoring** with SYSTEM_MONITOR.
- **Data Processing** via analysis module (temperature metrics).
- **Helper Functions** in utils module for general operations.
- **Serial Debugging** over both Micro-USB (Serial0) and Type-C (native Serial).

---

## Hardware Requirements
- ESP32-S3-DevKitC-1 Development Board  
- Adafruit MLX90640 IR Thermal Camera  
- Type-C cable (Native USB)  
- Wi-Fi connection  
- MQTT Broker (e.g., Mosquitto, HiveMQ)

---

## Project Structure 
├── main.ino # Main entry point

├── config.h # Configuration (Wi-Fi, MQTT, offsets, etc.)

├── MLX90640_camera.cpp/h # Thermal camera driver & interface

├── Wifi_MQTT.cpp/h # Wi-Fi connection & MQTT publish

├── analysis.cpp/h # Temperature data analysis

├── utils.cpp/h # Helper functions

└── README.md # Project documentation

---

---

## Getting Started

### 1. Clone Repository
```bash
git clone https://github.com/yourusername/esp32-thermal-mqtt.git
cd esp32-thermal-mqtt
```
### 2. Open in Arduino IDE
- Open ```main.ino``` in Arduino IDE (or PlatformIO).
- Select ESP32 board under Tools > Board.
- Install required libraries:
- ```Adafruit MLX90640```
- ```WiFi.h```
- ```PubSubClient.h```
