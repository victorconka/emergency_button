/**
 *  THIS SOFTWARE IS FREE TO MODIFY AND REDISTRIBUTE.
 *  TOO MANY PEOPLE CONTRIBUTED TO THE CREATION OF ALL
 *  THE LIBRARIES USED. THE EFFORT IS APPRECIATED.
 * 
 *  ALL THE LIBRARIES ARE INCLUDED HERE.
 *  THAT WAY IT IS EASIER TO CONTROL THE ORDER THEY'RE
 *  LOADED IN.
 *  
 *  VARIABLES ARE STORED HERE. THAT WAY IT WILL BE IMPOSSIBLE
 *  TO MESS UP ACCESSING THEM
 */
#include "lwip/tcp_impl.h"        // TELEGRAM avoids memory leak.
#include <UniversalTelegramBot.h> // TELEGRAM
#include "FS.h"                   // ESP8266 STORAGE MEMORY MANAGEMENT
#include <ArduinoJson.h>          // ESP8266 json implementation
#include <ESP8266Ping.h>          // ESP8266 ping library
#include <ESP8266WiFi.h>          // WIFI    https://github.com/esp8266/Arduino
#include <DNSServer.h>            // WIFI
#include <ESP8266WebServer.h>     // WIFI
#include <WiFiManager.h>          // WIFI    https://github.com/tzapu/WiFiManager
#include <WiFiUDP.h>              // WIFI    WOL
#include <WakeOnLan.h>            // WIFI    WOL
#include <SPI.h>                  // NRF24   SPI
#include "RF24.h"                 // NRF24   LIBRARY
#include "nRF24L01.h"             // NRF24   LIBRARY HEADERS
#include <IFTTTMaker.h>           // IFTTT   SEND EMAIL ALERTS
#include <WiFiClientSecure.h>     // IFTTT   USED BY IFTTT AND TELEGRAM
#include <ArduinoOTA.h>           // OTA, must be included here. otherwise won't work.

class ServerController{

/**
 * COMMON VARIABLES
 */
#define blueLed D1              // LED pin
#define redLed D3               // LED pin
#define button A0               // Button pin. All the buttons are connected to A0. Buttons are destinguished by different resistor value
bool ALERT = false;             // FLAG TO MAKE SURE ALERT WILL BE RAISED IF A SUDDEN REBOOT WILL HAPPEN
int btn;                        // analog button value
bool ledState = false;          // led state for toggle
bool DEBUG = true;              // enable or disable debug messages
bool SETTINGS_MODE = false;     // controls what mode device initializes in.
const int bufferSize = 2300;    // size of the buffer for reading/writing SPIFFS files 



void setup() {
  Serial.begin(115200); 
  fileSystem.setupFileSystem();   // initialize FS and load data from internal memory.
  wifi.setupWifi();         // initialize wifi
  
  if(!SETTINGS_MODE){
    Serial.println("loading nrf and telegram");
    ESP.wdtDisable();
    ESP.wdtEnable(WDTO_8S);
    nrf24.setupNRF();       // initialize nrf module
    telegram.setupTelegram();  // initialize telegram  
  }else{
    configWebSite.setupWebServer();
    ota.setupOTA();    
    //SETTINGS_MODE must be turned off after first start.
    SETTINGS_MODE = false;
    saveConfig();
    SETTINGS_MODE = true;
  }
  common.configIOPins();
}

void loop() {
  if(!SETTINGS_MODE){
    //normal, non settings mode.
    common.readButtons(); //read buttons
    listenNRF();
    readButtons(); //read buttons
    listenTelegram();
    ESP.wdtFeed();
  }else{
    // we're in settings mode
    otaHandle();
    handleWebServer();
  }
  
}
};
