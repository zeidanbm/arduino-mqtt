import RPi.GPIO as IO
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import time
import json
import secrets_pi


trig = 16
echo = 22
led = 18

client = mqtt.Client()

def distance():
  IO.output(trig, True)

  time.sleep(0.00001)
  IO.output(trig, False)

  StartTime = time.time()
  StopTime = time.time()

  while IO.input(echo) == 0:
    StartTime = time.time()

  while IO.input(echo) == 1:
    StopTime = time.time()

  TimeElapsed = StopTime - StartTime
  distance = (TimeElapsed * 34300) / 2

  return distance


def loop():
  client.loop_start()
  try:
    while True:
      dist = distance()
      print ("Measured Distance = %.1f cm" % dist)
      data = {'distance': dist}
      mqtt_msg = json.dumps(data)
      client.publish(secrets_pi.MQTT_TOPIC, payload=mqtt_msg)
      if(dist > 300):
        IO.output(led, True)
      else:
        IO.output(led, False)
      time.sleep(1)

  except KeyboardInterrupt:
    client.loop_stop()
    destroy()


def on_connect(client, userdata, flags, rc):
  if rc == 0:
    print("Connected to MQTT Broker")
  else:
    print(f"Connection failed to MQTT Broker with code {rc}")


def on_publish(client, userdata, mid):
    print ("Message Published...")


def setup():
  IO.setmode(IO.BOARD)
  IO.setup(trig, IO.OUT)
  IO.setup(echo, IO.IN)
  IO.setup(led, IO.OUT)

  IO.output(led,IO.LOW)

  client.username_pw_set(secrets_pi.DEVICE_TOKEN)
  client.on_connect = on_connect
  client.on_publish = on_publish

  client.connect(secrets_pi.MQTT_HOST, secrets_pi.MQTT_PORT, 60)


def destroy():
  IO.cleanup()


if __name__ == '__main__':
  print ('Program is starting...')
  setup()
  loop()
