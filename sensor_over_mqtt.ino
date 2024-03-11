/*
  Sensor Over MQTT
  Author: Guillaume CUADRADO
  Date: 2024-03-01
  Description: This code is used to send sensor data over MQTT, based on :

    ArduinoMqttClient - WiFi Simple Sender

    This example connects to a MQTT broker and publishes a message to
    a topic once a second.

    The circuit:
    - Arduino MKR 1000, MKR 1010 or Uno WiFi Rev2 board

    This example code is in the public domain.
*/
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <Arduino_MKRENV.h>
#include <ArduinoMqttClient.h>
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
  #include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
  #include <WiFi101.h>
#elif defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_NICLA_VISION) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_GIGA) || defined(ARDUINO_OPTA)
  #include <WiFi.h>
#elif defined(ARDUINO_PORTENTA_C33)
  #include <WiFiC3.h>
#elif defined(ARDUINO_UNOR4_WIFI)
  #include <WiFiS3.h>
#endif

#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

extern const char broker[];   
int        port     = 1883;
const char topic[]  = "/home/sensors";

const long interval = 20000;
unsigned long previousMillis = 0;

int count = 0;

int temperature = 0;
int humidity = 0;
int pressure = 0;
int lux = 0;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);

  // MKR ENV begin
  if (!ENV.begin()) {
    Serial.println("Failed to initialize MKR ENV shield!");
    while(1);
  }

  // attempt to connect to WiFi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  // mqttClient.setId("clientId");

  // You can provide a username and password for authentication
  // mqttClient.setUsernamePassword("username", "password");

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void loop() {
  // Create Json document to store sensor values
  StaticJsonDocument<32> doc;
  char output[55];

  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();

  // to avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
  // see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    // Read the sensor values
    float h = ENV.readHumidity();
    float t = ENV.readTemperature();
    float p = ENV.readPressure();
    float l = ENV.readIlluminance();

    // Cast
    temperature = (int)t;
    humidity = (int)h;
    lux = (int)l;
    pressure = (int)p;

    // Populate the JSON doc
    doc["t"] = temperature;
    doc["p"] = pressure;
    doc["h"] = humidity;
    doc["l"] = lux;

    Serial.print("Sending message to topic: ");
    Serial.println(topic);

    // Serialize the JSON doc to be sent to the MQTT Broker 
    serializeJson(doc, output);
    Serial.print(output);

    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic);
    mqttClient.print(output);
    mqttClient.endMessage();

    Serial.println();

    count++;
  }
}
