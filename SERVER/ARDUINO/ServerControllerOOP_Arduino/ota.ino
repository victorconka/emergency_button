class ota{

/**
 * OTA 
 */
const char* host = "OTA-TBOT";
  
/**
 * LISTEN TO OTA
 */
void otaHandle(){
  ArduinoOTA.handle();  
}

/*
 * OTA
 * Needed for the OTA to function properly
 */  
void setupOTA(){
if(DEBUG){
  Serial.println(F("setupOTA()"));  
}
  ArduinoOTA.setHostname(host);
  ArduinoOTA.onStart([]() { // switch off all the PWMs during upgrade
                          if(DEBUG){
                            Serial.println(F("OTA UPDATE START"));  
                          }
                    });
  ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
                          if(DEBUG){
                            Serial.println(F("OTA UPDATE END"));  
                          }
                        });
  ArduinoOTA.onError([](ota_error_t error) {
                          if(DEBUG){
                            Serial.println(F("OTA UPDATE ERROR!!!"));  
                          }
                          ESP.restart(); 
                        });               
  ArduinoOTA.begin();

}
};
