//Simple notification script that sends message using Blynk framework.
//Uses deepSleep to save power and turns on using external interupt GPIO16 to RST

#include <BlynkSimpleEsp8266.h>
// #include <WiFi.h>
// #include <SPI.h>
#include "settings.h"

#define BLYNK_NO_BUILTIN
#define rst_pin             16
#define temp_pin             7
#define flash_pin            4

WidgetTerminal terminal(V1);
BlynkTimer timer;

void goToSleep(){
    // Serial.println("Goodbye!");
    terminal.println("Going to sleep");
    terminal.flush();
    yield();
    ESP.deepSleep(0);// must be zero for tilt sensor to work
}

void setup(){

    //disable ability to reset while on.
    pinMode(rst_pin, OUTPUT);
    digitalWrite(rst_pin, LOW);

    Blynk.begin(auth, ssid, passwd); 

    if(!Blynk.connected()){
        // Serial.println("Wifi failed to connect. Try changing ssid/pw settings.");
        terminal.println("Failed to connect.");
        terminal.flush();
    }
    long rssi = WiFi.RSSI();
    terminal.print("RSSI: ");
    terminal.println(rssi);
    terminal.flush();

    // Serial.println("Alert Sending...");
    Blynk.notify("MailBot Alert!");
    timer.setInterval(1000, goToSleep); //1 second
}

void loop(){
    Blynk.run();
    timer.run();   
}
