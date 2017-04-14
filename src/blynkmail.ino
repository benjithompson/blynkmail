//Blynk Terminal V1

#include <BlynkSimpleEsp8266.h>
#include "settings.h"

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define BLYNK_PRINT Serial
#define rst_pin             16
#define temp_pin             7
#define flash_pin            2

bool door_open;
bool flash_enabled;
volatile long last_msg;
volatile bool mail_empty = true;
float tempF = 0;

WidgetTerminal terminal(V1);

BLYNK_WRITE(V1){
    //Terminal response here
}

void goToSleep(){
    yield();
    Serial.println("Goodbye.");
    ESP.deepSleep(0);// must be zero for tilt sensor to work
    yield();
}

void setup(){

    //Debug console
    Serial.begin(115200);
    Serial.println("Booting...");

    //Read GPIO 0 to see if in flash mode
    pinMode(flash_pin, INPUT);
    flash_enabled = digitalRead(flash_pin);

     
    if(flash_enabled == LOW){
        Serial.println("OTA enabled.");

        //enable ability to reset while on.
        pinMode(rst_pin, OUTPUT);
        digitalWrite(rst_pin, HIGH);

        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, passwd);
        while (WiFi.waitForConnectResult() != WL_CONNECTED) {
            Serial.println("Connection Failed! Rebooting...");
            delay(5000);
            ESP.restart();
        }

        //OTA Setup
        ArduinoOTA.onStart([]() {
            Serial.println("Start");
        });

        ArduinoOTA.onEnd([]() {
            Serial.println("\nEnd");
        });

        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        });

        ArduinoOTA.onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
            else if (error == OTA_END_ERROR) Serial.println("End Failed");
        });

        ArduinoOTA.begin();
        Serial.println("Ready");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());

    }else{
        Serial.println("Blynk Mail enabled.");

        //disable ability to reset while on.
        pinMode(rst_pin, OUTPUT);
        digitalWrite(rst_pin, LOW);

        Blynk.begin(auth, ssid, passwd);

        Serial.println("Alert Sending...");
        Blynk.notify("Mailbox Alert");

        int reading = analogRead(0);
        float voltage = reading * 3.3 / 1024;
        float tempC = (voltage - 0.5) * 100;
        tempF = (tempC * 9 / 5) + 32;

        Serial.print("TempF: ");
        Serial.println(tempF);
    }
}

void loop()
{
    if(flash_enabled == LOW){
        ArduinoOTA.handle();
    }else{
        Blynk.virtualWrite(V5, tempF);
        Blynk.run();
        goToSleep();   
    }
}
