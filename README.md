# ESP32 IoT Project

## Structure

### cpp

C++ code for ESP32

### python

Python code including servers


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


- Start `python/bluetooth-iot/main.py` script

This script will attempt connection to the ESP32 bluetooth using `pybluez` library. It will receive 
JSON messages and forward them (modifying the content).

## HTTP Module

Run Python Server

```shell
uvicorn http-server.main-http:app --reload --host 0.0.0.0 --port 8000```
```

Open server with ngrok
```shell
ngrok http --scheme=http --scheme https 8000
```