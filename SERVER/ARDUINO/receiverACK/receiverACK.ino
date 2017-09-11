#define CE_PIN D2
#define CSN_PIN D8
#define led D1 
#define button A0

#include <SPI.h>
#include "RF24.h"

RF24 radio(CE_PIN, CSN_PIN);
boolean ledState = false;

const byte address[][7] = {"server" , "button"};
unsigned long payload = 0;
int btn = 0; //analog button value
//nrf variables
byte pipeNo;
byte gotByte; 
byte gotBytePrev = 0;


void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);

  radio.begin(); // Start up the radio
  radio.setAutoAck(1); // Ensure autoACK is enabled
  radio.setRetries(0,15); // Max delay between retries & number of retries
  radio.enableAckPayload();       // Allow optional ack payloads
  radio.openWritingPipe(address[1]); // Write to device address 'button'
  radio.openReadingPipe(1, address[0]); // Write to device address 'SimpleNode'
  radio.startListening();
}

/**
 *  Toggle led from low to high and viceversa 
 */
void toggle(){
  
  if(ledState){
    ledState = false;
    digitalWrite(led, LOW);
  }else{
    ledState = true;
    digitalWrite(led, HIGH);
  }
  
}

/**
 * Make sure led toggles only once because the button
 * sends message until ack is received, so such message
 * is sent and received by the server more than once
 */
void toggleOnce(){
  if(gotBytePrev != gotByte){
    toggle();
    gotBytePrev = gotByte;
  }
}

/**
 * Read analog button value and decide what button was pressed.
 * Buttons are connected to different resistor values, therefore
 * each button gives a different value when read.
 */
void readButtons(){
  btn = analogRead(button)/100;
  if(btn!=0){
    Serial.println(btn);
    for(int i = 0;i < btn; i++){
      toggle();
      delay(300);
      toggle();
      delay(300);
    }
  }
}


void loop(void){

  readButtons();
  while( radio.available(&pipeNo)){
      //toggle();
      toggleOnce();
      radio.read( &gotByte, 1 );
      gotByte++;
      radio.writeAckPayload(pipeNo,&gotByte, 1 );
      Serial.print("Got Payload ");
      Serial.println(gotByte);
  }
  delay(250);
}

