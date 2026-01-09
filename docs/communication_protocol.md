# Communication Protocol

- Protocol: UDP
- Port: 8888
- Commands:
  - "OPEN": Open the dynamic lane
  - "CLOSE": Close the dynamic lane
  - "DIST": Send the distance between the ultrasonic and any object above the barrier
  - "STATUS": Send the current status of the barrier (open or closed)

UDP was selected due to its low latency and suitability for real-time control applications.