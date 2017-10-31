/**
 * OTA STUFF
 */
#include <ArduinoOTA.h>           // OTA, must be included here. otherwise won't work.
#include <ESP8266mDNS.h>          // OTA

const char* host = "OTA-TBOT";

/**
 * test call
 */
void otaHandle(){
  ArduinoOTA.handle();  
}

/*
 * OTA
 * Needed for the OTA to function properly
 */  
void setupOTA(){
  
  ArduinoOTA.setHostname(host);
  
  ArduinoOTA.onStart([]() { // switch off all the PWMs during upgrade
                          sendMsg(groupId, "OTA UPDATE START");
                    });

  ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
                          sendMsg(groupId, "OTA UPDATE END");
                        });

  ArduinoOTA.onError([](ota_error_t error) {
                          sendMsg(groupId, "OTA UPDATE ERROR!!!");
                          ESP.restart(); 
                        });
                        
   /* setup the OTA server */
   ArduinoOTA.begin();

}

