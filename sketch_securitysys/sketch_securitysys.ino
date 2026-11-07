
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD Configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin Configuration
#define IR_PIN 7
#define BUZZER_PIN 11

// System Variables
unsigned long detectionTime = 0;
unsigned long lastDebugTime = 0;

bool isTracking = false;
bool isAlarming = false;
bool accessGranted = false;

// Replace this with your actual 12-character RFID card ID
String knownRFID = "0200721E83ED";

void setup() {

  // Initialize Serial Monitor
  Serial.begin(9600);
  delay(1000);

  Serial.println();
  Serial.println("================================");
  Serial.println("   HOSTEL SECURITY SYSTEM");
  Serial.println("================================");

  // Initialize RFID Reader (Arduino Mega RX1 = Pin 19)
  Serial1.begin(9600);
  Serial.println("[OK] RFID Reader Initialized");

  // Initialize Sensors and Buzzer
  pinMode(IR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);

  Serial.println("[OK] IR Sensor Initialized");
  Serial.println("[OK] Buzzer Initialized");

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.backlight();

  Serial.println("[OK] LCD Initialized");

  resetSystem();

  Serial.println("[OK] System Initialization Complete");
  Serial.println("Waiting for motion...");
  Serial.println("--------------------------------");
}

void loop() {

  int detection = digitalRead(IR_PIN);

  // ==========================================
  // 1. CONTINUOUS DEBUGGING
  // ==========================================

  if (millis() - lastDebugTime >= 1000) {

    lastDebugTime = millis();

    Serial.print("[STATUS] IR: ");
    Serial.print(detection);

    Serial.print(" | Tracking: ");
    Serial.print(isTracking);

    Serial.print(" | Alarm: ");
    Serial.print(isAlarming);

    Serial.print(" | Access: ");
    Serial.println(accessGranted);
  }

  // ==========================================
  // 2. MOTION DETECTION
  // ==========================================

  if (detection == LOW && !isTracking && !accessGranted) {

    isTracking = true;
    detectionTime = millis();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Motion Detected!");

    lcd.setCursor(0, 1);
    lcd.print("Scan ID Card...");

    Serial.println();
    Serial.println("[MOTION] Intruder detected!");
    Serial.println("[TIMER] 10-second countdown started.");
  }

  // ==========================================
  // 3. ALARM TRIGGER AFTER 10 SECONDS
  // ==========================================

  if (isTracking && !accessGranted && !isAlarming) {

    if (millis() - detectionTime >= 10000) {

      isAlarming = true;

      digitalWrite(BUZZER_PIN, HIGH);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("INTRUDER ALERT!!");

      Serial.println();
      Serial.println("[ALERT] 10 seconds elapsed!");
      Serial.println("[ALARM] BUZZER ACTIVATED!");
    }
  }

  // ==========================================
  // 4. RFID CARD SCANNING
  // ==========================================

  if (isTracking || isAlarming) {

    String scannedRFID = readRFID();

    if (scannedRFID != "") {

      Serial.print("[RFID] Card Detected: ");
      Serial.println(scannedRFID);

      if (scannedRFID == knownRFID) {

        Serial.println("[ACCESS] Authorized card!");
        grantAccess();

      } else {

        Serial.println("[ACCESS DENIED] Unauthorized card!");

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Access Denied!");

        lcd.setCursor(0, 1);
        lcd.print("Invalid Card");

        delay(2000);

        // Restore the current system display
        if (isAlarming) {

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("INTRUDER ALERT!!");

        } else {

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Motion Detected!");

          lcd.setCursor(0, 1);
          lcd.print("Scan ID Card...");
        }
      }
    }
  }

  // ==========================================
  // 5. RESET IF PERSON LEAVES
  // ==========================================

  if (detection == HIGH && isTracking &&
      !isAlarming && !accessGranted) {

    if (millis() - detectionTime > 15000) {

      Serial.println("[INFO] Area clear for 15 seconds.");
      Serial.println("[SYSTEM] Returning to standby.");

      resetSystem();
    }
  }
}

// ==========================================
// RFID READING FUNCTION
// ==========================================

String readRFID() {

  String cardID = "";

  if (Serial1.available()) {

    while (Serial1.available()) {

      char c = Serial1.read();

      // Ignore carriage return and newline
      if (c != '\r' && c != '\n') {
        cardID += c;
      }

      delay(5);
    }

    cardID.trim();

    if (cardID.length() >= 12) {

      String finalID = cardID.substring(0, 12);

      Serial.print("[RFID RAW] ");
      Serial.println(finalID);

      return finalID;
    }
  }

  return "";
}

// ==========================================
// ACCESS GRANTED FUNCTION
// ==========================================

void grantAccess() {

  accessGranted = true;
  isAlarming = false;
  isTracking = false;

  // Stop Buzzer
  digitalWrite(BUZZER_PIN, LOW);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access Granted");

  lcd.setCursor(0, 1);
  lcd.print("Welcome Sawaransh");

  Serial.println();
  Serial.println("================================");
  Serial.println("[SUCCESS] ACCESS GRANTED");
  Serial.println("[ALARM] BUZZER DEACTIVATED");
  Serial.println("[USER] Welcome Sawaransh");
  Serial.println("================================");

  delay(5000);

  resetSystem();
}

// ==========================================
// SYSTEM RESET FUNCTION
// ==========================================

void resetSystem() {

  isTracking = false;
  isAlarming = false;
  accessGranted = false;

  digitalWrite(BUZZER_PIN, LOW);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("System Active");

  lcd.setCursor(0, 1);
  lcd.print("Safe Mode");

  Serial.println();
  Serial.println("[SYSTEM] Reset to Standby");
  Serial.println("[SYSTEM] Safe Mode Active");
  Serial.println("[SYSTEM] Waiting for motion...");
  Serial.println("--------------------------------");
}
