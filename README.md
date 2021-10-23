# Arduino MKR1010 And Raspberry pi with MQTT
We are using an Arduino MKR1010 and a Raspberry Pi 4 model B to send sensor data to an mqtt broker (ThingsBoard). The sensors used are a temperature sensor, motion sensor, distance sensor and a sound sensor. Also a led is used which is connected to PWM pin on the arduino/raspberry pi to be controlled from the ThingsBaord dashboard.

## Setup
To run this, you will need to change rename the file secrets-example.h into secrets.h and add your constants to the file using your own wifi secrets and your mqtt broker address/port/topic.