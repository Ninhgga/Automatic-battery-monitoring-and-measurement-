import argparse
import csv
import random
from datetime import datetime, timedelta


def clamp(value, low, high):
    return max(low, min(high, value))


def battery_percent(voltage):
    return clamp(((voltage - 3.2) / (4.2 - 3.2)) * 100.0, 0.0, 100.0)


def risk_level(percent, temp_c):
    if temp_c > 50.0:
        return "CRITICAL"
    if percent < 20.0:
        return "CRITICAL"
    if percent < 50.0:
        return "WARNING"
    return "NORMAL"


def main():
    parser = argparse.ArgumentParser(description="Generate synthetic battery monitoring dataset.")
    parser.add_argument("--rows", type=int, default=500)
    parser.add_argument("--output", type=str, default="dataset/generated_battery_readings.csv")
    parser.add_argument("--device-id", type=str, default="device_001")
    args = parser.parse_args()

    start = datetime.now().replace(microsecond=0)
    voltage = 4.18

    with open(args.output, "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow([
            "timestamp", "device_id", "voltage_v", "current_ma", "power_w",
            "temperature_c", "battery_percent", "risk_level", "relay_enabled"
        ])

        for i in range(args.rows):
            timestamp = start + timedelta(seconds=5 * i)
            voltage -= random.uniform(0.0005, 0.004)
            voltage = clamp(voltage, 3.05, 4.2)
            current_ma = random.uniform(80, 220)
            temp_c = 30 + (4.2 - voltage) * 10 + random.uniform(-1.5, 3.0)

            # Inject occasional high-temperature event.
            if i > args.rows * 0.7 and random.random() < 0.03:
                temp_c = random.uniform(50.5, 58.0)

            percent = battery_percent(voltage)
            risk = risk_level(percent, temp_c)
            relay = risk != "CRITICAL"
            power_w = voltage * current_ma / 1000.0

            writer.writerow([
                timestamp.isoformat(),
                args.device_id,
                round(voltage, 3),
                round(current_ma, 2),
                round(power_w, 3),
                round(temp_c, 2),
                round(percent, 1),
                risk,
                str(relay).lower(),
            ])

    print(f"Generated {args.rows} rows -> {args.output}")


if __name__ == "__main__":
    main()
