#include <UniversalTelegramBot.h> // Telegram API
#include <ESP8266Ping.h>          // PIGN    ping to check if PC is ON


/**
 * TELEGRAM STUFF
 */
//WiFiClientSecure client;  //CREATED BY IFTTT PREVIOUSLY
UniversalTelegramBot bot(BOTtoken, client);
int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done

/**
 * TELEGRAM
 * Sends the alert message to all the users set up in the userList
 */
void alertTelegram(String alertMsg ){
  String viktest= "287078218";
  //sendMsg(groupId,alertMsg);
  sendMsg(viktest,alertMsg);
    
}

/*
 * TELAGRAM
 * Checks if a user is authorized
 */
boolean containUser(String user_id){
  for(int i = 0; i < sizeof(users); i++){
      if( user_id.equals(users[i])) {
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
    //bot.sendChatAction(chat_id, "typing");
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

    /*
    else if (text =="/wol"){
        sendWOLPacket();
        sendMsg(chat_id, "Se ha enviado un paquete magico");
    }*/
/*
    else if (text == "/ping"){
        if ( pingIp() ) {
          sendMsg(chat_id, "Success pinging");
        } else {
          sendMsg(chat_id,"Error :(,  pinging");
      }
    } 
    */
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
String chat_id;
String text;
String from_name;

void handleNewMessages(int numNewMessages) {

  for (int i=0; i<numNewMessages; i++) {
    /*
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;
    */
    chat_id = String(bot.messages[i].chat_id);
    text = bot.messages[i].text;
    from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";
    
    Serial.println(from_name);
    text.remove(text.indexOf('@'), text.length()-1);
    Serial.println(text);
    
    if(containUser(chat_id)){
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
 int numNewMessages;
 void listenTelegram(){
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    //handle telegram messages
    while(numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    Bot_lasttime = millis();
  }  
}
