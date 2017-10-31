/**
 * Aplet used for this app can be found here:
 * https://ifttt.com/applets/61724532d-send-email-with-the-esp8266
 * To TEST the event visit : https://maker.ifttt.com/use/REPLACE_ME
 * to get your id visit : https://ifttt.com/services/maker_webhooks/ and go to settings
 * 
 */

#include <IFTTTMaker.h>           // IFTTT   SEND EMAIL ALERTS
#include <WiFiClientSecure.h>     // IFTTT   USED BY IFTTT AND TELEGRAM


/**
 * IFTTT 
 * NEEDED TO TRIGGER EMAIL ALERTS
 */
 
#define KEY "cn8c-3rHX6exivY1t8jCYy"  // Get it from this page https://ifttt.com/services/maker_webhooks/settings
                                      //it looks like this :https://maker.ifttt.com/use/cn8c-3rHX6exivY1t8jCYy, only id is needed
#define EVENT_NAME "email_alert"      // Name of your event name, set when you are creating the applet
WiFiClientSecure client;              // client needed to trigger ifttt event and to communicate with telegram api  
IFTTTMaker ifttt(KEY, client);        // IFTTT magic

void triggerIFTTT(){
  if(ifttt.triggerEvent(EVENT_NAME)){
    Serial.println("Successfully sent");
  } else
  {
    Serial.println("Failed!");
  }
}
