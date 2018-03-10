/**
 * TCP CLEANUP
 * cleans tcp connection heap memory so that esp module
 * doesn't reboot when out of memory.
 * thankx to @psy0rz for this marvelous piece of code
 * https://github.com/esp8266/Arduino/issues/1923
 */
 
struct tcp_pcb;
extern struct tcp_pcb* tcp_tw_pcbs;
extern "C" void tcp_abort (struct tcp_pcb* pcb);

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
    ESP.wdtFeed();
    ret = bot->sendMessage(chat_id, message);
    //tcpCleanup();//THIS IS SUPER IMPORTANT. SOLVES MEMORY LEAK
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
bool saveConfiguration = false;
bool previosSettingsMode = SETTINGS_MODE;
    if( text.indexOf(F("/set_telegram_group")) == 0 ){
        text = clearString(F("/set_telegram_group"), text);
        saveConfiguration = setTelegramGroup(text);
    }
    else if( text.indexOf(F("/set_contact")) == 0 ){
        text = clearString(F("/set_contact"), text);
        saveConfiguration = setTelegramContact(text);
    }
    else if( text.indexOf(F("/set_telegram_token")) == 0 ){
        text = clearString(F("/set_telegram_token"), text);
        saveConfiguration = setTelegramBotToken(text);
    }
    else if( text.indexOf(F("/set_ifttt_key")) == 0 ){
        text = clearString(F("/set_ifttt_key"), text);
        saveConfiguration = setIftttKey(text);
    }
    else if( text.indexOf(F("/set_ifttt_event_name")) == 0 ){
        text = clearString(F("/set_ifttt_event_name"), text);
        saveConfiguration = setIftttEventName(text);
    }
    else if( text.indexOf(F("/set_wifi_ap_name")) == 0 ){
        text = clearString(F("/set_wifi_ap_name"), text);
        saveConfiguration = setWifiApName(text);
    }
    else if( text.indexOf(F("/set_wifi_ap_password")) == 0 ){
        text = clearString(F("/set_wifi_ap_password"), text);
        saveConfiguration = setWifiApPassword(text);
    }
    else if( text.indexOf(F("/set_debug")) == 0 ){
        text = clearString(F("/set_debug"), text);   
        saveConfiguration = setDebug(text);
    }
   else if( text.indexOf(F("/set_settings_mode")) == 0 ){
        text = clearString(F("/set_settings_mode"), text);
        saveConfiguration = setSettingsMode(text);
    }
    else if( text.indexOf(F("/set_mac")) == 0 ){
        text = clearString(F("/set_mac"), text);
        saveConfiguration = setWolMac(text);
        if(!saveConfiguration){
          sendMsg(chat_id, F("Problem with provided MAC ADDRESS, maybe incorrect format"));
        }
    }
    else if( text.indexOf(F("/set_ip")) == 0 ){
        text = clearString(F("/set_ip"), text);  
        saveConfiguration = setIpAddress(text);
        if(!saveConfiguration){
          sendMsg(chat_id, F("Problem with provided IP ADDRESS, maybe incorrect format"));
        }
    }
    else if (text == F("/resetWifiSettings")) {
      //remove message before resetting configuration.
        numNewMessages = bot->getUpdates(bot->last_message_received + 1);
        resetWifiSettings();
    }
    else if (text == F("/show_settings")) {
        settingsToString();
        sendMsg(chat_id, settings);
        settings ="";
    }
    else if (text =="/wol"){
        sendWOLPacket();
        sendMsg(chat_id, "Se ha enviado un paquete WOL");
    }
    
    else if (text == "/ping"){
        if ( pingIp() ) {
          sendMsg(chat_id,"Computer is ONLINE");
        } else {
          sendMsg(chat_id,"Computer is OFFLINE");
      }
    } 
    //must be last. Executes action based on the command
    if (DEBUG){
      Serial.print("saveConfiguration: ");
      Serial.println(saveConfiguration);
    }
    if(saveConfiguration == true){
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
void listenTelegram(){
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    numNewMessages = bot->getUpdates(bot->last_message_received + 1);
    //handle telegram messages
    while(numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot->getUpdates(bot->last_message_received + 1);
      tcpCleanup();  //THIS IS SUPER IMPORTANT. SOLVES MEMORY LEAK
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
