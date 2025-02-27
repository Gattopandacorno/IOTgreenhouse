#include <DHT.h>
#include <coap-simple.h>
#include <WiFi.h>
#include "WifiUtils.h"

int LightPin    = 32;
int HumidityPin = 16;
int Green = 25, Blue = 26, Red = 27;
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
  
  analogWrite(Blue, 0);
  analogWrite(Green, 0);
  analogWrite(Red, 0);

  coap.response(callback_response);
  coap.start();
}

void loop() 
{
  temp = dht.readTemperature() - 2.30;

  Serial.println(analogRead(LightPin));

  if(temp >= 30.0 || temp <= 8.0 || dht.readHumidity() <= 10.0 || analogRead(LightPin) == 4095)
  {
    Serial.println("Call");
    coap.put(IPAddress(192, 168, 4, 2), 5683, "buzzer", "1");
  }

  delay(2000);
  coap.loop();
}


/**
 * HUMIDITY SENSOR
 *
 */

/**
 * LIGHT SENSOR
 *
 */