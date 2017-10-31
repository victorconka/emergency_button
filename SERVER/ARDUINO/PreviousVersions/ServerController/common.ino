void alert(){
if (DEBUG){
  Serial.println(F("alert()"));
}
  toggle('b');
  //ENABLE ALERT AND SAVE CONFIGURATION TO INTERNAL MEMORY
  ALERT = true;
  saveConfig();
  
  // TRIGGER IFTTT
  triggerIFTTT("");
  
  // ALERT TELEGRAM
  String alertMsg = "ALERT BUTTON PRESSED";
  sendMsg(telegram_group,alertMsg);
  for(uint8_t i = 0;i < numContacts; i++){
    if(telegram_contacts[i] != EMPTY){
      sendMsg(telegram_contacts[i],alertMsg);    
    }
  }
  
  //TURN ON PC
  sendWOLPacket();
  
  //DISABLE ALERT AND SAVE CONFIGURATION TO INTERNAL MEMORY
  ALERT = false;
  saveConfig();
  toggle('b');
}

/**
 * COMMON
 * Read analog button value and decide what button was pressed.
 * Buttons are connected to different resistor values, therefore
 * each button gives a different value when read.
 */
void readButtons(){
  btn = analogRead(button)/100;
  if(btn>0){
    //read stable value. assign max value.
    int btnStable = 0;
    for(uint8_t i = 0;i < 5; i++){
      btnStable = analogRead(button);
      btnStable = btnStable/100;
      if(btnStable > btn){
        btn = btnStable;  
      }
    }
    if (DEBUG){
      Serial.print(F("Button value: "));
      Serial.println(btn);
    }
    if(btn == 5){
      btn = 4;
    }else if(btn == 8){
      btn = 5;
    }
    toggle('b');
    if(!contactPerson(btn)){
      delay(300);
      toggle('b');
      toggleOnce('r');
    }else{
      toggle('b');
    } 
  }
}
/**
 * COMMON
 * TOGGLE LED ONCE
 * Turn led on and off
 */
void toggleOnce(char ledColor){
if (DEBUG){
  Serial.println(F("toggleOnce()"));
}   
  toggle(ledColor);
  delay(300);
  toggle(ledColor);
}

/**
 *  COMMON
 *  Toggle led from low to high and viceversa 
 */
void toggle(char color){
  
    if (DEBUG){
    Serial.println(F("toggle()")); 
    Serial.println(ESP.getFreeHeap()); 
    }
  if(ledState){
    ledState = false;  
    if(color == 'b'){
      digitalWrite(blueLed, LOW);
    }else if(color == 'r'){
      digitalWrite(redLed, LOW);
    }
  }else{
    ledState = true;
    if(color == 'b'){
      digitalWrite(blueLed, HIGH);
    }else if(color == 'r'){
      digitalWrite(redLed, HIGH);
    }
  }
}

/**
 * COMMON
 * convert string to char array
 */
char* string2char(String command){
    if (DEBUG){
      Serial.println(F("string2char()")); 
    }
    if(command.length()!=0){
        char *p = const_cast<char*>(command.c_str());
        return p;
    }
}
