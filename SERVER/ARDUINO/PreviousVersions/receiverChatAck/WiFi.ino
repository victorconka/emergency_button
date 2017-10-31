//#include <FS.h>                 // WIFI    this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>          // WIFI    https://github.com/esp8266/Arduino
#include <DNSServer.h>            // WIFI
#include <WiFiManager.h>          // WIFI     https://github.com/tzapu/WiFiManager
#include <WiFiUDP.h>              // WOL     UDP packet for sending WOL magic packet
#include <WakeOnLan.h>            // WOL     functionality

 /**
 * WAKE ON LAN MUST DEFINE VARIABLES
 */
WiFiUDP UDP;
int dst_port = 9;
int src_port = 9;
//The target IP address to send the magic packet to.
//IPAddress computer_ip(0,0,0,0); //ip used to wake up on lan. 0.0.0.0 used in case ip is unknown
//IPAddress ping_ip; //ip used to ask if it's online
//MAC address of the PC you need to wake up
byte mac[] = {0x1c,0x1b,0x0d,0xb1,0x2b,0xfa};   //Gigabyte miniPC

/**
 * WIFI variables
 */
//custom static IP
WiFiManager wifiManager;              // non local to access later
char* ApName = "AutoConnectAP";       // Access Point name
char* ApPassword = "password";        // Access Point password
boolean useStaticIp = false;          // should static ip be used.
IPAddress _ip,_gw,_sn;                // ip address objects
String static_ip = "192.168.1.201";   // static ip
String static_gw = "192.168.1.1";     // static gateway
String static_sn = "255.255.255.0";   // subnet mask



/**
 * WIFI
 * handles wifi connections
 */
void connectWifi(){

    if(useStaticIp){
      //Set up static ip from string
      _ip.fromString(static_ip);
      _gw.fromString(static_gw);
      _sn.fromString(static_sn);
      wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn);
    }
    
    //tries to connect to last known settings
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP" with password "password"
    //and goes into a blocking loop awaiting configuration
    if(WiFi.status() != WL_CONNECTED){
      if (!wifiManager.autoConnect(ApName, ApPassword)) {
        Serial.println("failed to connect, we should reset as see if it connects");
        delay(3000);
        ESP.reset();
        delay(5000);
      }
      //if you get here you have connected to the WiFi
      Serial.println("connected...yeey :)");
      Serial.println("local ip");
      Serial.println(WiFi.localIP());
      UDP.begin(src_port); //required for the WOL magic packet 
      String mensaje = "Telegram bot online with ip: " + WiFi.localIP().toString();
      //sendMsg(groupId, mensaje);
      
    }
    /*
    if((computer_ip.toString()).equals("0.0.0.0")){
      ping_ip.fromString(WiFi.gatewayIP().toString());
    }
    */
}

/**
 * WIFI
 * Reset wifi configuration and reboot
 */
void resetWifiSettings(){
  wifiManager.resetSettings();
}

/**
 * WIFI
 * Configure usage of static ip and reboot.
 * Changes applyed after reboot.
 */
void setUseStaticIp(boolean useStaticIpBool){
  useStaticIp = useStaticIpBool;
  //save configuration and later reset
  /*
  ESP.reset();
  delay(5000);
  */
}

/**
 * WOL
 * send wake on lan packet
 */
 /*
void sendWOLPacket(){
  WakeOnLan::sendWOL(computer_ip, UDP, mac, sizeof mac, dst_port);  
}
*/
/**
 * WIFI
 * Inittialize configuration portal on demand
 */
void initConfigPortal(){
    if (!wifiManager.startConfigPortal(ApName, ApPassword)) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    }
}

/**
 * WIFI
 * ping router gateway or computer ip
 */
 /*
boolean pingIp(){
  boolean ret;
  if(!(computer_ip.toString()).equals("0.0.0.0")){
    ret = Ping.ping(ping_ip,2);
  }else{
    ret = Ping.ping(computer_ip,2);
  }
  return ret;
}
*/
