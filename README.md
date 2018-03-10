# Emergency Button  
"emergency button" project based on attiny85, nrf24l01, esp8266 based devboard, telegram and email API's  
![Server and Emergency Button](/imgs/IMG_20171120_012127_170.jpg)  
Main funcions:  
*     	Attend Telegram Commands.  
*     	Telegram Authorization (access control).  
*     	Listen for the Alert button being pressed.  
*     	Send WOL packet to the PC.  
*     	Send Ping packets to the configured IP Address.  
Folder contains:  
* UML project files.  
* Enclosure for both, button and server. (opensource, not mine).  
* libraries used to compile arduino files for the server since the compilation is tricky because of the number of libraries used and available SRAM.  
* The code for Arduino IDE.  
* Schematics for the circuits (be aware that some of the part libraries were missing, so same shape parts were used.)  
 
