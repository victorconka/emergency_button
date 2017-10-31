/*******************************************************************
*  An example of bot that show bot action message.                *
*                                                                 *
*                                                                 *
*                                                                 *
*  originally written by Vadim Sinitski                           *
*  new functionality added by VictorConKa                         *
 *******************************************************************/
#include <ESP8266WiFi.h>          // wifi connection
#include <UniversalTelegramBot.h> // Telegram API
#include <WiFiUDP.h>              // UDP packet for sending WOL magic packet
#include <WakeOnLan.h>            // WOL functionality
#include <ESP8266Ping.h>          // ping to check if PC is ON
#include <SPI.h>                  // SPI for NRF24
#include "RF24.h"                 // NRF24 LIBRARY
#include <time.h>                 // RETRIEVE TIME FOR ALERT
#include <IFTTTMaker.h>           // IFTTT - SEND EMAIL ALERTS
#include <WiFiClientSecure.h>     // USED BY IFTTT AND TELEGRAM


/**
 * COMMON VARIABLES
 */
#define led D1              //LED pin
#define button A0           //All the buttons are connected to A0. Buttons are destinguished by different resistor value
int btn = 0;                //analog button value
boolean ledState = false;   //led state for toggle
String currentTime;         //used to store current time

/**
 * IFTTT 
 * NEEDED TO TRIGGER EMAIL ALERTS
 */
#define KEY "d0GlVSA7ujDm6qNXpB5wr5"  // Get it from this page https://ifttt.com/services/maker/settings
#define EVENT_NAME "alert_need_help" // Name of your event name, set when you are creating the applet
WiFiClientSecure client;            
IFTTTMaker ifttt(KEY, client);

/**
 * NRF24
 */
#define CE_PIN D2             //NRF CE PIN
#define CSN_PIN D8            //NRF CSN PIN
RF24 radio(CE_PIN, CSN_PIN);  //NRF object 
//nrf variables
const byte address[][7] = {"server" , "button"};  //NRF communication roles
byte pipeNo;                  //pipe Number the message was received from
byte gotByte;                 //value recieved
byte gotBytePrev = 0;         //previous value received. needed to keep track of repeated messages

/**
 * NRF24
 * initialize nrf module
 */
void setupNRF(){ 
  radio.begin(); // Start up the radio
  radio.setAutoAck(1); // Ensure autoACK is enabled
  radio.setRetries(0,15); // Max delay between retries & number of retries
  radio.enableAckPayload();       // Allow optional ack payloads
  radio.openWritingPipe(address[1]); // Write to device address 'button'
  radio.openReadingPipe(1, address[0]); // Write to device address 'SimpleNode'
  radio.startListening();
}

/**
 * NRF24
 * listen to messages
 */
 void listenNRF(){
    while( radio.available(&pipeNo)){
      toggleOnce();
      radio.read( &gotByte, 1 );
      gotByte++;
      radio.writeAckPayload(pipeNo,&gotByte, 1 );
      Serial.print("Got Payload ");
      Serial.println(gotByte);
  }
}


/**
 * OTA STUFF
 */
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
const char* host = "OTA-TBOT";

/**
 * WOL STULL
 */
WiFiUDP UDP;
int src_port = 9;
//The target IP address to send the magic packet to.
IPAddress computer_ip(0,0,0,0); //ip used to wake up on lan
IPAddress static_ip(192,168,1,1); //ip used to ask if it's online
//The targets MAC address to send the packet to
//byte mac[] = {0x4c,0xcc,0x6a,0x60,0x46,0x88};  //miguelPC
//byte mac[] = {0x54,0x04,0xa6,0x2f,0x92,0xd9};   //viktorPc
byte mac[] = {0x1c,0x1b,0x0d,0xb1,0x2b,0xfa};   //Gigabyte miniPC


//------- WiFi Settings -------
char ssid[] = "MASMOVIL_339b";       // your network SSID (name)
char password[] = "tereventaretodoelculo";  // your network key
//char ssid[] = "Tamara";       // your network SSID (name)
//char password[] = "97531Tamara";  // your network key

/**
 * TELEGRAM STUFF
 */
String users[] = {"VictorConKa"};
// Initialize Telegram BOT
#define BOTtoken "393419505:AAHQMu21oQ2aZMOlL-HZtOqQ6tVpovuZRNs" // your Bot Token (Get from Botfather)
String groupId = "-162756488";
//WiFiClientSecure client;  //CREATED BY IFTTT PREVIOUSLY
UniversalTelegramBot bot(BOTtoken, client);
int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done

/*
 * TELAGRAM
 * Checks if a user is authorized
 */
boolean containUser(String user){
  for(int i = 0; i < sizeof(users); i++){
      if( user.equals(users[i])) {
          return true;
        }
    }
    return false;
}

/*
 * TELEGRAM
 * Send a message to a chat
 */
void sendMsg(String chat_id, String message){
    Serial.print("message to: ");
    Serial.println(chat_id);
    bot.sendChatAction(chat_id, "typing");
    bot.sendMessage(chat_id, message);
  }

/**
 * TELEGRAM
 * actions based on telegram commands
 */
void actions(String text, String chat_id){

    if (text == "/led1_on") {
        digitalWrite(led, HIGH);
        sendMsg(chat_id, "Turning led1 on");        
    }
    
    else if (text == "/led1_off") {
        digitalWrite(led, LOW);
        sendMsg(chat_id, "Turning led1 off");
    }

    
    else if (text =="/wol"){
       WakeOnLan::sendWOL(computer_ip, UDP, mac, sizeof mac);  
       sendMsg(chat_id, "Se ha enviado un paquete magico");
    }

    else if (text == "/ping"){
        if (Ping.ping(static_ip),2) {
          sendMsg(chat_id, "Success pinging: " + static_ip.toString());
        } else {
          sendMsg(chat_id,"Error :(,  pinging: " + static_ip.toString());
      }
    } 

  }
  
/**
 * TELEGRAM
 * user is not authorized to execute any action
 */
void noAutorizado(String chat_id, String from_name){
    sendMsg(chat_id, from_name + "No estas autorizado ... Ponte en contacto con el administrador y solicita permiso.");
    sendMsg(groupId, "Acceso no autorizado de: " + from_name);
  }

/**
 * TELEGRAM
 * handle messages and execute corresponding actions
 */
void handleNewMessages(int numNewMessages) {

  for (int i=0; i<numNewMessages; i++) {
    
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";
    
    Serial.println(from_name);
    text.remove(text.indexOf('@'), text.length()-1);
    Serial.println(text);
    
    if(containUser(from_name)){
      actions(text, chat_id);
    }else{
      noAutorizado(chat_id, from_name);
     }
    
    Serial.println("---");
    
  }
}

/**
 * TELEGRAM
 * listen telegram api
 */
 void listenTelegram(){
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    //handle telegram messages
    while(numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    Bot_lasttime = millis();
  }  
}


/**
 * WIFI
 * handles wifi connections
 * whenever the device is disconnected from wifi it does the followind:
 ****** if wifi ap is not present -> retry connection -> become AP and let the user define bssid and password
 */
void connectWifi(){
  while(WiFi.status() != WL_CONNECTED){
    // Set WiFi to station mode and disconnect from an AP if it was Previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    
    // Attempt to connect to Wifi network:
    Serial.print("Connecting Wifi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
  
    
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
    }
    
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP().toString());
    
    UDP.begin(src_port); //required for the WOL magic packet 
  }
  
}
  
/*
 * OTA
 * Needed for the OTA to function properly
 */  
void setupOTA(){
  
  ArduinoOTA.setHostname(host);
  
  ArduinoOTA.onStart([]() { // switch off all the PWMs during upgrade
                          sendMsg(groupId, "OTA START");
                    });

  ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
                          sendMsg(groupId, "OTA END");
                        });

  ArduinoOTA.onError([](ota_error_t error) {
                          sendMsg(groupId, "OTA ERROR!!!");
                          ESP.restart(); 
                        });
                        
   /* setup the OTA server */
   ArduinoOTA.begin();

}

/**
 * COMMON
 * GET CURRENT TIME
 */
void getCurrentTime(){
  time_t now = time(nullptr);
  currentTime = ctime(&now);
}

/**
 * COMMON
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
void alert(){
  Serial.println(ESP.getFreeHeap()); 
  //ALERT TELEGRAM
  String alertMsg = "alert button pressed at " + currentTime;
  String viktest= "287078218";
  //sendMsg(groupId,alertMsg);
  //make sure wifi is connected before sending message
  connectWifi();
  sendMsg(viktest,alertMsg);
  
  //ALERT OTHER MEDIUMS 
  //triggerEvent takes an Event Name and then you can optional pass in up to 3 extra Strings

}

/**
 * COMMON
 * Make sure led toggles only once because the button
 * sends message until ack is received, so such message
 * is sent and received by the server more than once
 */
void toggleOnce(){
  if(gotBytePrev != gotByte){
    toggle();
    alert();
    gotBytePrev = gotByte;
  }
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
  
  //setup GreenWich time
  configTime(0 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  
  //connect to wifi
  connectWifi();

  //initialize nrf module
  setupNRF();

  //setup OTA
  setupOTA();

  String mensaje = "Telegram bot online with ip: " + WiFi.localIP().toString();
  sendMsg(groupId, mensaje);
}

void loop() {
  // 0. READ BUTTONS
  readButtons();

  // 1. NRF24 LISTEN RADIO
  listenNRF();
  
  // 2. CHECK WIFI connection, and handle if not connected
  connectWifi(); 
  
  // 3. Handle OTA
  ArduinoOTA.handle();

  // 4. TELEGRAM
  listenTelegram();

  // GET CURRENT TIME
  getCurrentTime();
}
