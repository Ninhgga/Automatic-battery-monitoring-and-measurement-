# Demo Plan

## Goal

Show that the system does more than display battery numbers. It should monitor, classify risk, upload data, and take action.

## Demo Steps

1. Power the ESP32.
2. Show Serial Monitor readings.
3. Show Firebase database receiving data.
4. Show NORMAL state:
   - Green LED ON
   - Fan/load ON
   - Relay enabled
5. Enable demo mode or adjust threshold to show WARNING:
   - Yellow LED ON
   - Buzzer beeps
   - Fan/load still ON
6. Show CRITICAL state:
   - Red LED ON
   - Buzzer beeps faster
   - Relay/SSR disconnects the fan/load
7. Explain that TP4056 handles charging/protection, INA219 handles measurement, and relay/SSR handles shutdown.

## What to Say

This project is an IoT-based battery monitoring and maintenance system. The ESP32 reads voltage, current, and temperature, estimates battery percentage, classifies the risk level, uploads data to Firebase, and disconnects the load during critical battery condition.
