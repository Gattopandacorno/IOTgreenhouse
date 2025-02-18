#include <DHT.h>
#include <coap-simple.h>
#include <WiFi.h>
#include "WifiUtils.h"

int MoisturePin = 33;
int LightPin    = 32;
int HumidityPin = 16;
int Red = 25, Green = 26, Blue = 27;
int temp;

WiFiUDP udp;
Coap coap(udp);

DHT dht(HumidityPin, DHT11);

// INIT WIFI TO BE ON ACCESS POINT MODE
void initWifi()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(SSID, PASS);

  Serial.print("Access Point with IP: ");
  Serial.println(WiFi.softAPIP());
}

void callback_response(CoapPacket &packet, IPAddress ip, int port)
{
  Serial.print("[Coap Response got] ");

  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;

  Serial.println(p);
}

void setup() 
{
  Serial.begin(115200);
  dht.begin();
  initWifi();

  pinMode(Red, OUTPUT);
  pinMode(Green, OUTPUT);
  pinMode(Blue, OUTPUT);
  
  analogWrite(Red, 220);
  analogWrite(Green, 200);
  analogWrite(Blue, 100);

  coap.response(callback_response);
  coap.start();
}

void loop() 
{
  temp = dht.readTemperature() - 2.30;

  Serial.println(analogRead(MoisturePin));

  if(temp >= 30.0 || temp <= 8.0 || dht.readHumidity() <= 10.0)
    coap.put(IPAddress(192, 168, 4, 2), 5683, "buzzer", "1");

  delay(1000);
  coap.loop();
}


/**
 * CAPACITIVE SOIL MOISTURE SENSOR
 * GND  -> GND
 * VCC  -> 3V3
 * AUOT -> SP
 */


/**
 * HUMIDITY SENSOR
 *
 */

/**
 * LIGHT SENSOR
 *
 */