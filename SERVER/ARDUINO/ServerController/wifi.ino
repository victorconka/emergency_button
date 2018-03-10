void setupWifi(){
  //set up ip address of the host we want to Wake on Lan
  setIpAddress(computer_ip);
  UDP.begin(9); 

  //Adding an additional config on the WIFI manager webpage for the bot token
  const char* botToken = telegram_bot_token.c_str();
  WiFiManagerParameter custom_bot_id("botid", "TELEGRAM BOT TOKEN", botToken, 50);
  wifiManager.addParameter(&custom_bot_id);
  
  const char* telegramUser = telegram_contacts[0].c_str();
  WiFiManagerParameter custom_telegram_user("telegram_user", "TELEGRAM FIRST USER", telegramUser, 21);
  wifiManager.addParameter(&custom_telegram_user);
  
  if(!wifiManager.autoConnect(wifiApName.c_str(), wifiApPassword.c_str())){
      //Serial.println(F("entered autoconnect"));
  }else{
    String recovered_bot_id(custom_bot_id.getValue());
    String recovered_telegram_user(custom_telegram_user.getValue());
    bool save = false;
    
    if(recovered_bot_id != telegram_bot_token){
      telegram_bot_token = recovered_bot_id;
      save = true;
    }
    if(recovered_telegram_user != telegram_contacts[0]){
      telegram_contacts[0] = recovered_telegram_user;
      save = true;
    }
    if(save){
      saveConfig();
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
  wifiManager.resetSettings();
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
 * packet is sent to 255,255,255,255, that way router will directly send WOL packet
 * without checking whom does it belong to and avoid unsent wol packets (broadcast).
 */
void sendWOLPacket(){
 IPAddress wol_ip(255,255,255,255);
 WakeOnLan::sendWOL(wol_ip, UDP, wol_mac, sizeof wol_mac);
}
