#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Adafruit_INA219.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "config.h"

Adafruit_INA219 ina219;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);

unsigned long lastReadMs = 0;
unsigned long lastUploadMs = 0;
int demoCounter = 0;

struct BatteryReading {
  float voltageV;
  float currentMA;
  float powerW;
  float temperatureC;
  float batteryPercent;
  String riskLevel;
  bool relayEnabled;
};

float clampFloat(float value, float low, float high) {
  if (value < low) return low;
  if (value > high) return high;
  return value;
}

float estimateBatteryPercent(float voltage) {
  float percent = ((voltage - BATTERY_EMPTY_VOLTAGE) / (BATTERY_FULL_VOLTAGE - BATTERY_EMPTY_VOLTAGE)) * 100.0;
  return clampFloat(percent, 0.0, 100.0);
}

String classifyRisk(float percent, float tempC, float voltageV) {
  if (tempC > CRITICAL_TEMP_C) return "CRITICAL";
  if (percent < CRITICAL_PERCENT_THRESHOLD) return "CRITICAL";
  if (percent < WARNING_PERCENT_THRESHOLD) return "WARNING";
  return "NORMAL";
}

void setRelay(bool on) {
  if (RELAY_ACTIVE_LOW) {
    digitalWrite(RELAY_PIN, on ? LOW : HIGH);
  } else {
    digitalWrite(RELAY_PIN, on ? HIGH : LOW);
  }
}

void setOutputs(const String &risk) {
  digitalWrite(LED_GREEN_PIN, risk == "NORMAL" ? HIGH : LOW);
  digitalWrite(LED_YELLOW_PIN, risk == "WARNING" ? HIGH : LOW);
  digitalWrite(LED_RED_PIN, risk == "CRITICAL" ? HIGH : LOW);

  if (risk == "NORMAL") {
    noTone(BUZZER_PIN);
    setRelay(true);     // load ON
  } else if (risk == "WARNING") {
    tone(BUZZER_PIN, 1200, 120);
    setRelay(true);     // load still ON
  } else {
    tone(BUZZER_PIN, 2200, 300);
    setRelay(false);    // load OFF
  }
}

void connectWiFi() {
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWi-Fi connection failed. Device will still run locally.");
  }
}

BatteryReading readBattery() {
  BatteryReading r;

  float busVoltage = ina219.getBusVoltage_V();
  float shuntVoltageMV = ina219.getShuntVoltage_mV();
  float currentMA = ina219.getCurrent_mA();
  float loadVoltage = busVoltage + (shuntVoltageMV / 1000.0);

  tempSensor.requestTemperatures();
  float tempC = tempSensor.getTempCByIndex(0);

  if (tempC == DEVICE_DISCONNECTED_C) {
    tempC = -999.0;
  }

#if DEMO_MODE
  // Demo simulation cycles through normal, warning, and critical without draining the battery.
  demoCounter++;
  int phase = (demoCounter / 5) % 3;
  if (phase == 0) {
    loadVoltage = 4.05;
    tempC = 32.0;
  } else if (phase == 1) {
    loadVoltage = 3.58;
    tempC = 36.0;
  } else {
    loadVoltage = 3.30;
    tempC = 52.0;
  }
#endif

  r.voltageV = loadVoltage;
  r.currentMA = currentMA;
  r.powerW = loadVoltage * (currentMA / 1000.0);
  r.temperatureC = tempC;
  r.batteryPercent = estimateBatteryPercent(loadVoltage);
  r.riskLevel = classifyRisk(r.batteryPercent, r.temperatureC, r.voltageV);
  r.relayEnabled = (r.riskLevel != "CRITICAL");

  return r;
}

String makeJson(const BatteryReading &r) {
  String json = "{";
  json += "\"device_id\":\"" + String(DEVICE_ID) + "\",";
  json += "\"voltage_v\":" + String(r.voltageV, 3) + ",";
  json += "\"current_ma\":" + String(r.currentMA, 2) + ",";
  json += "\"power_w\":" + String(r.powerW, 3) + ",";
  json += "\"temperature_c\":" + String(r.temperatureC, 2) + ",";
  json += "\"battery_percent\":" + String(r.batteryPercent, 1) + ",";
  json += "\"risk_level\":\"" + r.riskLevel + "\",";
  json += "\"relay_enabled\":" + String(r.relayEnabled ? "true" : "false") + ",";
  json += "\"uptime_ms\":" + String(millis());
  json += "}";
  return json;
}

bool firebasePut(const String &path, const String &json) {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }

  WiFiClientSecure client;
  client.setInsecure(); // Simple for student demo. For production, validate the certificate.

  HTTPClient https;
  String url = String(FIREBASE_DATABASE_URL) + path + ".json";
  if (String(FIREBASE_AUTH_TOKEN).length() > 0) {
    url += "?auth=" + String(FIREBASE_AUTH_TOKEN);
  }

  if (!https.begin(client, url)) {
    Serial.println("Firebase HTTPS begin failed.");
    return false;
  }

  https.addHeader("Content-Type", "application/json");
  int httpCode = https.PUT(json);
  String response = https.getString();
  https.end();

  Serial.print("Firebase PUT code: ");
  Serial.println(httpCode);
  if (httpCode < 200 || httpCode >= 300) {
    Serial.print("Firebase response: ");
    Serial.println(response);
    return false;
  }
  return true;
}

bool firebasePost(const String &path, const String &json) {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;
  String url = String(FIREBASE_DATABASE_URL) + path + ".json";
  if (String(FIREBASE_AUTH_TOKEN).length() > 0) {
    url += "?auth=" + String(FIREBASE_AUTH_TOKEN);
  }

  if (!https.begin(client, url)) {
    Serial.println("Firebase HTTPS begin failed.");
    return false;
  }

  https.addHeader("Content-Type", "application/json");
  int httpCode = https.POST(json);
  String response = https.getString();
  https.end();

  Serial.print("Firebase POST code: ");
  Serial.println(httpCode);
  if (httpCode < 200 || httpCode >= 300) {
    Serial.print("Firebase response: ");
    Serial.println(response);
    return false;
  }
  return true;
}

void printReading(const BatteryReading &r) {
  Serial.println("------------------------------");
  Serial.print("Voltage: "); Serial.print(r.voltageV, 3); Serial.println(" V");
  Serial.print("Current: "); Serial.print(r.currentMA, 2); Serial.println(" mA");
  Serial.print("Power: "); Serial.print(r.powerW, 3); Serial.println(" W");
  Serial.print("Temperature: "); Serial.print(r.temperatureC, 2); Serial.println(" C");
  Serial.print("Battery: "); Serial.print(r.batteryPercent, 1); Serial.println(" %");
  Serial.print("Risk: "); Serial.println(r.riskLevel);
  Serial.print("Relay enabled: "); Serial.println(r.relayEnabled ? "YES" : "NO");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_YELLOW_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  setRelay(true);

  Wire.begin(21, 22);
  tempSensor.begin();

  if (!ina219.begin()) {
    Serial.println("ERROR: INA219 not found. Check SDA/SCL/VCC/GND.");
    while (true) {
      digitalWrite(LED_RED_PIN, HIGH);
      delay(300);
      digitalWrite(LED_RED_PIN, LOW);
      delay(300);
    }
  }

  connectWiFi();
  Serial.println("Battery monitor started.");
}

void loop() {
  unsigned long now = millis();

  if (now - lastReadMs >= READ_INTERVAL_MS) {
    lastReadMs = now;
    BatteryReading r = readBattery();
    setOutputs(r.riskLevel);
    printReading(r);

    if (now - lastUploadMs >= FIREBASE_UPLOAD_INTERVAL_MS) {
      lastUploadMs = now;
      String json = makeJson(r);
      String latestPath = String("/battery_monitor/") + DEVICE_ID + "/latest";
      String readingsPath = String("/battery_monitor/") + DEVICE_ID + "/readings";
      firebasePut(latestPath, json);
      firebasePost(readingsPath, json);
    }
  }
}
