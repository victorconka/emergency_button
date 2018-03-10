# Emergency Button  
## About  
"emergency button" project based on attiny85, nrf24l01, esp8266 based devboard, telegram and email API's  
![Server and Emergency Button](/imgs/IMG_20171120_012127_170.jpg)  
### Main funcions:  
*     	Attend Telegram Commands.  
*     	Telegram Authorization (access control).  
*     	Listen for the Alert button being pressed.  
*     	Send WOL packet to the PC.  
*     	Send Ping packets to the configured IP Address.  
### Folder contains:  
* UML project files.  
* Enclosure for both, button and server. (opensource, not mine).  
* libraries used to compile arduino files for the server since the compilation is tricky because of the number of libraries used and available SRAM.  
* The code for Arduino IDE.  
* Schematics for the circuits (be aware that some of the part libraries were missing, so same shape parts were used.)  
  
## Configuration  
### Server  
#### Schematic:  
![Emergency Button Server Schematic](/imgs/server_schematic2.png)  
#### PCB  
![server protoboard pcb](/imgs/IMG_20171120_180733_227_cr.jpg)  
### Emergency Button  
#### Schematic  
![emergency button schematic](/imgs/schematic_vck_v1.5.1..png)  
#### PCB Layout  
![emergency button pcb layout](/imgs/vck_v1.5.1.png)  
#### PCB Implementation  
##### BACK
![emergency button pcb implementation back](/imgs/IMG_20171120_164349_951.jpg)  
##### FRONT
![emergency button pcb implementation fron](/imgs/IMG_20171120_163525_609.jpg)  
#### Battery Consumption  
![emergency button battery consumption](/imgs/emergency_button_energy_consumption.jpg)  
## Further Improvements  
### Battery improvement  
Only a step-down voltage regulator is included in the circuit. This means we can use batteries with voltage values up to 6V. Battery voltage will be stepped down to 3.3V, which is the value at which the circuit operates.  
Device uses CR2032 batterie which nominal value is 3V. Once it's charge goes under 2.6V, NRF24L01 is unable to operate properly.  
Adding an additional step up resistor will consume some extra microAmps, but in exchange the battery will get dryed out down to 0.8V.  