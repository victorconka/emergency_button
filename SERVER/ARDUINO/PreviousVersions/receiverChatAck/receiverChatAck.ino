/*******************************************************************
*  An example of bot that show bot action message.                *
*                                                                 *
*                                                                 *
*                                                                 *
*  originally written by Vadim Sinitski                           *
*  new functionality added by VictorConKa                         *
 *******************************************************************/
//#include "FS.h"
//#include <time.h>                 // TIME    RETRIEVE TIME FOR ALERT
#include <WiFiClientSecure.h>     // IFTTT   USED BY IFTTT AND TELEGRAM
WiFiClientSecure client; 

/**
 * TELEGRAM MUST DEFINEVARIABLES
 */
String users[] = {"287078218"};
String groupId = "-162756488";
String BOTtoken = "393419505:AAHQMu21oQ2aZMOlL-HZtOqQ6tVpovuZRNs"; // your Bot Token (Get from Botfather)


/**
 * COMMON VARIABLES
 */
#define led D1              //LED pin
  #define button A0           //All the buttons are connected to A0. Buttons are destinguished by different resistor value
  int btn = 0;                //analog button value
  boolean ledState = false;   //led state for toggle
  String currentTime;         //used to store current time
  
  /**
   * COMMON
   * GET CURRENT TIME
   */
   /*
  void getCurrentTime(){
    time_t now = time(nullptr);
    currentTime = ctime(&now);
  }
  */
  
  /**
   *  COMMON
   *  Toggle led from low to high and viceversa 
   */
  void toggle(){
    if(ledState){
      ledState = false;
      digitalWrite(led, LOW);
    }else{
      ledState = true;
      digitalWrite(led, HIGH);
    }
  }
  
  /**
   * COMMON
   *  Send alert messages to all the contacts
   */
  String alertMsg;
  void alert(){
    Serial.println(ESP.getFreeHeap()); 
    //ALERT TELEGRAM
    //first make sure wifi is connected before sending message
    //connectWifi();
    alertMsg = "alert button pressed ";
    alertTelegram(alertMsg);
    Serial.flush();
    client.flush();
    Serial.println(ESP.getFreeHeap());
    //ALERT OTHER MEDIUMS 
    //triggerIFTTT("s_vicuk@mail.ru");
    //client.flush();
  }
  
  /**
   * COMMON
   * Read analog button value and decide what button was pressed.
   * Buttons are connected to different resistor values, therefore
 * each button gives a different value when read.
 */
void readButtons(){
  btn = analogRead(button)/100;
  if(btn!=0){
    Serial.println(btn);
    for(int i = 0;i < btn; i++){
      toggle();
      delay(300);
      toggle();
      delay(300);
    }
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);

  Serial.println("setup file system");
  //setup filesystem and load data from internal memory
  //setupFileSystem();
  
  Serial.println("connect wifi");
  //connect to wifi
  connectWifi();
  
  Serial.println("setup web server");
  //setup webserver
  //setupWebServer();
  
  Serial.println("setup time");
  //setup GreenWich time
  //configTime(0 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  
  Serial.println("setup nrf");
  //initialize nrf module
  setupNRF();
  
  Serial.println("setup ota");
  //setup OTA
  //setupOTA();
  Serial.println(ESP.getFreeHeap());
}

void loop() {
  // 0. READ BUTTONS
  readButtons();

  // 1. HANDLE WEB SERVER
  //handleWebServer();
      
  // 2. NRF24 LISTEN RADIO
  listenNRF();
  
  // 3. CHECK WIFI connection, and handle if not connected
  //connectWifi(); 
  
  // 4. Handle OTA
  //otaHandle();
  
  // 5. TELEGRAM
  listenTelegram();

  // 6. GET CURRENT TIME
  //getCurrentTime();
  
  
}
