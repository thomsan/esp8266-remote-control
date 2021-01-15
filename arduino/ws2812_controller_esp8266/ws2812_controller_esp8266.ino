
/*
* This example works for ESP8266 and uses the NeoPixelBus library instead of the one bundle
* Sketch contributed to by Joey Babcock - https://joeybabcock.me/blog/
* Codebase created by ScottLawsonBC - https://github.com/scottlawsonbc
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// Maximum number of packets to hold in the buffer. Don't change this.
#define N_GPIOS 16

// IO pin settings
#define IOPin 5 // make sure to set this to the correct pin, ignored for Esp8266(set to 3 by default for DMA)

// Wifi and socket settings
const char *ssid = "YOUR WIFI SSID";
const char *password = "YOUR WIFI PASSWORD";
unsigned int localPort = 7777;
// set to 1 for each active GPIO pin (GPIO0 - GPIO15)
//                GPIO: 0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10,11,12,13,14,15
char GPIO_SELECTOR[] = {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char value_buffer[N_GPIOS];

WiFiUDP udpServer;

// Network information
// IP must match the IP in config.py in python folder
IPAddress ip(192, 168, 178, 11);
// Set gateway to your router's gateway
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

void setup()
{
  Serial.begin(115200);
  pinMode(IOPin, OUTPUT);
  digitalWrite(IOPin, LOW);

  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  Serial.println("");
  // Connect to wifi and print the IP address over serial
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  udpServer.begin(localPort);
}

/** Receive UDP messages and send an echo back */
void process_incoming_udp()
{
  if (udpServer.parsePacket())
  {
    // Fetch received message
    int len = udpServer.read(value_buffer, sizeof(value_buffer));
    Serial.print("len: ");
    Serial.println(len);

    // Display the message
    Serial.print(F("Received from "));
    Serial.print(udpServer.remoteIP());
    Serial.print(":");
    Serial.print(udpServer.remotePort());
    Serial.print(": ");
    for (int i = 0; i < len; i++)
    {
      byte on = value_buffer[i];
      Serial.print(on);
    }
    Serial.println();

    // Send echo back
    udpServer.beginPacket(udpServer.remoteIP(), udpServer.remotePort());
    udpServer.print(F("Echo: "));
    udpServer.print(value_buffer);
    udpServer.endPacket();

    // Execute some commands
    for (int i = 0; i < len; i++)
    {
      if (GPIO_SELECTOR[i])
      {
        Serial.print("Setting GPIO");
        Serial.print(i);
        Serial.println(" to: ");
        Serial.println(value_buffer[i]);
        digitalWrite(IOPin, value_buffer[i]);
      }
    }
  }
}

/** Main loop, executed repeatedly */
void loop()
{
  process_incoming_udp();
}
