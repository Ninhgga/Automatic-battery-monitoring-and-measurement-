#ifndef CONFIG_H
#define CONFIG_H

// Copy this file to config.h and fill in your own values.

#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// Example: https://your-project-id-default-rtdb.asia-southeast1.firebasedatabase.app
#define FIREBASE_DATABASE_URL "https://YOUR_PROJECT_ID-default-rtdb.firebaseio.com"

// For class demo only, you may keep this empty if your Firebase rules allow write access.
// For safer setup, use a database secret or auth token.
#define FIREBASE_AUTH_TOKEN ""

#define DEVICE_ID "device_001"

// Set to 1 to simulate warning/critical states during demo.
#define DEMO_MODE 0

// Hardware pins
#define ONE_WIRE_BUS 4
#define RELAY_PIN 13
#define BUZZER_PIN 14
#define LED_GREEN_PIN 25
#define LED_YELLOW_PIN 26
#define LED_RED_PIN 27

// Many relay modules are active LOW. If your relay turns on backward, change this.
#define RELAY_ACTIVE_LOW 0

// Battery calibration for one 18650 Li-ion cell.
#define BATTERY_FULL_VOLTAGE 4.20
#define BATTERY_EMPTY_VOLTAGE 3.20

// Risk thresholds
#define WARNING_PERCENT_THRESHOLD 50.0
#define CRITICAL_PERCENT_THRESHOLD 20.0
#define CRITICAL_TEMP_C 50.0

// Data upload interval
#define READ_INTERVAL_MS 2000
#define FIREBASE_UPLOAD_INTERVAL_MS 5000

#endif
