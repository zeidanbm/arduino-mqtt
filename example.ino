/*
  ArduinoMqttClient - WiFi Simple Sender

  This example connects to a MQTT broker and publishes a message to
  a topic once a second.

  The circuit:
  - Arduino MKR 1000, MKR 1010 or Uno WiFi Rev.2 board

  This example code is in the public domain.
*/
#include "DHT.h"
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include "secrets.h"

///////you may enter your sensitive data in secrets.h
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "0.tcp.ngrok.io";
int        port     = 17493;
const char topic[]  = "v1/devices/me/telemetry";

String username = "jHzOSbznV8JwGfGZoCs4"; //authentication token here
String password = "";

const long interval = 1000;
unsigned long previousMillis = 0;

int buttonPin = 1;

/////////// DHT11 sensor ////////////
#define DHT_PIN 2
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);
float hum; // Stores humidity value
float temp; // Stores temperature value

/////////// Sound sensor ////////////
int MicButtonPin=3; //define switch port
int val; //define digital variable val
void setup() {
  //pin setup
  // pinMode(buttonPin, INPUT);
  // pinMode(MicButtonPin, INPUT);
  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Setup sensor pins and set pull timings
  dht.begin();

  // attempt to connect to Wifi network:
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
  
  mqttClient.setUsernamePassword(username, password);

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}



void loop() {

  // Get humidity and temperature
  hum = dht.readHumidity();
  delay(10);
  temp = dht.readTemperature();

  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print(" %, Temp: ");
  Serial.print(temp);
  Serial.println(" Celsius");

  // Detect sound
  // val = digitalRead(MicButtonPin); // read the value of the digital interface 3

  Serial.print("Sound: ");
  Serial.println(val);

  //
  String msgPayload = "{\"temperature\":"; 
  msgPayload += temp;
  msgPayload += ",\"humidity\":\"";
  msgPayload += hum;
  msgPayload += ",\"Sound\":\"";
  msgPayload += val;
  msgPayload += "\"}";

  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  // mqttClient.poll();

  // // send message, the Print interface can be used to set the message contents
  // mqttClient.beginMessage(topic);
  // mqttClient.print(msgPayload);
  // mqttClient.endMessage();
  
  
  // send message, the Print interface can be used to set the message contents
  Serial.print("Sending message to topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(msgPayload);
  Serial.println();
  
  delay(5000);
}