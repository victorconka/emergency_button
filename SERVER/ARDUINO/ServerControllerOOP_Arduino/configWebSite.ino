class configWebSite{
/**
 * WEB SERVER
 */
std::unique_ptr<ESP8266WebServer> server; //pointer to server object
String header;      //header for web page
String header0;     //header for web page
String header1;     //header for web page
String error_404;   //error message for web page
String login;       //login website for web page
String wrong_user;  //wrong user for web page

  
/**
 * Handle web server
 */
void handleWebServer(){
  server->handleClient();
}

/**
 * Setup server configuration
 */
void setupWebServer(){
  server.reset(new ESP8266WebServer(80));
  if (DEBUG){
    Serial.println(F("setupWebServer()")); 
  }
  server->on("/", handleLogin);
  server->on("/users", handleUsers);
  server->onNotFound(handleNotFound);
  
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent","Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  //ask server to track these headers
  server->collectHeaders(headerkeys, headerkeyssize );
  server->begin();
  Serial.println(F("HTTP server started"));
}

/**
 * Verify user authentication
 */
bool is_authentified(){
  Serial.println(F("Enter is_authentified()"));
  if (server->hasHeader(F("Cookie"))){   
    Serial.print(F("Found cookie: "));
    String cookie = server->header(F("Cookie"));
    Serial.println(cookie);
    if (cookie.indexOf(F("ESPSESSIONID=1")) != -1) {
      Serial.println(F("Authentification Successful"));
      return true;
    }
  }
  Serial.println(F("Authentification Failed"));
  return false;  
}

/**
 * In charge to handle users url
 */
void handleUsers(){
  if (!is_authentified()){
    server->sendContent(header);
    return;
  }else{
    String userPage = readJsonFile(F("users"));
    String read_mode = F("readonly");
    String read_mode_debug = F("disabled");
    String button =  readJsonFile(F("edit_button"));
    String temp_value;
    
    if(server->hasArg(F("submit"))){
      if(server->arg(F("submit")) == F("EDIT")){
        read_mode = F("");
        read_mode_debug = F("");
        button =  readJsonFile(F("cancel_save_button"));
      }else if(server->arg(F("submit")) == "CANCEL"){
        Serial.println(F("CANCEL pressed"));
      }else if(server->arg(F("submit")) == "SAVE"){
        Serial.println(F("SAVE pressed"));
        DEBUG = server->hasArg(F("DEBUG"));
        computer_ip = server->arg(F("computer_ip"));
        setWolMac(server->arg(F("wol_mac")));
        wifiApName = server->arg(F("wifiApName"));
        wifiApPassword = server->arg(F("wifiApPassword"));
        ifttt_key = server->arg(F("ifttt_key"));
        ifttt_event_name = server->arg(F("ifttt_event_name"));
        telegram_bot_token = server->arg(F("telegram_bot_token"));
        telegram_group = server->arg(F("telegram_group"));
        for(int i = 0; i < 5; i++){
          temp_value =  F("contact");
          temp_value += i+1;
          temp_value = server->arg(temp_value);
          setTelegramContact(i+1,temp_value);
        }
      }
    }
    
    userPage.replace(F("button_place"),button);
    userPage.replace(F("r_d"),read_mode_debug);//r_d - read_mode_debug
    userPage.replace(F("r_m"),read_mode);      //r_m - read_mode
    //TELEGRAM_CONTACTS
    for(uint8_t i = 0 ; i< numContacts; i++){
      temp_value =  F("value_contact_");
      temp_value += i+1;
      userPage.replace(temp_value,telegram_contacts[i]);
    }
    //DEBUG MODE
    temp_value = F("");
    if(DEBUG){
      temp_value = F("checked");
    }
    userPage.replace(F("DEBUG_state"),temp_value);
    userPage.replace(F("computer_ip_value"),computer_ip);
    userPage.replace(F("wol_mac_value"),macToString(wol_mac));  
    userPage.replace(F("wifiApName_value"),wifiApName);
    userPage.replace(F("wifiApPassword_value"),wifiApPassword);
    userPage.replace(F("ifttt_key_value"),ifttt_key);
    userPage.replace(F("ifttt_event_name_value"),ifttt_event_name);
    userPage.replace(F("telegram_bot_token_value"),telegram_bot_token);
    userPage.replace(F("telegram_group_value"),telegram_group);

    server->send(200, F("text/html"), userPage);
  }
}

//login page, also called for disconnect
void handleLogin(){
  String login_res = login;
  String msg = "";
  if (server->hasHeader(F("Cookie"))){   
    Serial.print(F("Found cookie: "));
    String cookie = server->header(F("Cookie"));
    Serial.println(cookie);
  }
  if (server->hasArg(F("DISCONNECT"))){
    Serial.println(F("Disconnection"));
    server->sendContent(header0);
    return;
  }
  if (server->hasArg(F("USERNAME")) && server->hasArg(F("PASSWORD"))){
    if (server->arg(F("USERNAME")) == F("admin") &&  server->arg(F("PASSWORD")) == F("admin") ){
      server->sendContent(header1);
      Serial.println(F("Log in Successful"));
      return;
    }
  msg = wrong_user;
  Serial.println(F("Log in Failed"));
  }
  //form action must point to this same url, so that cookie reading is done properly
  login_res.replace(F("msg"),msg);
  server->send(200, F("text/html"), login_res);
}

/**
 * Handle unexisting URL's
 * show 404 page.
 */
void handleNotFound(){
  server->send(404, F("text/html"), error_404);
}
};
