#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

/**
 * Handle web server
 */
void handleWebServer(){
  server.handleClient();
}

/**
 * Setup server configuration
 */
void setupWebServer(){
  server.on("/", handleLogin);
  server.on("/users", handleUsers);
  server.onNotFound(handleNotFound);
  
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent","Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize );
  server.begin();
  Serial.println("HTTP server started");
}

/**
 * Verify user authentication
 */
bool is_authentified(){
  Serial.println("Enter is_authentified");
  if (server.hasHeader("Cookie")){   
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  return false;  
}

/**
 * In charge to handle users url
 */
void handleUsers(){
  String header;
  
  if (!is_authentified()){
    String header = "HTTP/1.1 301 OK\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }
  
  String content = "<html><body>\n";
   content += "<h1>Telegram Users</h1>";
   content += "<ul>";
   //load contacts
   for(int i = 0 ; i< contactMaxNum; i++){
     content += "<li>" + contacts[i][0] + " : " + contacts[i][1] + "</li>";
   }
   content += "</ul>";
   content += "You can access this page until you <a href=\"/?DISCONNECT=YES\">disconnect</a></body></html>";
   server.send(200, "text/html", content);
}

//login page, also called for disconnect
void handleLogin(){
  String msg;
  if (server.hasHeader("Cookie")){   
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")){
    Serial.println("Disconnection");
    String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ESPSESSIONID=0\r\nLocation: /\r\nCache-Control: no-cache\r\n\r\n";
    server.sendContent(header);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")){
    if (server.arg("USERNAME") == "admin" &&  server.arg("PASSWORD") == "admin" ){
      String header = "HTTP/1.1 301 OK\r\nSet-Cookie: ESPSESSIONID=1\r\nLocation: /users\r\nCache-Control: no-cache\r\n\r\n";
      server.sendContent(header);
      Serial.println("Log in Successful");
      return;
    }
  msg = "Wrong username/password! try again.";
  Serial.println("Log in Failed");
  }
  //form action must point to this same url, so that cookie reading is done properly
  String content = "<html><body><form action='/' method='POST'>To log in, please use : admin/admin<br>";
  content +=       "User:<input type='text' name='USERNAME' placeholder='user name'><br>";
  content +=       "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
  content +=       "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
  content += "</body></html>";
  server.send(200, "text/html", content);
}

/**
 * Handle unexisting URL's
 * show 404 page.
 */
 void handleNotFound(){
  String message = "<html> <body> <center>";
  message += "<h1> Error 404 </h1>";
  message += "<h2> Page Not Found </h2>";
  message += "<p>URI: ";
  message += server.uri();
  message += "</p><p>Method: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\n</p><p>Arguments: ";
  message += server.args();
  message += "\n</p></br></br>";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  message += "</center></body></html>";
  server.send(404, "text/html", message);
  
}
/*
void setup(void){
  Serial.begin(115200);
  

  
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleLogin);
  server.on("/users", handleUsers);
  server.onNotFound(handleNotFound);
  
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent","Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize );
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
*/
