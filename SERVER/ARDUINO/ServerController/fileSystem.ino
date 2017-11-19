/**
 * Initialize spiffs file system and load
 * stored data.
 */
void setupFileSystem(){
  if (DEBUG){
    Serial.println(F("setupFileSystem()"));
  }
  if (!SPIFFS.begin()) {
    if (DEBUG){
      Serial.println(F("Failed to mount file system"));
    }
    return;
  }
  if (!loadConfig()) {
    if (DEBUG){
      Serial.println(F("Failed to load config"));
    }
  } else {
    if (DEBUG){
      Serial.println(F("Config loaded"));
    }
  }  
}

/**
 * Writes settings to the memory as
 * a json file.
 */
bool saveConfig() { 
if (DEBUG){
Serial.println(F("saveConfig()"));
}
// must be dynamic buffer, eslewise crashes
  DynamicJsonBuffer jsonBuffer(bufferSize);
  JsonObject& json = jsonBuffer.createObject();
  File configFile = SPIFFS.open("/config.json", "w"); 
  if (!configFile) {
      if (DEBUG){
      Serial.println(F("Failed to open config file for writing"));
      }
    return false;
  }
  //create array and write data
  JsonArray& json_contacts = json.createNestedArray("telegram_contacts");    
  String contact;
  for(uint8_t i = 0 ; i < numContacts; i++){
    contact = telegram_contacts[i];
    if(contact != "" && contact != "null" && contact != NULL){
      json_contacts.add(telegram_contacts[i]);
    }else{
      json_contacts.add(EMPTY);    
    }
  }   
  json.set(F("computer_ip"),computer_ip);
  json.set(F("wol_mac"),macToString(wol_mac)); 
  json.set(F("telegram_group"),telegram_group);
  json.set(F("telegram_bot_token"),telegram_bot_token);
  json.set(F("ifttt_key"),ifttt_key);
  json.set(F("ifttt_event_name"),ifttt_event_name);
  json.set(F("wifiApName"),wifiApName);
  json.set(F("wifiApPassword"),wifiApPassword);
  json.set(F("DEBUG"),DEBUG);
  json.set(F("ALERT"),ALERT);
  json.set(F("SETTINGS_MODE"),SETTINGS_MODE);
  
if (DEBUG){
Serial.println(F("saveConfig() fisnish"));
}
  //write json object to the memory
  json.printTo(configFile);
  
  return true;
}

/**
 * Once spiffs file system is set up we need to
 * read data from the memory
 */
bool loadConfig() {
if (DEBUG){
Serial.println(F("loadConfig()"));
}
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
if (DEBUG){
Serial.println(F("Failed to open config file"));
}
    return false;
  }
  size_t size = configFile.size();
  if (size > bufferSize) {
if (DEBUG){
Serial.println(F("Config file size is too large"));
}
    return false;
  }
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);
if (DEBUG){
Serial.println(buf.get());
}
  DynamicJsonBuffer jsonBuffer(bufferSize);
  JsonObject& json = jsonBuffer.parseObject(buf.get());
  if (!json.success()) {
if (DEBUG){
Serial.println(F("Failed to parse config file"));
}
    return false;
  }else{
    //RECOVER VALUES FROM MEMORY
if (DEBUG){
Serial.println(F("Success parse config file. recovering values."));
}
    String mac = json["wol_mac"].as<String>();
    bool macBool = setWolMac(mac);
if (DEBUG && !macBool){
Serial.println(F("MAC Set ERROR"));
}
    SETTINGS_MODE = json["SETTINGS_MODE"].as<bool>();
    computer_ip = json["computer_ip"].as<String>();
    wifiApName = json["wifiApName"].as<String>();
    wifiApPassword = json["wifiApPassword"].as<String>();
    telegram_group = json["telegram_group"].as<String>();
    telegram_bot_token = json["telegram_bot_token"].as<String>();
    ifttt_key = json["ifttt_key"].as<String>();
    ifttt_event_name = json["ifttt_event_name"].as<String>();
    DEBUG = json["DEBUG"].as<bool>();
    ALERT = json["ALERT"].as<bool>();
    
    String contact;
    for(uint8_t i = 0;i < numContacts; i++){
      contact = json["telegram_contacts"][i].as<String>();
      contact = clearString("",contact);   
      if(contact != "" && contact != "null" && contact != NULL){
        telegram_contacts[i] = contact;
      }else{
        telegram_contacts[i] = EMPTY;  
      }  
    }    
if (DEBUG){
Serial.println(F("finished recovering json into variables"));
}    
  } 
  jsonBuffer.clear();
  configFile.close();
  
  if(SETTINGS_MODE){
    loadHtmlFiles();
  }
  
  return true;
}

/**
 * List of all the html files read from the internal memory
 * this way if modified, because some data was changed, 
 * we can reload it.
 */
void loadHtmlFiles(){
  if(DEBUG){
    Serial.println(F("loadHtmlFiles()"));
  }
  header = readJsonFile(F("header"));
  header0 = readJsonFile(F("header0"));
  header1 = readJsonFile(F("header1"));
  error_404 = readJsonFile(F("error_404"));
  login = readJsonFile(F("login"));
  wrong_user = readJsonFile(F("wrong_user"));  
}

/**
 * Once spiffs file system is set up we need to
 * read data from the memory
 */
String readJsonFile(String string) {
    String ret;
if (DEBUG){
Serial.print(F("readJsonFile("));
Serial.print(string);
Serial.println(F(")"));
}
    String filename = "/" + string;
    File configFile = SPIFFS.open(filename, "r");
  
    if (!configFile) {
if (DEBUG){
Serial.println(F("Failed to open file"));
}
      return "";
    }else{   
    size_t size = configFile.size();
if (DEBUG){
Serial.print("HTML file size: ");
Serial.println(size);
}
    if (size > bufferSize) {
if (DEBUG){
Serial.println(F("File size is too large"));
}
      return "";
    }
    // Allocate a buffer to store contents of the file.
    std::unique_ptr<char[]> buf(new char[size]);
    configFile.readBytes(buf.get(), size);
    //StaticJsonBuffer<bufferSize> jsonBuffer;
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& json = jsonBuffer.parseObject(buf.get());
    
    if (!json.success()) {
if (DEBUG){
Serial.println(F("Failed to parse config file"));
}
    return "";
    }else{  
if (DEBUG){
Serial.println(F("Success parse config file. recovering values."));
}     
    ret = json[string].as<String>(); 
if (DEBUG){
Serial.println(F("finished recovering json into variables"));
}    
    } 
    jsonBuffer.clear();
    configFile.close();
    return ret;
  }
}


/**
 * Turn on/off serial debug messages
 * String text - value received from telegram. possible values ['0','1'], 0 being false and 1 - true.
 */
bool setDebug(String text){
char debug = text.charAt(0);
if (DEBUG){
Serial.println(F("setDebug()"));
}
    if(isDigit(debug)){
      if(debug == '0' || debug == '1' ){//bool is 0 or 1
        DEBUG = (debug == '1');
        return true;
      }
    }else{
if (DEBUG){
Serial.println(F("debug must be a digit 0 or 1"));
}
    }
    return false;
}

/**
 * Turn on/off SETTINGS MODE.
 * In SETTINGS MODE: TELEGRAM and NRF is disabled,
 *                   OTA and (CONFIGURATION WEBSITE) is enabled.
 * String text - value received from telegram and cleared.
 *                     possible values ['0','1'], 
 *                     0 - false (turn off), 
 *                     1 - true (turn on).
 */
bool setSettingsMode(String text){
char settingsMode = text.charAt(0);
if (DEBUG){
Serial.println(F("setSettingsMode()"));
}
    if(isDigit(settingsMode)){
      if(settingsMode == '0' || settingsMode == '1' ){//settingsMode MUST BE 0 or 1
        SETTINGS_MODE = (settingsMode == '1');
        return true;
      }
    }else{
if (DEBUG){
Serial.println(F("settingsMode must be a digit 0 or 1"));
}
    }
    return false;
}

/**
 * Set password to to the Access Point that is initialized if
 * wifi configuration isn't recovered from EEPROM
 * String wifiPassword - AP password
 */
bool setWifiApPassword(String wifiPassword){
if (DEBUG){
Serial.println(F("setWifiPassword()"));
}
  if(wifiPassword != ""){
    wifiApPassword = wifiPassword;
    return true;
  }
  return false;
}

/**
 * Set (SSID) name to the Access Point that is initialized if
 * wifi configuration isn't recovered from EEPROM
 * String apName - AP SSID
 */
bool setWifiApName(String apName){
if (DEBUG){
Serial.println(F("setWifiName()"));
}
  if(apName != ""){
    wifiApName = apName;
    return true;
  }
  return false;
}

/**
 * Set event name for the ifttt configured plugin.
 * String eventName - event name
 */
bool setIftttEventName(String eventName){
if (DEBUG){
Serial.println(F("setIftttEventName()"));
}
  if(eventName != ""){
    ifttt_event_name = eventName;
    return true;
  }
  return false;
}

/**
 * Set key for the ifttt configured plugin.
 * String key - key
 */
bool setIftttKey(String key){
if (DEBUG){
Serial.println(F("setIftttKey()"));
}
  if(key != ""){
    ifttt_key = key;
    return true;
  }
  return false;
}

/**
 * Set token for the telegram bot
 * String key - key
 */
bool setTelegramBotToken(String botToken){
if (DEBUG){
Serial.println(F("setTelegramBotToken()"));
}
  if(botToken != ""){
    telegram_bot_token = botToken;
    return true;
  }
  return false;
}

/**
 * Set a telegram contact id
 * String contact - telegram id of the contact in format: "contactN+contactID".
 */
bool setTelegramContact(String contact){
  char contactN = contact.charAt(0);
  contact.remove(0,1);

if (DEBUG){
Serial.print(F("setTelegramContact() "));
Serial.print(contactN);
Serial.print(" ");
Serial.println(contact);
}
    if(contactN > 0){
      if(!containsUser(contact)){
        contactN -=1;
        if(contactN  < 5){//n is inside the necessary range
          if(contact == EMPTY || contact == "" || contact == NULL){
            telegram_contacts[contactN] = EMPTY;
          }else{
            telegram_contacts[contactN] = contact;
          }
          return true;
        }
      }else{
        if (DEBUG){
        Serial.println(F("Contact already present"));  
        }
      }
    }else{
if (DEBUG){
Serial.println(F("Contact number <= 0"));
}  
    }
    return false;
}

/**
 * Set telegram group where bot and users are in
 * String group - id of the telegram group
 */
bool setTelegramGroup(String group){
if (DEBUG){
Serial.println(F("setTelegramGroup()"));
}
  if(group != ""){
    telegram_group = group;
    return true;
  }
  return false;
}

/**
 * Set wol mac
 */
bool setWolMac(String stringMac){
if (DEBUG){
Serial.println(F("setWolMac()"));
}
  byte mac[6];
  bool ret = macToByteArray(stringMac, mac);
  if(ret){
    memcpy(wol_mac, mac, 6);
  }
  return ret;
}

/*
 * Checks if a user is authorized
 * String user_id - user id.
 */
bool containsUser(String user_id){
if (DEBUG){
Serial.print(F("containsUser() "));
Serial.println(">" + user_id + "<");
}
  if(user_id ==  EMPTY || user_id == ""){
    return false;
  }
  if(user_id == telegram_group){
    return true;
  }
  for(uint8_t i = 0; i < 5; i++){
     if(user_id == telegram_contacts[i]){
        return true;
     }
  }
    return false;
}

/**
 * Set up new ip address
 * if new ip address is not set, computer_ip will be set.
 * if computer_ip not set, default_ip is set.
 * String ip - new ip address.
 */
bool setIpAddress(String ip){
   if(_ip.fromString(ip)){
      computer_ip = ip;
      return true;
    }else{
      if(!_ip.fromString(computer_ip)){
        _ip.fromString(default_ip);
        computer_ip = default_ip;
      }
      return false;  
    }
}

/**
 * Returns a String containing the most important settings
 * 
 */
void settingsToString(){
if (DEBUG){
Serial.println(F("settingsToString()"));
}
  settings = F("DEBUG: ");
  settings += DEBUG;
  settings += F("\nSETTINGS_MODE: ");
  settings += SETTINGS_MODE;
  settings += F("\n   TELEGRAM");
  settings += F("\n   Bot Token: ");
  settings += telegram_bot_token;
  settings += F("\n  Contacts: \n");
  for(uint8_t i = 0; i< numContacts; i++){
     settings += F("   ");
     settings += String((i+1)) + F(". ");
     settings += telegram_contacts[i];
     settings += F("\n");
  }
  settings += F("  Group id: ");
  settings += telegram_group;
  settings += F("\n IFTTT \nKEY: ");
  settings += ifttt_key;
  settings += F("\nEVENT NAME: ");
  settings += ifttt_event_name;
  settings += F("\n  WIFI Access Point\n");
  settings += F("ApName: ");
  settings += wifiApName;
  settings += F("\nPassword: ");
  settings += wifiApPassword;
  settings += F("\n  Wake On Lan");
  settings += F("\nMAC: ");
  settings += macToString(wol_mac);
  settings += F("\nComputer IP: ");
  settings += computer_ip;

}
