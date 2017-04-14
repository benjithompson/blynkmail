//Blynk Terminal V1
//Blynk Button   V2
//Blynk Button2 V10
//Blynk TempF    V5

#include <BlynkSimpleEsp8266.h>
#include "settings.h"

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>

// #define BLYNK_PRINT Serial
#define BLYNK_NO_BUILTIN
#define rst_pin             16
#define temp_pin             7
#define flash_pin            2

bool door_open;
bool flash_enabled;
volatile long last_msg;
volatile bool mail_empty = true;

float tempF = 0;

uint8_t wifi_step = 0;
char* t_ssid = "00000000000000000000000000000000";
char* t_pw = "00000000000000000000000000000001";

int ssid_addr = 0;
int pw_addr = 50;
// const char* t_auth = "";

WidgetTerminal terminal(V1);
BlynkTimer timer;


void wifiSettings(){
    if(wifi_step == 0){        
        terminal.println("Enter wifi ssid: ");
        terminal.flush();
        wifi_step = 1;
        return;
    }
}

//Terminal Input
BLYNK_WRITE(V1){

    if(wifi_step > 0 && flash_enabled){
        switch(wifi_step){
            case 1:{
                String y_ssid = param.asStr();
                y_ssid.toCharArray(t_ssid, y_ssid.length()+1);
                terminal.println("Enter wifi password: ");
                terminal.flush();
                EEPROM.put(ssid_addr, t_ssid);
                wifi_step = 2;
                return;
            }
            case 2:{
                String y_pw = param.asStr();
                y_pw.toCharArray(t_pw, y_pw.length()+1);
                terminal.println("Wifi settings saved!");
                terminal.flush();
                EEPROM.put(pw_addr, t_pw);
                wifi_step = 0;
                return;
            }
        }
    }else{
        //Terminal response here
        terminal.println("unknown command.");
        terminal.flush();
    }
}

BLYNK_WRITE(V2){
    Serial.println("Reset button pressed.");
    terminal.println("Resetting...");
    terminal.flush();
    ESP.reset();
}
//Button to start WIFI settings
BLYNK_WRITE(V10){
    if(flash_enabled){
        wifiSettings();
    }
}

void goToSleep(){
    Serial.println("Goodbye!");
    yield();
    ESP.deepSleep(0);// must be zero for tilt sensor to work
}


void vWriteTemp(){
    int reading = analogRead(0);
    float voltage = reading * 3.3 / 1024;
    float tempC = (voltage - 0.5) * 100;
    tempF = (tempC * 9 / 5) + 32;
    Serial.print("TempF: ");
    Serial.println(tempF);
    Blynk.virtualWrite(V5, tempF);
}

void setButton1Norm(){
    Blynk.setProperty(V2, "offLabel", "Zzz..");
    Blynk.setProperty(V2, "onLabel", ":O");
    Blynk.setProperty(V2, "color", "#1261e2");
    Blynk.setProperty(V2, "label", "Normal Mode");
}

void setButton2Norm(){
    Blynk.setProperty(V10, "offLabel", "Sleeping");
    Blynk.setProperty(V10, "onLabel", ":D");
    Blynk.setProperty(V10, "color", "#1261e2");
    Blynk.setProperty(V10, "label", "Waiting");
}

void setButton1OTA(){
    Blynk.setProperty(V2, "offLabel", "Reset");
    Blynk.setProperty(V2, "onLabel", "Resetting...");
    Blynk.setProperty(V2, "color", "#a81303");
    Blynk.setProperty(V2, "label", "OTA POWER");
}

void setButton2OTA(){
    Blynk.setProperty(V10, "offLabel", "Wifi Setup");
    Blynk.setProperty(V10, "onLabel", ":D");
    Blynk.setProperty(V10, "color", "#00af1d");
    Blynk.setProperty(V10, "label", "SETTINGS");
}

void setup(){

    //Debug console
    Serial.begin(115200);
    Serial.println("Booting...");

    //Read GPIO 0 to see if in flash mode
    pinMode(flash_pin, INPUT);
    flash_enabled = !digitalRead(flash_pin);

    if(flash_enabled == true){
        //default wifi
        Blynk.begin(auth, ssid, passwd);

        Serial.println("OTA enabled.");

        terminal.println("OTA enabled.");
        terminal.print("IP address: ");
        terminal.println(WiFi.localIP());
        terminal.flush();

        setButton1OTA();
        setButton2OTA();

        //enable ability to reset while on.
        pinMode(rst_pin, OUTPUT);
        digitalWrite(rst_pin, HIGH);

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

        Serial.print("IP address: ");
        ArduinoOTA.begin();
        Serial.println(WiFi.localIP());



    }else{

        EEPROM.get(ssid_addr, t_ssid);
        EEPROM.get(pw_addr, t_pw);
        Serial.print("EEPROM ssid: ");
        Serial.println(t_ssid);
        Serial.print("EEPROM pw: ");
        Serial.println(t_pw);


        Blynk.begin(auth, t_ssid, t_pw);
        if(!Blynk.connected()){
            Serial.println("Wifi failed to connect. Try changing ssid/pw settings.");
            terminal.println("Failed to connect. Change wifi settings in OTA mode.");
            terminal.flush();
        }

        Serial.println("Blynk Mail enabled.");
        terminal.println("MailBot activated!");
        terminal.flush();

        Serial.println("Alert Sending...");
        Blynk.notify("Mailbox Alert");

        //disable ability to reset while on.
        pinMode(rst_pin, OUTPUT);
        digitalWrite(rst_pin, LOW);

        timer.setInterval(500, vWriteTemp);
        timer.setInterval(2000, setButton1Norm);
        timer.setInterval(2500, setButton2Norm);
        timer.setInterval(3000, goToSleep); //3 seconds
    }
}

void loop(){
    Blynk.run();
    timer.run();   
    if(flash_enabled == true){
        ArduinoOTA.handle();
    }
}
