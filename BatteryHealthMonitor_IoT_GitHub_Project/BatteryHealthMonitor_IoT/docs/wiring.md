# Wiring Guide

## 1. ESP32 to INA219

| INA219 Pin | ESP32 Pin |
|---|---|
| VCC | 3.3V or 5V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

## 2. INA219 Battery/Load Side

```text
Battery positive -> INA219 VIN+
INA219 VIN- -> Relay/SSR COM or load positive path
Load negative -> Battery negative
```

The INA219 must sit in series between the battery and the load to measure current.

## 3. ESP32 to DS18B20

| DS18B20 Pin | ESP32 Pin |
|---|---|
| VCC | 3.3V |
| GND | GND |
| DATA | GPIO 4 |

Add a 4.7kΩ pull-up resistor between DATA and 3.3V.

## 4. ESP32 to LEDs

| LED | ESP32 Pin |
|---|---|
| Green LED | GPIO 25 |
| Yellow LED | GPIO 26 |
| Red LED | GPIO 27 |

Each LED needs a 220Ω or 330Ω resistor.

```text
GPIO -> resistor -> LED anode
LED cathode -> GND
```

## 5. ESP32 to Buzzer

| Buzzer Pin | ESP32 Pin |
|---|---|
| Positive | GPIO 14 |
| Negative | GND |

## 6. ESP32 to 5V Relay or 5V SSR

| Relay/SSR Pin | Connection |
|---|---|
| VCC | 5V |
| GND | GND |
| IN | GPIO 13 |

Some relay modules are active LOW. If your relay behaves backward, change `RELAY_ACTIVE_LOW` in `config.h`.

## 7. TP4056 and 18650 Battery

Use TP4056 with protection.

| TP4056 Pin | Connection |
|---|---|
| B+ | 18650 positive |
| B- | 18650 negative |
| OUT+ | Project battery output positive |
| OUT- | Project battery output negative |
| IN+ / USB 5V | Charger input positive |
| IN- / USB GND | Charger input ground |

TP4056 is for charging/protection. It does not measure battery percentage.

## 8. MT3608 Boost Converter

Use this if your relay/load needs stable 5V from one 18650 battery.

```text
TP4056 OUT+ -> MT3608 IN+
TP4056 OUT- -> MT3608 IN-
MT3608 OUT+ -> 5V relay/load power
MT3608 OUT- -> common GND
```

Adjust MT3608 output to 5.0V using a multimeter before connecting it to the relay/fan.

## Safety

- Start with a small 5V fan or LED load.
- Do not connect high-current motors or large battery packs.
- All grounds must be common.
- Check relay/SSR current rating before using it.
