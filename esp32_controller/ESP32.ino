#include <WiFi.h>
#include <WiFiUdp.h>
#include <ESP32Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// ================= WIFI ================= //Change the WIFI name and password
const char* ssid = "demo";
const char* password = "123456789";
const int UDP_PORT = 8888;
WiFiUDP udp;

// ================= PINS ================= //Change the pins based on your ESP pins
#define SERVO1_PIN 2
#define SERVO2_PIN 4
#define TRIG_PIN 14
#define ECHO_PIN 33
#define SS_PIN 5
#define RST_PIN 22
#define GREEN_LED 17
#define YELLOW_LED 16
#define RED_LED 15
#define BUZZER_PIN 12
#define I2C_SDA 21
#define I2C_SCL 25

// ================= OBJECTS =================
Servo servo1, servo2;
MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 columns, 2 rows

// ================= CONSTANTS =================
const int OPEN_ANGLE = 0;
const int CLOSED_ANGLE = 90;
const float SAFE_DISTANCE_CM = 90.0;
const String EMERGENCY_UID = "D1 05 39 03"; //change the EMERGENCY_UID based on your RFID Tag
const unsigned long EMERGENCY_DURATION = 8000;

// ================= STATE =================
int currentAngle = CLOSED_ANGLE;
bool inEmergency = false;
unsigned long emergencyStart = 0;
float currentCongestion = 0.0;

// ================= DISTANCE =================
float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 35000);
  if (duration == 0) return 999;

  return duration * 0.034 / 2.0;
}

// ================= LCD =================
void updateLCD(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void displayCongestion(float congestion) {
  currentCongestion = congestion;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Congestion:");
  lcd.setCursor(0, 1);
  lcd.print(String(congestion, 1) + "%");
}

void displayEmergencyTimer(int seconds) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("!! EMERGENCY !!");
  lcd.setCursor(0, 1);
  lcd.print("Timer: ");
  lcd.print(seconds);
  lcd.print("s");
}

void displayClosingWarning() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("!! WARNING !!");
  lcd.setCursor(0, 1);
  lcd.print("Barrier Closing");
}

// ================= SERVO =================
void moveSmooth(int target) {
  int step = (currentAngle < target) ? 1 : -1;
  while (currentAngle != target) {
    currentAngle += step;
    servo1.write(currentAngle);
    servo2.write(currentAngle);
    delay(6);
  }
}

void openBarrier() {
  if (inEmergency) return;
  moveSmooth(OPEN_ANGLE);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);
  
  updateLCD("Barrier OPEN", "Congestion:" + String(currentCongestion, 1) + "%");
}

void closeBarrierSafe() {
  displayClosingWarning();
  
  while (getDistance() < SAFE_DISTANCE_CM) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, HIGH);

    tone(BUZZER_PIN, 1000, 150);
    delay(150);
    noTone(BUZZER_PIN);
    delay(150);
  }

  digitalWrite(YELLOW_LED, LOW);

  moveSmooth(CLOSED_ANGLE);
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  
  updateLCD("Barrier CLOSED", "Congestion:" + String(currentCongestion, 1) + "%");
}


// ================= BUZZER =================
void emergencySiren(unsigned long duration) {
  unsigned long start = millis();
  while (millis() - start < duration) {
    tone(BUZZER_PIN, 1800, 200);
    delay(200);
    tone(BUZZER_PIN, 1200, 200);
    delay(200);
  }
  noTone(BUZZER_PIN);
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(I2C_SDA, I2C_SCL);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  moveSmooth(CLOSED_ANGLE);

  lcd.init();
  lcd.backlight();
  updateLCD("Smart Barrier", "Initializing...");
  delay(2000);

  SPI.begin(18, 19, 23, SS_PIN);
  delay(100);
  rfid.PCD_Init();
  delay(100);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  updateLCD("Connecting to", "WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  Serial.print("\nESP32 IP: ");
  Serial.println(WiFi.localIP());

  updateLCD("WiFi Connected", WiFi.localIP().toString());
  delay(2000);

  udp.begin(UDP_PORT);
  Serial.println("UDP ready");
  
  updateLCD("System Ready", "Barrier CLOSED");
  
  // Final LED state
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
}

// ================= LOOP =================
void loop() {

  // ===== UDP =====
  int packetSize = udp.parsePacket();
  if (packetSize) {
    IPAddress ip = udp.remoteIP();
    uint16_t port = udp.remotePort();

    char buf[64];
    int len = udp.read(buf, sizeof(buf) - 1);
    buf[len] = '\0';
    String cmd = String(buf);
    cmd.trim();

    if (cmd == "STATUS") {
      udp.beginPacket(ip, port);
      udp.print(currentAngle == OPEN_ANGLE ? "STATUS:OPEN" : "STATUS:CLOSED");
      udp.endPacket();
    }
    else if (cmd == "DIST") {
      float d = getDistance();
      udp.beginPacket(ip, port);
      udp.print("DIST:" + String(d));
      udp.endPacket();
    }
    else if (cmd.startsWith("CONGESTION:")) {
      // Receive congestion data from Python
      String congestionStr = cmd.substring(11);
      currentCongestion = congestionStr.toFloat();
      if (!inEmergency) {
        displayCongestion(currentCongestion);
      }
    }
    else if (cmd == "CMD:OPEN") openBarrier();
    else if (cmd == "CMD:CLOSE") closeBarrierSafe();
  }

  // ===== RFID =====
  if (!inEmergency) {
    if (rfid.PICC_IsNewCardPresent()) {
      if (rfid.PICC_ReadCardSerial()) {
        String uid = "";
        for (byte i = 0; i < rfid.uid.size; i++) {
          if (rfid.uid.uidByte[i] < 0x10) uid += "0";
          uid += String(rfid.uid.uidByte[i], HEX);
          if (i < rfid.uid.size - 1) uid += " ";
        }
        uid.toUpperCase();

        Serial.print("RFID UID: ");
        Serial.println(uid);

        if (uid == EMERGENCY_UID) {
          inEmergency = true;
          digitalWrite(YELLOW_LED, HIGH);
          
          updateLCD("!! EMERGENCY !!", "Siren Active");
          emergencySiren(3000);

          if (currentAngle == OPEN_ANGLE) {
            updateLCD("!! EMERGENCY !!", "Waiting Clear...");
            
            while (getDistance() < SAFE_DISTANCE_CM) {
              digitalWrite(RED_LED, HIGH);
              tone(BUZZER_PIN, 1000, 150);
              delay(150);
              noTone(BUZZER_PIN);
              delay(150);
            }
            
            closeBarrierSafe();
          } else {
            updateLCD("!! EMERGENCY !!", "Already Closed");
            delay(1000);
          }
          
          emergencyStart = millis();
        }

        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
      }
    }
  }

  if (inEmergency) {
    unsigned long elapsed = millis() - emergencyStart;
    
    if (elapsed < EMERGENCY_DURATION) {
      unsigned long remaining = EMERGENCY_DURATION - elapsed;
      int secondsLeft = (remaining / 1000) + 1;
      displayEmergencyTimer(secondsLeft);
      delay(100);
    } else {
      inEmergency = false;
      digitalWrite(YELLOW_LED, LOW);
      displayCongestion(currentCongestion);
    }
  } else {
    static unsigned long lastLCDUpdate = 0;
    if (millis() - lastLCDUpdate > 500) {
      if (currentCongestion > 0) {
        displayCongestion(currentCongestion);
      }
      lastLCDUpdate = millis();
    }
  }
}