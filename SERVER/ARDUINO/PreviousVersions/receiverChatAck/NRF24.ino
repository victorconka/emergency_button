#include <SPI.h>                  // NRF24   SPI
#include "RF24.h"                 // NRF24   LIBRARY

/**
 * NRF24 variables
 */
#define CE_PIN D2             //NRF CE PIN
#define CSN_PIN D8            //NRF CSN PIN
RF24 radio(CE_PIN, CSN_PIN);  //NRF object 
const byte address[][7] = {"server" , "button"};  //NRF communication roles
byte pipeNo;                  //pipe Number the message was received from
byte gotByte;                 //value recieved
byte gotBytePrev = 0;         //previous value received. needed to keep track of repeated messages

/**
 * NRF24
 * initialize nrf module
 */
void setupNRF(){ 
  radio.begin(); // Start up the radio
  radio.setAutoAck(1); // Ensure autoACK is enabled
  radio.setRetries(0,15); // Max delay between retries & number of retries
  radio.enableAckPayload();       // Allow optional ack payloads
  radio.openWritingPipe(address[1]); // Write to device address 'button'
  radio.openReadingPipe(1, address[0]); // Write to device address 'SimpleNode'
  radio.startListening();
}



/**
 * NRF 24
 * TOGGLE LED ONLY ONCE
 * Make sure led toggles only once because the button
 * sends message until ack is received, so such message
 * is sent and received by the server more than once
 */
void toggleOnce(){
  if(gotBytePrev != gotByte){
    toggle();
    alert();
    gotBytePrev = gotByte;
  }
}

/**
 * NRF24
 * listen to messages
 */
 void listenNRF(){
    while( radio.available(&pipeNo)){
      radio.read( &gotByte, 1 );
      toggleOnce();
      gotByte++;
      radio.writeAckPayload(pipeNo,&gotByte, 1 );
      Serial.print("Got Payload ");
      Serial.println(gotByte);
  }
}
