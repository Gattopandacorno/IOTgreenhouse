#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>
#include "WifiUtils.h"

int TouchPin = 13;
int BuzzPin  = 15;
IPAddress ip(192,168, 4, 2);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);
WiFiUDP udp;
Coap coap(udp);
bool bip = false;

void initWifi()
{
  //if(WiFiScan ssid not exists) return
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(SSID, PASS);

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(2000);
    Serial.println("Trying to connect...");  
  }
}

// Server coap to beep the buzzer
void callback_buzzer(CoapPacket &packet, IPAddress ip, int port)
{
  Serial.println("[Buzzer] ON");
 
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;

  String message(p);
  if (message.equals("1"))
  {
    bip = true;
    coap.sendResponse(ip, port, packet.messageid, "1");
  }
  else if (message.equals("-1")) //TODO: improve
  {
    WiFi.forceSleepBegin();
  }

}

void setup() 
{
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  initWifi();

  pinMode(TouchPin, INPUT);
  digitalWrite(TouchPin, LOW);

  coap.server(callback_buzzer, "buzzer");
  coap.start();
}

void loop() 
{
  if(bip)
  {
    tone(BuzzPin, 100, 100);
    delay(100);

    if(digitalRead(TouchPin) == HIGH) 
      bip = false;
  }

  coap.loop();  
}
