# Static-Test-Pad-ESP32
Simple Static Test Pad Logger using ESP32 and ESP-NOW for communication between STP and Ground Station. Also includes Grapher and logger python scripts for Visual Graphs.

## For Uploading Code on ESP32 
```bash
 idf.py -p <port-name> -b 115200 flash monitor

 [USAGE:]
idf.py -p /dev/ttyUSB0 -b 115200 flash monitor
```


## For Using the Logger
```bash
python logger.py
```

## For converting the CSV to Graph
```bash
python graph.py <logfile.csv>
```
