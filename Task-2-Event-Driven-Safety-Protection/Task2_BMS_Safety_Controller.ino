#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================= PIN CONFIGURATION =================

const int cellPins[4] = {34, 35, 32, 33};

const int RELAY_PIN = 25;
const int BUZZER_PIN = 26;

// ================= SAFETY LIMITS =================

const float MIN_CELL_VOLTAGE = 3.0;
const float MAX_CELL_VOLTAGE = 4.2;

// Rapid change threshold
const float RAPID_CHANGE_LIMIT = 0.50;

// ================= TIMING =================

const unsigned long SENSOR_INTERVAL = 200;
const unsigned long BUZZER_INTERVAL = 500;
const unsigned long LCD_INTERVAL = 500;

// Battery must remain safe for this long before recovery
const unsigned long RECOVERY_TIME = 5000;

// ================= STATES =================

enum SafetyState {
  NORMAL,
  WARNING,
  FAULT,
  RECOVERY
};

SafetyState state = NORMAL;

// ================= VARIABLES =================

float cellVoltage[4] = {3.7, 3.7, 3.7, 3.7};
float previousVoltage[4] = {3.7, 3.7, 3.7, 3.7};

bool weakCell = false;
bool overVoltage = false;
bool sensorFault = false;
bool rapidChange = false;

unsigned long lastSensorTime = 0;
unsigned long lastBuzzerTime = 0;
unsigned long lastLCDTime = 0;
unsigned long recoveryStartTime = 0;

// ================= SETUP =================

void setup() {

  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Start in safe state
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(BUZZER_PIN, LOW);

  Wire.begin(21, 22);

  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BMS SAFETY");
  lcd.setCursor(0, 1);
  lcd.print("INITIALIZING");

  // Give sensors time to stabilize
  unsigned long startTime = millis();

  while (millis() - startTime < 1000) {
    // Non-blocking requirement applies to the main system;
    // no delay() is used.
  }

  lcd.clear();
}

// ================= MAIN LOOP =================

void loop() {

  unsigned long currentMillis = millis();

  // ==================================================
  // SENSOR MONITORING
  // ==================================================

  if (currentMillis - lastSensorTime >= SENSOR_INTERVAL) {

    lastSensorTime = currentMillis;

    weakCell = false;
    overVoltage = false;
    sensorFault = false;
    rapidChange = false;

    for (int i = 0; i < 4; i++) {

      int rawValue = analogRead(cellPins[i]);

      // Convert ADC value to simulated cell voltage
      cellVoltage[i] = (rawValue / 4095.0) * 4.5;

      // ---------------- SENSOR ANOMALY ----------------

      if (cellVoltage[i] < 0.0 ||
          cellVoltage[i] > 4.5) {

        sensorFault = true;
      }

      // ---------------- RAPID CHANGE ----------------

      if (abs(cellVoltage[i] - previousVoltage[i])
          > RAPID_CHANGE_LIMIT) {

        rapidChange = true;
      }

      // ---------------- WEAK CELL ----------------

      if (cellVoltage[i] < MIN_CELL_VOLTAGE) {

        weakCell = true;
      }

      // ---------------- OVERVOLTAGE ----------------

      if (cellVoltage[i] > MAX_CELL_VOLTAGE) {

        overVoltage = true;
      }

      previousVoltage[i] = cellVoltage[i];
    }
  }

  // ==================================================
  // SAFETY STATE MACHINE
  // ==================================================

  switch (state) {

    // =================================================
    // NORMAL
    // =================================================

    case NORMAL:

      digitalWrite(RELAY_PIN, HIGH);
      digitalWrite(BUZZER_PIN, LOW);

      if (sensorFault ||
          rapidChange ||
          overVoltage) {

        state = FAULT;
      }

      else if (weakCell) {

        state = WARNING;
      }

      break;


    // =================================================
    // WARNING
    // =================================================

    case WARNING:

      // Battery still connected
      digitalWrite(RELAY_PIN, HIGH);

      // Non-blocking buzzer
      if (currentMillis - lastBuzzerTime >= BUZZER_INTERVAL) {

        lastBuzzerTime = currentMillis;

        digitalWrite(
          BUZZER_PIN,
          !digitalRead(BUZZER_PIN)
        );
      }

      // Critical condition
      if (sensorFault ||
          rapidChange ||
          overVoltage) {

        state = FAULT;
      }

      // Weak cell recovered
      else if (!weakCell) {

        state = RECOVERY;
        recoveryStartTime = currentMillis;
      }

      break;


    // =================================================
    // FAULT
    // =================================================

    case FAULT:

      // Safety cutoff
      digitalWrite(RELAY_PIN, LOW);

      // Continuous alarm
      digitalWrite(BUZZER_PIN, HIGH);

      // All conditions must be safe
      if (!sensorFault &&
          !rapidChange &&
          !overVoltage &&
          !weakCell) {

        state = RECOVERY;

        recoveryStartTime = currentMillis;
      }

      break;


    // =================================================
    // RECOVERY
    // =================================================

    case RECOVERY:

      // Keep relay OFF during recovery
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);

      // Fault returned
      if (sensorFault ||
          rapidChange ||
          overVoltage ||
          weakCell) {

        state = FAULT;

        recoveryStartTime = 0;
      }

      // Battery remained safe long enough
      else if (currentMillis - recoveryStartTime
               >= RECOVERY_TIME) {

        state = NORMAL;
      }

      break;
  }

  // ==================================================
  // LCD DISPLAY
  // ==================================================

  if (currentMillis - lastLCDTime >= LCD_INTERVAL) {

    lastLCDTime = currentMillis;

    lcd.clear();

    switch (state) {

      // ---------------- NORMAL ----------------

      case NORMAL:

        lcd.setCursor(0, 0);
        lcd.print("SYSTEM NORMAL");

        lcd.setCursor(0, 1);
        lcd.print("RELAY: ON");

        break;


      // ---------------- WARNING ----------------

      case WARNING:

        lcd.setCursor(0, 0);
        lcd.print("WARNING");

        lcd.setCursor(0, 1);
        lcd.print("WEAK CELL");

        break;


      // ---------------- FAULT ----------------

      case FAULT:

        lcd.setCursor(0, 0);
        lcd.print("SAFETY FAULT");

        lcd.setCursor(0, 1);

        if (overVoltage) {

          lcd.print("OVERVOLTAGE");
        }

        else if (sensorFault) {

          lcd.print("SENSOR ERROR");
        }

        else if (rapidChange) {

          lcd.print("RAPID CHANGE");
        }

        else if (weakCell) {

          lcd.print("WEAK CELL");
        }

        else {

          lcd.print("RELAY: OFF");
        }

        break;


      // ---------------- RECOVERY ----------------

      case RECOVERY:

        lcd.setCursor(0, 0);
        lcd.print("RECOVERY");

        lcd.setCursor(0, 1);
        lcd.print("CHECKING...");

        break;
    }
  }

  // ==================================================
  // SERIAL MONITOR
  // ==================================================

  static unsigned long lastSerialTime = 0;

  if (currentMillis - lastSerialTime >= 1000) {

    lastSerialTime = currentMillis;

    Serial.print("C1: ");
    Serial.print(cellVoltage[0], 2);

    Serial.print(" | C2: ");
    Serial.print(cellVoltage[1], 2);

    Serial.print(" | C3: ");
    Serial.print(cellVoltage[2], 2);

    Serial.print(" | C4: ");
    Serial.print(cellVoltage[3], 2);

    Serial.print(" | STATE: ");

    switch (state) {

      case NORMAL:
        Serial.println("NORMAL");
        break;

      case WARNING:
        Serial.println("WARNING");
        break;

      case FAULT:
        Serial.println("FAULT");
        break;

      case RECOVERY:
        Serial.println("RECOVERY");
        break;
    }
  }
}
