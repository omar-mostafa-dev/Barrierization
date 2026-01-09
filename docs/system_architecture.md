# System Architecture

The system consists of three main subsystems:
1. Vision subsystem (ESP32-CAM + Python)
2. Decision subsystem (Python)
3. Actuation subsystem (ESP32 Controller)

Data flows from the camera to the AI system, where traffic conditions are analyzed and control decisions are transmitted wirelessly to the road actuator.