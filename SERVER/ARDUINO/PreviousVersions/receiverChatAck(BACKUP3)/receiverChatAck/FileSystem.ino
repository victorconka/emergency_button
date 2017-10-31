#include "FS.h"
#include <ArduinoJson.h>


const int contactMaxNum = 5;    //max number of contacts in the list
String contacts[contactMaxNum][2]; //stores telegram contacts

void setupFileSystem(){
  Serial.println("Mounting FS...");
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  if (!loadConfig()) {
    Serial.println("Failed to load config");
  } else {
    Serial.println("Config loaded");
  }  
}

bool loadConfig() {
  File configFile = SPIFFS.open("/config1.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }
 
  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);
  
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());
  
  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }

  //read data from the memory
  int i = 0;
  for(auto it : json){
    
      Serial.print(it.key);
      Serial.print(" : ");
      Serial.println(it.value.as<String>());
      
      contacts[i][0] = it.key;
      contacts[i][1] = it.value.as<String>();
      i++;
    
  }
  
  return true;
}

bool saveConfig() { 
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  
  File configFile = SPIFFS.open("/config1.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }
  
  for(int i = 0 ; i< contactMaxNum; i++){
      json.set(contacts[i][0],contacts[i][1]);
  }

  json.printTo(configFile);
  
  return true;
}
