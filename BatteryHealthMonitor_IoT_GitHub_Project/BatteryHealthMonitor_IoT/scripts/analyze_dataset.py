import csv
import sys
from collections import Counter


def main(path):
    rows = []
    with open(path, newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            rows.append(row)

    if not rows:
        print("No rows found.")
        return

    risks = Counter(row["risk_level"] for row in rows)
    voltages = [float(row["voltage_v"]) for row in rows]
    temps = [float(row["temperature_c"]) for row in rows]
    currents = [float(row["current_ma"]) for row in rows]

    print("Dataset summary")
    print("---------------")
    print(f"Rows: {len(rows)}")
    print(f"Voltage: min={min(voltages):.3f}V max={max(voltages):.3f}V avg={sum(voltages)/len(voltages):.3f}V")
    print(f"Current: min={min(currents):.2f}mA max={max(currents):.2f}mA avg={sum(currents)/len(currents):.2f}mA")
    print(f"Temperature: min={min(temps):.2f}C max={max(temps):.2f}C avg={sum(temps)/len(temps):.2f}C")
    print("Risk counts:")
    for risk, count in risks.items():
        print(f"  {risk}: {count}")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python analyze_dataset.py <csv_path>")
        sys.exit(1)
    main(sys.argv[1])
