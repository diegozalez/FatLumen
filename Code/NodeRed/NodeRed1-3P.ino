#include <Wire.h>
#include "Adafruit_MCP9808.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

Adafruit_MCP9808 temp = Adafruit_MCP9808();

const char* ssid = "Your WiFi SSID";
const char* password = "Your WiFi Password";
const char* mqtt_server = "Your MQTT Server(mosquitto)";
const uint8_t ledPin [] = {12,14,16}; 
uint8_t LEDs []={255,255,255};
IPAddress ip;  
byte mac[6];
boolean BuidlLED = false;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup() {
  pinMode(ledPin[0], OUTPUT);
  pinMode(ledPin[1], OUTPUT);
  pinMode(ledPin[2], OUTPUT);
  setLED(3,1,1,1); 
  pinMode(BUILTIN_LED, OUTPUT);
  LEDtoggle(20,50);
  delay(400);
  LEDtoggle();

  
  Serial.begin(9600);
  
  Serial.print("Starting FatLumen");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  digitalWrite(BUILTIN_LED,HIGH);

  setup_wifi();
  
  if (!temp.begin(0x18)) {
    Serial.println("Temp sensor error");
    delay(3000);
  }
  temp.setResolution(1);
  LEDtoggle(10,40);
  LEDtoggle(true);

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    sendStatus();
  }
   
  

}

void setLED(uint8_t speedTrans,uint8_t a,uint8_t b,uint8_t c){  
  LEDs[0]=a;
  LEDs[1]=b;
  LEDs[2]=c;
  analogWrite(ledPin[0], LEDs[0]);
  analogWrite(ledPin[1], LEDs[1]);
  analogWrite(ledPin[2], LEDs[2]);      
}

void setLED(){  
  analogWrite(ledPin[0], LEDs[0]);
  analogWrite(ledPin[1], LEDs[1]);
  analogWrite(ledPin[2], LEDs[2]);  
}

void sendStatus(){
  temp.wake();
  StaticJsonDocument<220> doc;
  doc["IP"][0] = ip[0];
  doc["IP"][1] = ip[1];
  doc["IP"][2] = ip[2];
  doc["IP"][3] = ip[3];
  doc["Mac"][0]=mac[3];
  doc["Mac"][1]=mac[2];
  doc["Mac"][2]=mac[1];
  doc["Mac"][3]=mac[0];
  doc["RSSI"]=WiFi.RSSI();
  doc["Temp"] = temp.readTempC();
  
  char buffer[256];
  size_t n = serializeJson(doc, buffer); 
  if (client.publish("FatLumen/Temp", buffer, n) == true) {
    Serial.println("Success sending message");
    LEDtoggle(2,2);
  } else {
    Serial.println("Error sending message");
  }
  temp.shutdown_wake(1);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    LEDtoggle();
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  ip=WiFi.localIP();
  Serial.println(ip);
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
  WiFi.macAddress(mac);

  
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  StaticJsonDocument<256> doc;
  deserializeJson(doc, payload, length);
  JsonArray LED = doc["LED"];
  int LED_0 = LED[0]; // 255
  int LED_1 = LED[1]; // 255
  int LED_2 = LED[2]; // 255
  Serial.print("LED0");Serial.println(LED_0);
  Serial.print("LED1");Serial.println(LED_1);
  Serial.print("LED2");Serial.println(LED_2);

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      LEDtoggle(5,500);
      
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void LEDtoggle(){
  BuidlLED = !BuidlLED;
  digitalWrite(BUILTIN_LED,BuidlLED);
}
void LEDtoggle(boolean b){
  BuidlLED = b;
  digitalWrite(BUILTIN_LED,BuidlLED);
}
void LEDtoggle(int t, int pause){
  for(;t>0;t--){
    BuidlLED = !BuidlLED;
  digitalWrite(BUILTIN_LED,BuidlLED);
  delay(pause);
  }
  
}
