# Dataset

`sample_battery_readings.csv` is a small example dataset for report screenshots and testing analysis scripts.

Columns:

- timestamp
- device_id
- voltage_v
- current_ma
- power_w
- temperature_c
- battery_percent
- risk_level
- relay_enabled

Generate more data:

```bash
python ../scripts/generate_synthetic_dataset.py --rows 1000 --output generated_battery_readings.csv
```
