#define CE_PIN 7
#define CSN_PIN 8


#include <SPI.h>
#include "RF24.h"

RF24 radio(CE_PIN, CSN_PIN);
boolean ledState = false;
int led = 5;
const byte address[][6] = {"server","button"};
unsigned long payload = 0;

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

void toggle(){
  
  if(ledState){
    ledState = false;
    digitalWrite(led, LOW);
  }else{
    ledState = true;
    digitalWrite(led, HIGH);
  }
  
}

void loop(void){
  byte pipeNo;
  byte gotByte;
  
  while( radio.available(&pipeNo)){
      toggle();
      radio.read( &gotByte, 1 );
      gotByte++;
      radio.writeAckPayload(pipeNo,&gotByte, 1 );
      Serial.print("Got Payload ");
      Serial.println(gotByte);
  }
}

