import serial
import time

SERIAL_PORT = "/dev/ttyUSB0"
BAUD = 115200

ser = serial.Serial(SERIAL_PORT, BAUD, timeout=1)

filename = f"log_{int(time.time())}.csv"

print(f"Logging to {filename}...")

with open(filename, "w") as f:
    f.write("timestamp_ms,weight\n")

    while True:
        try:
            line = ser.readline().decode(errors='ignore').strip()

            if not line:
                continue

            # 🔥 Ignore ESP-IDF logs
            if line.startswith(("I (", "E (", "W (", "D (")):
                continue

            # 🔥 Only accept CSV format
            if ',' not in line:
                continue

            parts = line.split(',')

            if len(parts) != 2:
                continue

            t = int(parts[0])
            w = float(parts[1])

            # write to file
            f.write(f"{t},{w}\n")
            f.flush()

            print(f"{t} ms | {w} kg")

        except Exception as e:
            print("Error:", e)
