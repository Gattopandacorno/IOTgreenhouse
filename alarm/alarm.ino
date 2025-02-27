#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>
#include "WifiUtils.h"

int TouchPin = 13;
int BuzzPin  = 15;

IPAddress ip(192,168, 4, 2); // Defines the ip address for this device
IPAddress gateway(192, 168, 4, 1); // Defines the gateway (ESP32 device)
IPAddress subnet(255, 255, 255, 0);
WiFiUDP udp;
Coap coap(udp); // Initializes coap variable with the transmission channel
bool bip = false; // Used to know when the buzzer should beep or not

void initWifi()
{
  WiFi.config(ip, gateway, subnet); // Configuring which ip address the device should have
  WiFi.begin(SSID, PASS); // Begins to search the wifi and tries to connect with the password

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
}

void setup() 
{
  Serial.begin(9600);

  WiFi.mode(WIFI_STA); // different mode from board1 
  WiFi.disconnect(); // disconnect before trying to connect again
  initWifi();

  pinMode(TouchPin, INPUT);
  digitalWrite(TouchPin, LOW);  // done to prevent the pin to be always HIGH


  coap.server(callback_buzzer, "buzzer");
  coap.start();
}

void loop() 
{
  if(bip)
  {
    tone(BuzzPin, 100, 100); // Sound up when bip is true
    delay(100);

    // If the touch sensor is touched it means that buzzer 
    // should not beep
    if(digitalRead(TouchPin) == HIGH) 
      bip = false;
  }

  coap.loop();  
}
