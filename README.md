# Barrierization

## Overview
The Intelligent Road System is a smart traffic management project that uses computer vision and embedded systems to reduce traffic congestion and give priority to emergency vehicles. The system analyzes real-time road traffic and dynamically opens or closes an extra lane using a smart retractable barrier.

---

## Problem
Traditional roads use fixed lanes, which causes:
- High traffic congestion
- Delayed emergency vehicle response
- Inefficient use of road space

---

## Solution
This project uses:
- A camera to monitor traffic
- Artificial intelligence to analyze congestion
- Wireless communication to control a smart barrier
- Safety and emergency override mechanisms

The road adapts automatically based on real-time conditions.

---

## System Components

| Component | Role |
|----------|------|
| ESP32-CAM | Streams live road video |
| Python + YOLO | Detects vehicles and congestion |
| ESP32 Controller | Controls barrier and safety logic |
| Servos | Opens or closes lanes |
| RFID | Detects emergency vehicles |
| Ultrasonic Sensor | Prevents unsafe barrier closing |

---

## Hardware Used
- ESP32-CAM
- ESP32 Development Board
- Servo or Linear Actuator
- Ultrasonic Sensor
- RFID Module
- LEDs
- Buzzer

---

## Requirements

This section explains **everything needed** to run the project successfully.
Make sure all requirements are met before running the system.

---

## Hardware Requirements

### Required Components
- **ESP32-CAM**
  - Used for real-time traffic video streaming
- **ESP32 Development Board**
  - Used for barrier control and decision execution
- **Servo Motor or Linear Actuator**
  - Controls the opening and closing of the road barrier
- **Ultrasonic Sensor**
  - Ensures safe barrier movement by detecting vehicles
- **RFID Module (MFRC522)**
  - Detects emergency vehicles
- **LEDs**
  - Visual indication of lane status
- **Buzzer**
  - Make a small noise to alert to prevent accidents
- **Power Supply**
  - Stable power for servos

---

## Software Requirements

### Operating System
- Windows / Linux / macOS

### Python Environment
- **Python 3.8 or higher**

Required Python libraries:
- `opencv-python` → Video stream processing
- `ultralytics` → YOLO object detection

Install all libraries using:
```bash
pip install -r "Library"
```
### Arduino Environment

- Arduino IDE
- ESP32 Board Package installed

Required Arduino libraries:

- ESP32Servo
- MFRC522
- LiquidCrystal_I2C
- WiFi
- WiFiUdp

These libraries can be installed from the Arduino Library Manager.

### Network Requirements

All devices must be connected to the same Wi-Fi network:

- ESP32-CAM
- ESP32 Controller
- Computer running Python


---


## Files Structure
```
python_vision/
├── final.py
├── best.pt
└── requirements.txt

esp32_controller/
└── sketch_jan4a.ino

esp32_cam/
└── ESP32-CAM streaming files
```
