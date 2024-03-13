# Description
The purpose of this project was to play IoT weather station using MQTT protocol. 

# Server side
## Set up 
The MQTT Broker of this project is a Raspberry Pi 3B+, the setup of the stack is very simple thanks to this amazing course : https://learnembeddedsystems.co.uk/easy-raspberry-pi-iot-server

Therefore I won't dig into lot of details, the IOTStack was used with those components :
- Mosquitto
- Portainer-ce
- Node-Red
- Influxdb
- Grafana

# Client side
## Set up
The MQTT Client is handled by an Arduino MKR WIFI 1010 with a MKR ENV sensor shield, the idea was to collect data and publish it to the broker to ramp up on how all the broker stack is working :
- Initialize
- Connect to a WIFI network
- Connect to a topic of the MQTT Broker
- Publish messages (JSON string) at regular intervals

## Dev Environment
This project was developped under VSCode editor with Arduino plugin (v0.4.11), and Arduino CLI (v0.29.0). 

## Arduino Libraries used
These are the used libs :
- WIFININA v1.8.14
- ArduinoMqttClient v0.1.8
- Arduino_MKRENV v1.2.1
- ArduinoJson v7.0.3

## Firmware
The main is located in sensor_over-mqtt.ino file, and to make it works you should have a arduino_secrets.h file which holds personnal wifi related data such as wifi passkey and broker private ip address : 
```
#define SECRET_SSID "Toto"
#define SECRET_PASS "my-name-is-toto"

const char broker[] = "192.168.1.66";
```
