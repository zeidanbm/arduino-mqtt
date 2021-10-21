#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>
#include "secrets.h"

///////you may enter your sensitive data in secrets.h
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "4.tcp.ngrok.io";
int        port     = 16180;
const char topic[]  = "v1/devices/me/telemetry";
const char inTopic[]   = "v1/devices/me/rpc/request/+";

String username = "jHzOSbznV8JwGfGZoCs4"; //authentication token here
String password = "";

const long interval = 1000;
unsigned long previousMillis = 0;

/////////// Temp sensor ////////////
// Data wire is plugged into digital pin 5 on the Arduino
#define ONE_WIRE_BUS 1
// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);
// Pass oneWire reference to DallasTemperature library
DallasTemperature tempSensor(&oneWire);
float temp; //Stores temperature value

/////////// Sound sensor ////////////
int micPin=4; //define switch port
int hasSound; //define digital variable 0 or 1 if we have sound or not

/////////// Motion sensor ////////////
int pirPin = 0; //choose the input pin (for PIR sensor)
int pirState = LOW; //we start, assuming no motion detected
int hasMotion = 0; //variable for reading the pin status

/////////// LED ////////////
int ledPin = 2;
int brightness = 250;

void setup() 
{
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Setup sensor pins
  pinMode(micPin, INPUT);
  pinMode(pirPin, INPUT);     // declare sensor as input
  tempSensor.begin();

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

  // set the message receive callback
  mqttClient.onMessage(onMqttMessage);
  Serial.print("Subscribing to topic: ");
  Serial.println(inTopic);
  Serial.println();

  // subscribe to a topic
  // the second parameter set's the QoS of the subscription,
  // the the library supports subscribing at QoS 0, 1, or 2
  int subscribeQos = 1;

  mqttClient.subscribe(inTopic, subscribeQos);

  // topics can be unsubscribed using:
  // mqttClient.unsubscribe(inTopic);

  Serial.print("Waiting for messages on topic: ");
  Serial.println(inTopic);
  Serial.println();
}


void loop() 
{
  // Read value from PIR sensor
  hasMotion = digitalRead(pirPin);  // read input value

  Serial.print("Motion: ");	// print on output change
  Serial.println(hasMotion);

  // Get temperature in Celsius
  tempSensor.requestTemperatures(); 
  temp = tempSensor.getTempCByIndex(0);

  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.println(" Celsius");

  // Detect sound
  hasSound = digitalRead(micPin); // read the value of the digital interface 3

  Serial.print("Sound: ");
  Serial.println(hasSound);

  //
  String msgPayload = "{\"temperature\":"; 
  msgPayload += temp;
  msgPayload += ",\"sound\":";
  msgPayload += hasSound;
  msgPayload += ",\"motion\":";
  msgPayload += hasMotion;
  msgPayload += "}";

  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();

  // send message, the Print interface can be used to set the message contents
  mqttClient.beginMessage(topic);
  mqttClient.print(msgPayload);
  mqttClient.endMessage();
  
  
  // send message, the Print interface can be used to set the message contents
  Serial.print("Sending message to topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(msgPayload);
  Serial.println();
  
  delay(1000);
}

void onMqttMessage(int messageSize) {

  // we received a message, print out the topic and contents
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', duplicate = ");
  Serial.print(mqttClient.messageDup() ? "true" : "false");
  Serial.print(", QoS = ");
  Serial.print(mqttClient.messageQoS());
  Serial.print(", retained = ");
  Serial.print(mqttClient.messageRetain() ? "true" : "false");
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    StaticJsonDocument<256> doc;
    deserializeJson(doc, mqttClient.readString());

    JsonObject obj = doc.as<JsonObject>();
    brightness = obj[String("params")];

    if(brightness <= 250) {
      analogWrite(ledPin, brightness);
    }
  }
  Serial.println();

  Serial.println();
}