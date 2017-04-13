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

#define BLYNK_PRINT Serial
#define in_pin             14
#define openMail_pin       12
#define rstPin             16
#define sensor_wait      1000
#define buzzerPin           4
#define temp_pin            7

bool door_open;
volatile long last_msg;
volatile bool mail_empty = true;
float tempF = 0;

WidgetTerminal terminal(V1);

IPAddress esp_ip (192,168,10,1);
IPAddress dns_ip(8,8,8,8);
IPAddress gateway_ip(192,168,1,1);
IPAddress subnet_mask(255,255,255,0);
byte esp_mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

BLYNK_WRITE(V1){

}

void goToSleep(){
    yield();
    Serial.println("Goodbye.");
    ESP.deepSleep(0);// must be zero for tilt sensor to work
    yield();
}

void setup(){

    //disable ability to reset while on.
    pinMode(rstPin, OUTPUT);
    digitalWrite(rstPin, LOW);

    // Debug console
    Serial.begin(115200);

    Blynk.config(auth, "blynk-cloud.com", 8442);
    while(Blynk.connect(1000) == false){
        Serial.println(".");
    }

    Serial.println("Alert Sending...");
    Blynk.notify("Mailbox Alert");

    int reading = analogRead(0);
    float voltage = reading * 3.3 / 1024;
    float tempC = (voltage - 0.5) * 100;
    tempF = (tempC * 9 / 5) + 161;

    Serial.print("TempF: ");
    Serial.println(tempF);
    
    //input of door switch
    pinMode(in_pin, INPUT_PULLUP);
    pinMode(openMail_pin, INPUT_PULLUP);

    terminal.print(F("Blynk v" BLYNK_VERSION ": Mail door opened. \n"));
    terminal.flush();
}

void loop()
{
    Blynk.virtualWrite(V5, tempF);
    Blynk.run();
    goToSleep();   
}
