/* Code for FatLumen v1.0
 *  wirten by diegozalez  
 *  More info in https://github.com/diegozalez/FatLumen
 */
 
#define BLYNK_PRINT Serial

#include <Wire.h>
#include "Adafruit_MCP9808.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266_SSL.h>
const int ledPin = 16; 
// Your token.
char auth[] = "YOUR TOKEN";

// Your WiFi credentials.
char ssid[] = "WIFI SSID";
char pass[] = "WIFI PASSWORD";

Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
void setup()
{
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  tempsensor.begin(0x18);
  tempsensor.setResolution(0);
}

BLYNK_WRITE(V1)
{
  analogWrite(ledPin, param.asInt());
}
BLYNK_READ(V2)
{
  tempsensor.wake();
  Blynk.virtualWrite(V2, tempsensor.readTempC());
  tempsensor.shutdown_wake(1);
  
}

void loop()
{
  Blynk.run();
}
