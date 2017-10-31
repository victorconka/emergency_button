void setupWifi(){
if (DEBUG){
Serial.println(F("setupWifi()"));
}
  wifiManager.reset(new WiFiManager);
  //set up ip address of the host we want to Wake on Lan
  setIpAddress(computer_ip);
  UDP.begin(9); 

  //Adding an additional config on the WIFI manager webpage for the bot token
  char* botToken = string2char(telegram_bot_token);
  WiFiManagerParameter custom_bot_id("botid", "TELEGRAM BOT TOKEN", botToken, 50);
  wifiManager->addParameter(&custom_bot_id);
  char* telegramUser = string2char(telegram_contacts[0]); 
  WiFiManagerParameter custom_telegram_user("telegram_user", "TELEGRAM FIRST USER", telegramUser, 21);
  wifiManager->addParameter(&custom_telegram_user);

  //WIFI INITIALIZE WIFI IN AP OR STA MODE
  char* ap = string2char(wifiApName);
  char* pass = string2char(wifiApPassword);
  
  if(!wifiManager->autoConnect(ap, pass)){
    //TELEGRAM
    if (DEBUG){
      Serial.println(F("entered autoconnect"));
    }
    telegram_bot_token = custom_bot_id.getValue();
    ifttt_event_name = custom_telegram_user.getValue();
    if (DEBUG){
      Serial.println(custom_bot_id.getValue());
      Serial.println(custom_telegram_user.getValue());
    }
  } 
}

/**
 * WIFI
 * Reset wifi configuration and reboot
 */
void resetWifiSettings(){
if (DEBUG){
Serial.println(F("resetWifiSettings()"));
}
  wifiManager->resetSettings();
}

/**
 * Transform mac address from it's byte representation
 * into the corresponding String object value.
 */
String macToString(byte mac[6]){
  String ret;
  String s;
  for(int i = 0;i < 6; i++){
    s = String( mac[i], HEX);
    if(s.length() == 1){
      s = "0" + s;    
    }
    ret += s;
    if(i+1<6){
      ret += ":";  
    }
  }
  ret.toUpperCase();
  return ret;
}

/**
 * Convert ONE HEX value in it's String representation
 * into corresponding byte value.
 */
byte hexToByte(String string){
  byte ret;
  char *p = const_cast<char*>(string.c_str());
  ret = (byte) strtol(p,NULL,16);
  return ret;
}

/**
 * Convert String mac address into byte array
 * String macIn - String mac addres separated by ":".
 * byte macOut[6] - output byte array.
 * Returns true if conversion was successfull 
 * returns false if conversion failed
 */
bool macToByteArray(String macIn, byte macOut[6]){
  macIn.replace("0x","");
  if(macIn != ""){
    int index;
    String s;
    int b;
    for(int i = 0;i < 6; i++){
        index = macIn.indexOf(":");
        if((index < 3 && index > 0 ) && (i >= 0 && i <5) || (index == -1 && i == 5) ){
          if(i+1 < 6){
            b = hexToByte(macIn.substring(0,index));
          }else{
            b = hexToByte(macIn);
          }
          macOut[i] = (byte)b;
          macIn.remove(0,index+1);
        }else{
          return false;
        }
    }
    return true;
  }
  return false;
}

/**
 * Ping programmed ip address
 */
bool pingIp(){
  return Ping.ping(_ip,2); 
}

/**
 * Send Wake on Lan packet to wake up the configured pc
 */
void sendWOLPacket(){
 WakeOnLan::sendWOL(_ip, UDP, wol_mac, sizeof wol_mac);
 WakeOnLan::sendWOL(_ip, UDP, wol_mac, sizeof wol_mac);
 WakeOnLan::sendWOL(_ip, UDP, wol_mac, sizeof wol_mac);
}


