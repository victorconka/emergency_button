#include <ESP8266WiFi.h>          // WIFI     connection
#include <WiFiUDP.h>              // WOL     UDP packet for sending WOL magic packet
#include <WakeOnLan.h>            // WOL     functionality

 /**
 * WAKE ON LAN MUST DEFINE VARIABLES
 */
//The target IP address to send the magic packet to.
IPAddress computer_ip(0,0,0,0); //ip used to wake up on lan. 0.0.0.0 used in case ip is unknown
//ping static_gw if computer_ip == "0.0.0.0"
IPAddress static_ip(192,168,1,1); //ip used to ask if it's online

/**
 * WOL VARIABLES
 */
int src_port = 9;
//The targets MAC address to send the packet to
//byte mac[] = {0x4c,0xcc,0x6a,0x60,0x46,0x88};  //miguelPC
//byte mac[] = {0x54,0x04,0xa6,0x2f,0x92,0xd9};   //viktorPc
byte mac[] = {0x1c,0x1b,0x0d,0xb1,0x2b,0xfa};   //Gigabyte miniPC

/**
 * WOL STUFF
 */
WiFiUDP  UDP;

/**
 * WIFI
 * handles wifi connections
 * whenever the device is disconnected from wifi it does the followind:
 ****** if wifi ap is not present -> retry connection -> become AP and let the user define bssid and password
 */
void connectWifi(){
  while(WiFi.status() != WL_CONNECTED){
    // Set WiFi to station mode and disconnect from an AP if it was Previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    
    // Attempt to connect to Wifi network:
    Serial.print("Connecting Wifi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
  
    
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
    }
    
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP().toString());
    
    UDP.begin(src_port); //required for the WOL magic packet 

    String mensaje = "Telegram bot online with ip: " + WiFi.localIP().toString();
    sendMsg(groupId, mensaje);
  }
}

/**
 * WOL
 * send wake on lan packet
 */
void sendWOLPacket(){
  WakeOnLan::sendWOL(computer_ip, UDP, mac, sizeof mac);  
}

String staticIpToString(){
  return static_ip.toString();
}

boolean pingIP(){
  return Ping.ping(static_ip,2);
}
