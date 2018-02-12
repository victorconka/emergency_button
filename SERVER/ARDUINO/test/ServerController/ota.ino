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

  ArduinoOTA.setHostname(host);
  ArduinoOTA.onStart([]() { // switch off all the PWMs during upgrade

                    });
  ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end

                        });
  ArduinoOTA.onError([](ota_error_t error) {

                          ESP.restart(); 
                        });               
  ArduinoOTA.begin();

}
