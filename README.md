# ESP32 IoT Project

## Structure

### cpp

C++ code for ESP32

Libraries used:
- knolleary/PubSubClient@^2.8
- hirotakaster/CoAP simple library@^1.3.25
- arduino-libraries/ArduinoHttpClient@^0.5.0

### python

Python code including servers

## Tech Stack

### Mosquitto

MQTT Message Broker 

Modified mosquitto.conf to be visible inside local network

Start mosquitto
```shell
sudo systemctl start mosquitto
```

Example: 
```shell
mosquitto_sub -t "topic/numbers"
```

### NodeRed

Interactive Dashboards to show network performance stats.

Install

```shell
sudo npm install -g --unsafe-perm node-red
```

File `flows.json` contains a backup of NodeRed Flow

Head to [NodeRed UI](http://localhost:1880/ui) to see the Dashboard

## Bluetooth Module

Install `pybluez` library

```shell
pip install git+https://github.com/pybluez/pybluez.git#egg=pybluez
```

```shell
pip install -r requirements.txt
```

- Start `python/bluetooth-iot/main-bluetooth.py` script

This script will attempt connection to the ESP32 bluetooth using `pybluez` library. It will receive 
messages and forward them (modifying the content).

## HTTP Module

```shell
pip install -r requirements.txt
```

Run Python Server

```shell
uvicorn python.http-server.main-http:app --reload --host 0.0.0.0 --port 8000
```

The FastAPI server exposes a GET endpoint and a POST endpoint.

Open server with ngrok (no more needed)
```shell
ngrok http --scheme=http --scheme https 8000
```

## CoAP Module

Install `aiocoap` library and other requirements.

```shell
pip install -r requirements.txt
```

- Start `python/coap-server/main-coap.py` script

The CoAP server exposes a GET endpoint and a PUT endpoint.

# Performance Analysis

- Start HTTP server
- Start CoAP server
- Start Bluetooth client

Upload `cpp/performance/src/main.cpp` onto ESP32 board. 

The board will setup all the required connections, using the file `secrets.h` and then start 
looping. Each loop will send one packet per connection type, registering the RTT (Round Trip Time),
number of packets sent and received, and bytes sent.

Once the packets sent hit a threshold (default 5), the results are aggregated and sent to Node-RED 
via MQTT. Those results are then displayed, both separated by connection type, and all together
for better comparison.

Stats are then reset and the loop goes on.