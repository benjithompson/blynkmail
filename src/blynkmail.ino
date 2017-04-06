/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Social networks:            http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

  This example shows how to send values to the Blynk App,
  when there is a widget, attached to the Virtual Pin and it
  is set to some frequency

  Project setup in the app:
    Value Display widget attached to V5. Set any reading
    frequency (i.e. 1 second)
 *************************************************************/

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "settings.h"

// Use Virtual pin 5 for uptime display
#define PIN_MAIL V5
#define recvPin 13
#define sendPin 12

/* Comment this out to disable prints and save space */
// #define BLYNK_PRINT Serial

void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, passwd);
  Blynk.notify("You received mail!");
}

void loop()
{

  //If arduino sent the door closed is 1
  if(recvPin){
    Blynk.virtualWrite(V5, "CLOSED");

    //received door closed from Arduino.
    //Telling arduino status has been updated.
    digitalWrite(sendPin, HIGH);
  }else{
    Blynk.virtualWrite(V5, "OPEN");
  }

  Blynk.run();

}
