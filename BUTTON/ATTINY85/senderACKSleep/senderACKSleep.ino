/**
 * Sender assumes the role of the button. button sends the message.
 */

#include "nRF24L01.h"
#include "RF24.h"
#include <avr/sleep.h>
#include <avr/interrupt.h>

#define CE_PIN 3
#define CSN_PIN 3 //IF we are using 3 pin configuration we will use same pin for both CE and CSN, E.G. pin 3

#define BTN_PIN 3 // if used a differen button to pb3, sleep interrupt button needs to be changed
#define NRF_VCC_PIN 4
//int NRF_VCC_PIN = 4;

RF24 radio(CE_PIN, CSN_PIN);  //NRF24 object
const byte rxAddr[][7] = {"server","button"}; //device roles
byte counter = 1; //value that will be sent to the other device
boolean bReleased = HIGH;

void setup() {
  pinMode(BTN_PIN, INPUT);
  pinMode(NRF_VCC_PIN, OUTPUT);
}

void nrfON(){
//---------------------NRF-CONFIGURATION------------------------------------------------ 
  digitalWrite(NRF_VCC_PIN,HIGH); // turn nrf_vcc ON
  delay(50);
  radio.powerUp();  
//--------------------------------------------------------------------------------------
  radio.begin(); // Start up the radio
  radio.setAutoAck(1); // Ensure autoACK is enabled
  radio.setRetries(0,15); // Max delay between retries & number of retries
  radio.enableAckPayload();               // Allow optional ack payloads
  radio.openWritingPipe(rxAddr[0]); // Write to device address 'server'
  radio.openReadingPipe(1,rxAddr[1]); // Read on pipe 1 for device address 'button'
//--------------------------------------------------------------------------------------  
}

void nrfOFF(){
  radio.powerDown(); 
  digitalWrite(NRF_VCC_PIN,LOW);//turn nrf_vcc off
}

void writeMSG(){ 
    nrfON(); //turn nrf on to send the message
    
    byte gotByte;  
    int cont = 0;
    
    while(!radio.write( &counter, 1 )){
        delay(250);
    }

    if(!radio.available()){ 
        writeMSG(); 
    }else{
        while(radio.available() ){
          radio.read( &gotByte, 1 );
                    
          if(counter+1 != gotByte){
            writeMSG();
          }else{
            counter++;
          }
      }
    }

    nrfOFF();//message recieved, turn nrf off
}

void sleep() {
    GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
    PCMSK |= _BV(PCINT3);                   // Use PB3 as interrupt pin
    ADCSRA &= ~_BV(ADEN);                   // ADC off
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement

    sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
    sei();                                  // Enable interrupts
    sleep_cpu();                            // sleep

    cli();                                  // Disable interrupts
    PCMSK &= ~_BV(PCINT3);                  // Turn off PB3 as interrupt pin
    sleep_disable();                        // Clear SE bit
    ADCSRA |= _BV(ADEN);                    // ADC on

    sei();                                  // Enable interrupts
} // sleep

ISR(PCINT0_vect) {
    // This is called when the interrupt occurs, but I don't need to do anything in it
    // to send the message once, it is enough to put it inside the loop.
    // besides, the nrf initialization wasn't fuctioning correctly when put here.
}

void loop(void){
  sleep();
  //button press means 2 actions (press and unpress).
  //in order to send the message once, we have to use variable
  //to control such a situation.
  
  if (bReleased) { // is button in released state (open)
    bReleased = LOW; // button is now in pressed state (closed)
    writeMSG(); //send the message
  }
    else { // button was in presse state so has been released
    bReleased = HIGH; // so set flag and go take a nap
  }
  
}
