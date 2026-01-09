import socket
import time
import threading
import cv2
from ultralytics import YOLO

# Config
ESP32_IP = "192.168.137.88"     # ESP32-Main URL
ESP32_PORT = 8888 #ESP32 UDP_PORT

ESP32_CAM_URL = "http://192.168.137.208:81/stream"  # ESP32-cam URL
YOLO_MODEL_PATH = "best.pt" #YOLO Custom Model Path

LANES_CLOSED = 3 #Number of lanes before opening the barrier
LANES_OPEN = 4 #Number of lanes after opening the barrier
CAPACITY_PER_LANE = 3 #lane Capacity

OPEN_THRESHOLD = 60     # %
CLOSE_THRESHOLD = 40    # %

# State
barrier_state = "UNKNOWN!!!"
vehicle_count = 0

# UDP 
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("", 0))
sock.settimeout(0.2)

def send(msg):
    if msg is None:
        return
    sock.sendto(msg.encode(), (ESP32_IP, ESP32_PORT))

# Receive ESP32
def receive_loop():
    global barrier_state
    while True:
        try:
            data, _ = sock.recvfrom(1024)
            msg = data.decode().strip()

            if msg == "STATUS:OPEN":
                barrier_state = "OPEN"
            elif msg == "STATUS:CLOSED":
                barrier_state = "CLOSED"

        except socket.timeout:
            pass

threading.Thread(target=receive_loop, daemon=True).start()

# YOLO
model = YOLO(YOLO_MODEL_PATH)
cap = cv2.VideoCapture(ESP32_CAM_URL)

if not cap.isOpened():
    raise RuntimeError("Cann't open ESP32-CAM URL")


def calculate_congestion(vehicles, lanes):
    return (vehicles / (lanes * CAPACITY_PER_LANE)) * 100

def decide(congestion):
    """
    Hysteresis-based decision:
    - >= 60%  -> OPEN
    - <= 40%  -> CLOSE
    """
    if congestion >= OPEN_THRESHOLD and barrier_state != "OPEN":
        return "CMD:OPEN"

    if congestion <= CLOSE_THRESHOLD and barrier_state != "CLOSED":
        return "CMD:CLOSE"

    return None

# Main loop
last_request = 0

while True:
    now = time.time()

    # Ask ESP32 for status periodically
    if now - last_request > 0.5:
        send("STATUS")
        last_request = now

    # Read camera frame
    ret, frame = cap.read()
    if not ret:
        continue

    # YOLO inference
    results = model(frame, verbose=False)
    boxes = results[0].boxes
    vehicle_count = len(boxes)

    # Choose lane count based on barrier state
    lanes = LANES_OPEN if barrier_state == "OPEN" else LANES_CLOSED

    congestion = calculate_congestion(vehicle_count, lanes)

    cmd = decide(congestion)
    send(cmd)

    # Display
    annotated = results[0].plot()
    cv2.imshow("ESP32-CAM YOLO", annotated)

    print(
        f"Barrier={barrier_state:<6} | "
        f"Vehicles={vehicle_count:<2} | "
        f"Lanes={lanes} | "
        f"Congestion={congestion:5.1f}% | "
        f"Action={cmd}"
    )

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
