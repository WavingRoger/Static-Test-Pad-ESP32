import sys
import pandas as pd
import matplotlib.pyplot as plt

def main():
    # ✅ Check argument
    if len(sys.argv) != 2:
        print("Usage: python graph.py <csv_file>")
        sys.exit(1)

    file_path = sys.argv[1]

    try:
        # 📥 Load data
        df = pd.read_csv(file_path)

        # ✅ Validate columns
        if "timestamp_ms" not in df.columns or "weight" not in df.columns:
            print("CSV must contain: timestamp_ms, weight")
            sys.exit(1)

        # ⏱ Normalize time (start from 0)
        t0 = df["timestamp_ms"].iloc[0]
        df["time_s"] = (df["timestamp_ms"] - t0) / 1000.0

        # 📊 Plot
        plt.figure(figsize=(10, 5))
        plt.plot(df["time_s"], df["weight"], linewidth=1)

        plt.title("Weight vs Time")
        plt.xlabel("Time (s)")
        plt.ylabel("Weight (kg)")
        plt.grid()

        plt.tight_layout()
        plt.show()

    except FileNotFoundError:
        print(f"File not found: {file_path}")
    except Exception as e:
        print("Error:", e)

if __name__ == "__main__":
    main()
