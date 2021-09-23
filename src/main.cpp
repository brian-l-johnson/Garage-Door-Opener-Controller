#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//Config is externalized to the Config.h, customize for your uses
#include <Config.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);

String wifiMAC;

void init_wifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PW);
    wifiMAC = WiFi.macAddress();
    wifiMAC.replace(":", "");
    Serial.println("MAC Address: "+wifiMAC);

    Serial.println("Connecting to wifi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }
    Serial.println("Wifi connected. IP = " + WiFi.localIP().toString());
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String topicString = String(topic);
  String payloadString = "";
  for(int i=0; i<length; i++) {
    payloadString += (char) payload[i];
  }
  Serial.println(topicString+":"+payloadString);

  if(payloadString.equals("activate")) {
    digitalWrite(D1, HIGH);
    delay(500);
    digitalWrite(D1, LOW);
  }
}

void mqttReconnect() {
  if(mqttClient.connect("garageClient", MQTT_USER, MQTT_PASS)) {
    Serial.println("Connected to mqtt!");

    Serial.println(MQTT_TOPIC + String("/") + wifiMAC);
    mqttClient.subscribe((MQTT_TOPIC + String("/") + wifiMAC).c_str());
    mqttClient.publish("garage/register", wifiMAC.c_str() );
  }
  else {
    Serial.println("failed to connect to mqtt server :-(");
    Serial.println(mqttClient.state());
  }
}

void setup() {
  pinMode(D1, OUTPUT);
  Serial.begin(115200);
  delay(10);
  Serial.println("Starting...");
  init_wifi();
  IPAddress mqttIP;
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  mqttReconnect();

}

void loop() {
  if(!mqttClient.connected()) {
    mqttReconnect();
  }
  mqttClient.loop();

}