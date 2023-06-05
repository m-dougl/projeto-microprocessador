#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

const char* SSID        = "";
const char* PASSWORD    = "";
const char* MQTT_SERVER = "broker.hivemq.com";
const char* MQTT_TOPIC  = "teste_casa_douglas";
int         MQTT_PORT   = 1883;
int         LED         = 13;

WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  digitalWrite(LED,LOW);

  WiFi.begin(SSID, PASSWORD);
  Serial.println("Connected!");
  client.setServer(MQTT_SERVER, MQTT_PORT);
  
  client.setCallback(callback);
  connectmqtt();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  if ((char)payload[0] == 'O' && (char)payload[1] == 'N') //on
  {
    digitalWrite(LED, HIGH);
    Serial.println("on");
    client.publish(topic, "LED turned ON");
  }
  else if ((char)payload[0] == 'O' && (char)payload[1] == 'F' && (char)payload[2] == 'F') //off
  {
    digitalWrite(LED, LOW);
    Serial.println(" off");
    client.publish(topic, "LED turned OFF");
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32_clientID")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(MQTT_TOPIC, "Nodemcu connected to MQTT");
      // ... and resubscribe
      client.subscribe(MQTT_TOPIC);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void connectmqtt()
{
  client.connect("ESP32_clientID");  // ESP will connect to mqtt broker with clientID
  {
    Serial.println("connected to MQTT");
    // Once connected, publish an announcement...

    // ... and resubscribe
    client.subscribe(MQTT_TOPIC); //topic=Demo
    client.publish(MQTT_TOPIC,  "connected to MQTT");

    if (!client.connected())
    {
      reconnect();
    }
  }
}
