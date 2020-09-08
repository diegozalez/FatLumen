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
char auth[] = "PLACE TOKEN HERE";

Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
void setup()
{
  Serial.begin(9600);
  analogWrite(12, 3);
  analogWrite(14, 3);
  analogWrite(ledPin, 3);
  WiFi.mode(WIFI_STA);

  int cnt = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (cnt++ >= 10) {
      WiFi.beginSmartConfig();
      while (1) {
        delay(1000);
        if (WiFi.smartConfigDone()) {
          Serial.println();
          Serial.println("SmartConfig: Success");
          break;
        }
        Serial.print("|");
      }
    }
  }

  WiFi.printDiag(Serial);

  Blynk.config(auth);
  
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
