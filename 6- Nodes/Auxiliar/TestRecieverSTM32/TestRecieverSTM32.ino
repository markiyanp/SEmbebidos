#include<SPI.h>
#include"nRF24L01.h"
#include"RF24.h"
#include "data.h"

RF24 radio(PB0,PA4);
const uint8_t addresses[][6] = {"1Node", "2Node"};

struct DATA data; 

void setup(void){
Serial.begin(9600);
radio.begin();
radio.setRetries(15,15);
radio.printDetails();
radio.openReadingPipe(1,addresses[0]);
//radio.setDataRate(RF24_250KBPS);
radio.startListening();
}


void loop(void){
if (radio.available()){
radio.read(&data, sizeof(data));
if(data.id == 1){
  Serial.println("#####################");
  Serial.print("##TEMP:  ");
  Serial.print(data.tmp);
  Serial.println("        ##");
  Serial.print("##HUM:   ");
  Serial.print(data.hum);
  Serial.println("        ##"); 
  Serial.println("#####################"); 
}else if(data.id == 2){
  Serial.println("#####################");
  Serial.print("##MOVM:  ");
  Serial.print(data.tmp);
  Serial.println("         ##");
  Serial.println("#####################"); 
}else if(data.id == 1){
  Serial.println("#####################");
  Serial.print("##DOOR:  ");
  Serial.print(data.tmp);
  Serial.println("        ##");
  Serial.println("#####################"); 
}
Serial.println("");
}
delay(100);
}
