# Algorithm Flow

### Step 1: Capture Traffic Video
The ESP32-CAM streams live video of the road over Wi-Fi for Python program.

---

### Step 2: Analyze Traffic Using AI
A Python program:
- Receives the live video stream
- Uses a YOLO model to detect vehicles
- Calculates traffic congestion percentage

---

### Step 3: Make a Decision
The system applies this logic:
- If an emergency vehicle is detected → **Open barrier**
- If congestion > 60% → **Open barrier**
- If congestion < 40% → **Close barrier**

---

### Step 4: Send Control Command
The Python program sends a wireless UDP command:
- `"OPEN"` to open the barrier
- `"CLOSE"` to close the barrier

---

### Step 5: Control the Barrier Safely
The ESP32 controller:
- Receives the command
- Checks ultrasonic distance before closing
- Moves the barrier safely
- Updates LEDs and Buzzer