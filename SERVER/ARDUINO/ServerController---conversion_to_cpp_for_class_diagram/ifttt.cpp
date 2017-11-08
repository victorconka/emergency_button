/**
* Este archivo contiene los metodos y procedimientos necesarios para el uso del servicio IFTTT.
*/

/**
 * Aplet used for this app can be found here:
 * https://ifttt.com/applets/61724532d-send-email-with-the-esp8266
 * To TEST the event visit : https://maker.ifttt.com/use/REPLACE_ME
 * to get your id visit : https://ifttt.com/services/maker_webhooks/ and go to settings
 * to get your key visit: https://ifttt.com/services/maker_webhooks/settings
 * 
 */
class ifttt{
	public:
	
bool triggerIFTTT(String value1){
if (DEBUG){
  Serial.println(F("triggerIFTTT()"));
}
  bool ret = false;
  std::unique_ptr<IFTTTMaker> ifttt;
  ifttt.reset(new IFTTTMaker(ifttt_key, client));
  if(ifttt->triggerEvent(ifttt_event_name, value1)){
      if (DEBUG){
        Serial.println(F("Successfully sent"));
      }
      ret = true;
  } else
  {
    if (DEBUG){
      Serial.println(F("Failed!"));
    }
    ret = false;
  }
  return ret;
}
}