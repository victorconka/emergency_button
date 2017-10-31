/*******************************************************************
*  An example of bot that show bot action message.                *
*                                                                 *
*                                                                 *
*                                                                 *
*  written by Vadim Sinitski                                      *
 *******************************************************************/
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <WiFiUDP.h>
#include <WakeOnLan.h>
#include <ESP8266Ping.h>

//OTA
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

const char* host = "OTA-TBOT";

WiFiUDP UDP;
int dst_port = 9;
int src_port = 9;
/**
 * The target IP address to send the magic packet to.
 */
IPAddress computer_ip(0,0,0,0); //ip used to wake up on lan
IPAddress static_ip(192,168,0,1); //ip used to ask if it's online

/**
 * The targets MAC address to send the packet to
 */
//byte mac[] = {0x4c,0xcc,0x6a,0x60,0x46,0x88};  //miguelPC
//byte mac[] = {0x54,0x04,0xa6,0x2f,0x92,0xd9};   //viktorPc
byte mac[] = {0x1c,0x1b,0x0d,0xb1,0x2b,0xfa};   //Gigabyte miniPC

String users[] = {"VictorConKa"};

//------- WiFi Settings -------
/*
char ssid[] = "JAZZTEL_MF95";       // your network SSID (name)
char password[] = "tereventaretodoelculo";  // your network key
*/
char ssid[] = "Tamara";       // your network SSID (name)
char password[] = "97531Tamara";  // your network key
int led_pin1 = D1;
int led_pin2 = D2;
int led_pin3 = D3;
int dimmer_pin[] = {D1, D2, D3};


// Initialize Telegram BOT
#define BOTtoken "393419505:AAHQMu21oQ2aZMOlL-HZtOqQ6tVpovuZRNs" // your Bot Token (Get from Botfather)
String groupId = "-235935043";

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
//bool Start = false;

/*
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
 * Send a message to a chat
 */
void sendMsg(String chat_id, String message){
    bot.sendChatAction(chat_id, "typing");
    bot.sendMessage(chat_id, message);
  }


void actions(String text, String chat_id){

    if (text == "/led1_on") {
        digitalWrite(led_pin1, HIGH);
        sendMsg(chat_id, "Turning led1 on");        
    }
    
    else if (text == "/led1_off") {
        digitalWrite(led_pin1, LOW);
        sendMsg(chat_id, "Turning led1 off");
    }
  
    else if (text == "/led2_on") {
        digitalWrite(led_pin2, HIGH);
        sendMsg(chat_id, "Turning led2 on");
    }
    
    else if (text == "/led2_off") {
        digitalWrite(led_pin2, LOW);
        sendMsg(chat_id, "Turning led2 off");
    }

    else if (text == "/led3_on") {
        digitalWrite(led_pin3, HIGH);
        sendMsg(chat_id, "Turning led3 on");
    }
    
    else if (text == "/led3_off") {
        digitalWrite(led_pin3, LOW);
        sendMsg(chat_id, "Turning led3 off");
    }
    
    else if (text =="/wol"){
       WakeOnLan::sendWOL(computer_ip, UDP, mac, sizeof mac, dst_port);  
       bot.sendChatAction(chat_id, "typing");
       bot.sendMessage(chat_id, "Se ha enviado un paquete magico");
    }

    else if (text == "/ping"){
        if (Ping.ping(static_ip),2) {
          bot.sendMessage(chat_id,"Success pinging: " + static_ip.toString());
        } else {
          bot.sendMessage(chat_id,"Error :(,  pinging: " + static_ip.toString());
      }
    } 

  }
void noAutorizado(String chat_id, String from_name){
    sendMsg(chat_id, from_name + "No estas autorizado ... Ponte en contacto con el administrador y solicita permiso.");
    sendMsg(groupId, "Acceso no autorizado de: " + from_name);
  }

  
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


void connectWifi(){
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
  
  UDP.begin(src_port); //start UDP client, not sure if really necessary.
  
  String mensaje = "Estoy en línea " + WiFi.localIP().toString();
  sendMsg(groupId, mensaje);
  
  }
  
/*
* Needed for the OTA to function properly
*/  
void setupOTA(){
  
  ArduinoOTA.setHostname(host);
  
  ArduinoOTA.onStart([]() { // switch off all the PWMs during upgrade
                          sendMsg(groupId, "OTA START");
                          for(int i=0; i<sizeof(dimmer_pin);i++){
                            digitalWrite(dimmer_pin[i], HIGH);
                            delay(250);
                          }
                    });

  ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
                          sendMsg(groupId, "OTA END");
                          for (int i=0;i<30;i++)
                          {
                            digitalWrite(dimmer_pin[i], LOW);
                            delay(250);
                          }
                        });

   ArduinoOTA.onError([](ota_error_t error) {
                        sendMsg(groupId, "OTA ERROR!!!");
                          ESP.restart(); 
                        });
                        
   /* setup the OTA server */
   ArduinoOTA.begin();

  }

void alarma(){
  for(int i = 0;i < 10; i++){
      sendMsg(groupId, "ALARMA");
    }
  }



int alarmPin = D3;
boolean alarm = true;

void setup() {
  Serial.begin(115200);

  pinMode(alarmPin, INPUT);
  
  //Turn off leds by default.
  pinMode(led_pin1, OUTPUT);
  digitalWrite(led_pin1, LOW);
  pinMode(led_pin2, OUTPUT);
  digitalWrite(led_pin2, LOW);
  pinMode(led_pin3, OUTPUT);
  digitalWrite(led_pin3, LOW);

  //connect to wifi
  connectWifi();

  //setup OTA
  setupOTA();

    
}

int val= 0;

void loop() {
  if(WiFi.status() != WL_CONNECTED){
   connectWifi(); 
  }

  /*
  *Alarm section
  */

  val = digitalRead(alarmPin);
  if(val != 1){
    alarm = false;  
  }
  if(alarm == false){
    alarma();
    }
  
  /*
  *alarm end
  */
  ArduinoOTA.handle();
  
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
