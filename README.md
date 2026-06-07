# IoT Battery Health Monitoring and Risk Classification System

This repository contains a student-buildable IoT project for monitoring a rechargeable battery using an ESP32, INA219 voltage/current sensor, DS18B20 temperature sensor, TP4056 charging/protection module, MT3608 boost converter, and a 5V relay/SSR shutdown module.

The system measures voltage, current, temperature, estimated battery percentage, power, and risk level. Data is sent to Firebase Realtime Database through Wi-Fi. The ESP32 also controls LEDs, buzzer, and relay/SSR based on the battery risk level.

## Features

- ESP32 Wi-Fi IoT monitoring
- INA219 voltage/current/power reading
- DS18B20 temperature reading
- Battery percentage estimation for 1-cell 18650 Li-ion
- Risk classification: NORMAL, WARNING, CRITICAL
- Green/yellow/red LED status output
- Buzzer warning output
- 5V relay/SSR shutdown output
- Firebase Realtime Database logging
- Sample dataset and synthetic dataset generator

## Hardware

| Component | Purpose |
|---|---|
| ESP32 DevKit | Main microcontroller and Wi-Fi connection |
| INA219 | Measures voltage, current, and power |
| DS18B20 | Measures battery temperature |
| TP4056 with protection | Charges and protects one 18650 Li-ion battery |
| MT3608 boost converter | Boosts 3.2V-4.2V battery voltage to 5V for load/relay |
| 5V relay module or 5V SSR module | Disconnects load during critical battery state |
| Green/yellow/red LEDs | Shows NORMAL/WARNING/CRITICAL status |
| Buzzer | Audible warning |
| Small 5V fan or LED load | Demo load |

## Suggested Wiring

Read `docs/wiring.md` before connecting anything.

## Repository Structure

```text
BatteryHealthMonitor_IoT/
  arduino/esp32_battery_monitor/
    esp32_battery_monitor.ino
    config.example.h
  backend/firebase/
    database_rules.json
    example_database_shape.json
  dataset/
    sample_battery_readings.csv
    README.md
  scripts/
    generate_synthetic_dataset.py
    analyze_dataset.py
  docs/
    wiring.md
    demo_plan.md
  README.md
  .gitignore
```

## Setup: Arduino ESP32 Code

1. Open Arduino IDE.
2. Install ESP32 board support.
3. Install these libraries from Library Manager:
   - Adafruit INA219
   - Adafruit BusIO
   - OneWire
   - DallasTemperature
4. Open `arduino/esp32_battery_monitor/esp32_battery_monitor.ino`.
5. Copy `config.example.h` to `config.h`.
6. Fill in Wi-Fi and Firebase settings inside `config.h`.
7. Upload to ESP32.
8. Open Serial Monitor at `115200` baud.

## Firebase Setup

1. Create a Firebase project.
2. Create a Realtime Database.
3. Use test mode only for class demo, or use the rules in `backend/firebase/database_rules.json`.
4. Copy your database URL into `config.h`.
5. For a simple demo, keep `FIREBASE_AUTH_TOKEN` empty if your database rules allow writes. For safer setup, use a database secret/token.

Example database path:

```text
/battery_monitor/device_001/latest
/battery_monitor/device_001/readings
```

## Dataset

A small sample dataset is provided in `dataset/sample_battery_readings.csv`.

Generate a bigger fake dataset:

```bash
python scripts/generate_synthetic_dataset.py --rows 1000 --output dataset/generated_battery_readings.csv
```

Analyze the dataset:

```bash
python scripts/analyze_dataset.py dataset/generated_battery_readings.csv
```

## Demo Mode

The Arduino code has a `DEMO_MODE` option in `config.h`. When enabled, the ESP32 can simulate warning and critical states without waiting for the battery to actually drain.

## Risk Logic

```text
If temperature > 50°C -> CRITICAL
Else if battery percentage < 20% -> CRITICAL
Else if battery percentage < 50% -> WARNING
Else -> NORMAL
```

## Safety Notes

- Use one 18650 cell for prototype only.
- Do not use high-power battery packs for a student demo.
- Check relay/SSR rating before connecting a load.
- Adjust MT3608 output to 5.0V with a multimeter before connecting your load.
- TP4056 is for charging/protection, not measurement.
- INA219 is for measurement.
- Relay/SSR is for shutdown action.
