# ESP32 IoT Project

## Structure

### cpp

C++ code for ESP32

### python

Python code including servers

## Tech Stack

### Mosquitto

Modified mosquitto.conf to be visible inside local network
```shell
sudo systemctl start mosquitto
```

Example: 
```shell
mosquitto_sub -t "topic/numbers"
```

### NodeRed

Install

```shell
sudo npm install -g --unsafe-perm node-red
```

File `flows.json` contains a backup of NodeRed Flow

Head to [NodeRed UI](http://localhost:1880/ui) to see the Dashboard

## Bluetooth Module

### Requirements

```shell
pip install git+https://github.com/pybluez/pybluez.git#egg=pybluez
```

```shell
pip install -r requirements.txt
```

### Run

- Upload `cpp/bluetooth-arduino/main.cpp` onto ESP32 board

This will set up the board bluetooth and start sending numbered JSON packages 
(including `millis` for measurements) over `BluetoothSerial` library.


- Start `python/bluetooth-iot/main-bluetooth.py` script

This script will attempt connection to the ESP32 bluetooth using `pybluez` library. It will receive 
JSON messages and forward them (modifying the content).

## HTTP Module

Run Python Server

```shell
uvicorn python.http-server.main-http:app --reload --host 0.0.0.0 --port 8000
```

Open server with ngrok (no more needed)
```shell
ngrok http --scheme=http --scheme https 8000
```

