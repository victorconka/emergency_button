
class telegram{
/**
 * TELEGRAM VARIABLES
 */
WiFiClientSecure client;                   //client used by ifttt and telegram
std::unique_ptr<UniversalTelegramBot> bot; // pointer to the bot
const uint8_t numContacts = 5;             //number of telegram contacts stored
String EMPTY = "empty";                    //value used when null, deleted, etc...
String telegram_contacts[numContacts];     //telegram contact container.
String telegram_group;                     //telegram group id
String telegram_bot_token;                 //telegram bot token
int Bot_mtbs = 1000;                       //mean time between scan messages
long Bot_lasttime;                         //last time messages' scan has been done


 /**
 * TCP CLEANUP
 * cleans tcp connection heap memory so that esp module
 * doesn't reboot when out of memory.
 * thankx to @psy0rz for this marvelous piece of code
 * https://github.com/esp8266/Arduino/issues/1923
 */ 
 void tcpCleanup()
{
  while(tcp_tw_pcbs!=NULL)
  {
    tcp_abort(tcp_tw_pcbs);
  }
}

/*
 * TELEGRAM
 * Send a message to a chat
 * returns true if message was sent
 * returns false if message was not sent
 */
bool sendMsg(String chat_id, String message){
if (DEBUG){
Serial.println(F("sendMsg()"));
Serial.print(F("message to: "));
Serial.println(chat_id);
Serial.print(F("ESP.heapSize(): "));
Serial.println(ESP.getFreeHeap());
}
    bool ret = false;
    bot->sendChatAction(chat_id, "typing");
    ret = bot->sendMessage(chat_id, message);
    tcpCleanup();//THIS IS SUPER IMPORTANT. SOLVES MEMORY LEAK
    return ret;    
}

/**
 * TELEGRAM
 * user is not authorized message to execute any action
 */
void noAutorizado(String chat_id, String from_name){
if (DEBUG){
Serial.println(F("noAutorizado()"));
Serial.print(F("message to: "));
Serial.println(chat_id);
}
    sendMsg(chat_id, from_name + F("YOU ARE NOT AUTHORIZED"));
    sendMsg(telegram_group, "Unauthorized access from: " + from_name + "with id " + chat_id);
}

/**
 * Format string removing the part of the command from it's content 
 * String command - command recieved from telegram
 * String content - content we want to clear
 */
String clearString(String command, String content){
if (DEBUG){
  Serial.println(F("clearString()"));
  Serial.print(F("Command: "));
  Serial.println(command);
  Serial.print(F("String: "));
  Serial.println(content);
}
  content.replace(command,F(""));
  content.replace(F("\t"),F(""));
  content.replace(F("\n"),F(""));
  content.replace(F(";") ,F(""));
  content.replace(F(" ") ,F(""));
if (DEBUG){
  Serial.print(F("cleared: "));
  Serial.println(content);
}
  return content;
}

/**
 * TELEGRAM
 * actions based on telegram commands
 */
void actions(String text, String chat_id){
if (DEBUG){
  Serial.println(F("actions()"));
  Serial.print(F("text: "));
  Serial.println(text);
  Serial.print(F("chat_id: "));
  Serial.println(chat_id);
}
bool result = false;
bool previosSettingsMode = SETTINGS_MODE;
    if( text.indexOf(F("/set_telegram_group")) == 0 ){
        text = clearString(F("/set_telegram_group"), text);
        result = setTelegramGroup(text);
    }
    else if( text.indexOf(F("/set_contact")) == 0 ){
        text = clearString(F("/set_contact"), text);
        char contactN = text.charAt(0);
          if (DEBUG){
            Serial.print(F("cleared char: "));
            Serial.println(contactN);
          }
        text.remove(0,1);
        result = setTelegramContact(contactN,text);
    }
    else if( text.indexOf(F("/set_ifttt_key")) == 0 ){
        text = clearString(F("/set_ifttt_key"), text);
        result = setIftttKey(text);
    }
    else if( text.indexOf(F("/set_ifttt_event_name")) == 0 ){
        text = clearString(F("/set_ifttt_event_name"), text);
        result = setIftttEventName(text);
    }
    else if( text.indexOf(F("/set_wifi_ap_name")) == 0 ){
        text = clearString(F("/set_wifi_ap_name"), text);
        result = setWifiApName(text);
    }
    else if( text.indexOf(F("/set_wifi_ap_password")) == 0 ){
        text = clearString(F("/set_wifi_ap_password"), text);
        result = setWifiApPassword(text);
    }
    else if( text.indexOf(F("/set_debug")) == 0 ){
        text = clearString(F("/set_debug"), text);
        char debug = text.charAt(0);
        result = setDebug(debug);
    }
   else if( text.indexOf(F("/set_settings_mode")) == 0 ){
        text = clearString(F("/set_settings_mode"), text);
        char settings_mode = text.charAt(0);
        result = setSettingsMode(settings_mode);
    }
    else if( text.indexOf(F("/set_mac")) == 0 ){
        text = clearString(F("/set_mac"), text);
        result = setWolMac(text);
        if(!result){
          sendMsg(chat_id, F("Problem with provided mac, maybe incorrect format"));
        }
    }
    else if( text.indexOf(F("/set_ip")) == 0 ){
        text = clearString(F("/set_ip"), text);  
        IPAddress _ip;
        result = setIpAddress(text);
        if(!result){
          sendMsg(chat_id, F("Problem with provided mac, maybe incorrect format"));
        }
    }
    else if (text == F("/show_settings")) {
        settingsToString();
        sendMsg(chat_id, settings);
        settings ="";
    }
    else if (text =="/wol"){
        sendWOLPacket();
        sendMsg(chat_id, "Se ha enviado un paquete magico");
    }
    
    else if (text == "/ping"){
        if ( pingIp() ) {
          sendMsg(chat_id, "Success pinging");
        } else {
          sendMsg(chat_id,"Error :(,  pinging");
      }
    } 
    //must be last. Executes action based on the command
    if(result == true){
      saveConfig();
      //TOGGLE SETTINGS MODE BACK
      SETTINGS_MODE = previosSettingsMode;
    } 
}

/**
 * TELEGRAM
 * handle messages and execute corresponding actions
 */
void handleNewMessages(int numNewMessages) {
if (DEBUG){
  Serial.println(F("handleNewMessages()"));
}
  String chat_id;
  String text;
  String from_name;

  for (uint8_t i=0; i<numNewMessages; i++) {
    chat_id = String(bot->messages[i].chat_id);
    text = bot->messages[i].text;
    from_name = bot->messages[i].from_name;
    
    if (from_name == "") from_name = "Guest";
    
    text.remove(text.indexOf('@'), text.length()-1);
    
    if(containsUser(chat_id)){
      actions(text, chat_id);
    }else{
      noAutorizado(chat_id, from_name);
    }    
  }
}


/**
 * TELEGRAM
 * listen for new telegram messages
 */
uint8_t numNewMessages;
void listenTelegram(){
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    numNewMessages = bot->getUpdates(bot->last_message_received + 1);
    //handle telegram messages
    while(numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot->getUpdates(bot->last_message_received + 1);
    }
    Bot_lasttime = millis();
  }  
}

/**
 * TELEGRAM
 * Initialize telegram.
 */
void setupTelegram(){
if (DEBUG){
  Serial.println(F("setupTelegram()"));
}
  bot.reset(new UniversalTelegramBot(telegram_bot_token, client));
  
  String message = "Online with IP: ";
  message += WiFi.localIP().toString();
  sendMsg(telegram_group, message);
}

bool contactPerson(uint8_t contact_number){
if (DEBUG){
  Serial.print(F("contactPerson() -> Nº= "));
  Serial.println(contact_number);
}
  if (contact_number > 0 && contact_number < 6){
    contact_number-=1;
    if(containsUser(telegram_contacts[contact_number])){
        return(sendMsg(telegram_contacts[contact_number], F("Please contact me")));
    }
  }
  return false;
}
};
