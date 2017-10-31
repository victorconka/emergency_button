/**
 * NRF24
 * initialize nrf module
 */
void setupNRF(){
if (DEBUG){
  Serial.println(F("setupNRF()"));
}
  radio.begin();                           // Setup and configure rf radio
  radio.setChannel(126);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);         // WITH LOWER DATA RATE WEMOS DOES NOT REBOOT
  radio.setAutoAck(1);                     // Ensure autoACK is enabled
  radio.enableAckPayload();                // Allow optional ack payloads
  radio.setRetries(15,15);                 // Optionally, increase the delay between retries & # of retries
  radio.setCRCLength(RF24_CRC_8);          // Use 8-bit CRC for performance
  radio.openWritingPipe(address[1]);       // write to button
  radio.openReadingPipe(1,address[0]);     // receive data as server
  radio.startListening();                  // Start listening
  //radio.printDetails();                   // Dump the configuration of the rf unit for debugging
  radio.powerUp();                         //Power up the radio
}

/**
 * NRF24
 * listen to messages
 */
void listenNRF(){
  if(ALERT){
    alert();
  }
  while( radio.available(&pipeNo)){
      radio.read( &gotByte, 1 );
      if(gotBytePrev != gotByte){
        alert();
        gotBytePrev = gotByte;
      }
      gotByte++;
      //ACK IS SENT ONCE ALERTWAS SENT. IT'S A PRIORITY.
      radio.writeAckPayload(pipeNo,&gotByte, 1 );
  }
}

