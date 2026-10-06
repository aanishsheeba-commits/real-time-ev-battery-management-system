#include <WiFi.h>
#include <HTTPClient.h>

// =====================================================
// ADAPTIVE MULTI-CELL BATTERY INTELLIGENCE ENGINE
// Wokwi ESP32 + ThingSpeak
// =====================================================

// ---------------- WiFi ----------------
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

// ---------------- ThingSpeak ----------
const char* THINGSPEAK_SERVER = "http://api.thingspeak.com/update";
const char* WRITE_API_KEY = "M9DMI8AVI5Y1I7JH";

// ---------------- Cell Inputs ----------
const int CELL1_PIN = 34;
const int CELL2_PIN = 35;
const int CELL3_PIN = 32;
const int CELL4_PIN = 33;

// ---------------- Battery Limits -------
const float CELL_MIN = 3.0;
const float CELL_MAX = 4.2;

const float MINOR_IMBALANCE = 2.0;
const float CRITICAL_IMBALANCE = 5.0;

// =====================================================
// READ CELL VOLTAGE
// =====================================================

float readCellVoltage(int pin)
{
  int adcValue = analogRead(pin);

  float voltage = (adcValue / 4095.0) * 3.3;

  // Convert potentiometer value to simulated
  // lithium cell voltage: 3.0V to 4.2V
  voltage = 3.0 + (voltage / 3.3) * 1.2;

  return voltage;
}

// =====================================================
// FIND STRONGEST CELL
// =====================================================

int findStrongestCell(float cells[])
{
  int index = 0;

  for (int i = 1; i < 4; i++)
  {
    if (cells[i] > cells[index])
      index = i;
  }

  return index;
}

// =====================================================
// FIND WEAKEST CELL
// =====================================================

int findWeakestCell(float cells[])
{
  int index = 0;

  for (int i = 1; i < 4; i++)
  {
    if (cells[i] < cells[index])
      index = i;
  }

  return index;
}

// =====================================================
// BATTERY HEALTH CLASSIFICATION
// =====================================================

String classifyBattery(float cells[], float imbalance)
{
  for (int i = 0; i < 4; i++)
  {
    if (cells[i] <= CELL_MIN || cells[i] >= CELL_MAX)
    {
      return "PACK FAILURE";
    }
  }

  if (imbalance >= CRITICAL_IMBALANCE)
  {
    return "CRITICAL IMBALANCE";
  }

  if (imbalance >= MINOR_IMBALANCE)
  {
    return "MINOR IMBALANCE";
  }

  return "HEALTHY";
}

// =====================================================
// SEND DATA TO THINGSPEAK
// =====================================================

void sendToThingSpeak(
  float cell1,
  float cell2,
  float cell3,
  float cell4,
  float packVoltage,
  float averageVoltage,
  float imbalance,
  String batteryState)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    String url = String(THINGSPEAK_SERVER) +
                 "?api_key=" + WRITE_API_KEY +
                 "&field1=" + String(cell1, 3) +
                 "&field2=" + String(cell2, 3) +
                 "&field3=" + String(cell3, 3) +
                 "&field4=" + String(cell4, 3) +
                 "&field5=" + String(packVoltage, 3) +
                 "&field6=" + String(averageVoltage, 3) +
                 "&field7=" + String(imbalance, 2) +
                 "&field8=" + batteryState;

    http.begin(url);

    int httpResponseCode = http.GET();

    Serial.print("ThingSpeak Response: ");
    Serial.println(httpResponseCode);

    http.end();
  }
  else
  {
    Serial.println("WiFi not connected.");
  }
}

// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);

  analogReadResolution(12);

  Serial.println();
  Serial.println("========================================");
  Serial.println(" ADAPTIVE MULTI-CELL BATTERY ENGINE");
  Serial.println("========================================");

  // Connect to Wokwi WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected!");
  Serial.println(WiFi.localIP());
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop()
{
  float cells[4];

  // Read all four cells
  cells[0] = readCellVoltage(CELL1_PIN);
  cells[1] = readCellVoltage(CELL2_PIN);
  cells[2] = readCellVoltage(CELL3_PIN);
  cells[3] = readCellVoltage(CELL4_PIN);

  // Calculate pack voltage
  float packVoltage = 0;

  for (int i = 0; i < 4; i++)
  {
    packVoltage += cells[i];
  }

  // Average voltage
  float averageVoltage = packVoltage / 4.0;

  // Strongest and weakest
  int strongest = findStrongestCell(cells);
  int weakest = findWeakestCell(cells);

  // Imbalance percentage
  float imbalance =
    ((cells[strongest] - cells[weakest])
     / averageVoltage) * 100.0;

  // Battery health
  String batteryState =
    classifyBattery(cells, imbalance);

  // ---------------- Serial Monitor ----------------

  Serial.println();
  Serial.println("----------------------------------------");

  Serial.print("Cell 1 : ");
  Serial.print(cells[0], 3);
  Serial.println(" V");

  Serial.print("Cell 2 : ");
  Serial.print(cells[1], 3);
  Serial.println(" V");

  Serial.print("Cell 3 : ");
  Serial.print(cells[2], 3);
  Serial.println(" V");

  Serial.print("Cell 4 : ");
  Serial.print(cells[3], 3);
  Serial.println(" V");

  Serial.println();

  Serial.print("Pack Voltage  : ");
  Serial.print(packVoltage, 3);
  Serial.println(" V");

  Serial.print("Average Cell  : ");
  Serial.print(averageVoltage, 3);
  Serial.println(" V");

  Serial.print("Strongest Cell: Cell ");
  Serial.println(strongest + 1);

  Serial.print("Weakest Cell  : Cell ");
  Serial.println(weakest + 1);

  Serial.print("Imbalance     : ");
  Serial.print(imbalance, 2);
  Serial.println(" %");

  Serial.print("Battery State : ");
  Serial.println(batteryState);

  // ---------------- ThingSpeak ----------------

  sendToThingSpeak(
    cells[0],
    cells[1],
    cells[2],
    cells[3],
    packVoltage,
    averageVoltage,
    imbalance,
    batteryState
  );

  delay(20000);
}
