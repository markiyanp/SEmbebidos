#include "RF24.h"
#include <stdio.h>
#include "data.h"


RF24 radio(1,3); 

const uint8_t addresses[][6] = {"1Node"}; //Define the communication channel here

volatile DATA data;

void setup() {
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  radio.begin();
  radio.openReadingPipe(1,addresses[0]);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening(); 
}

bool state = true;
void loop(){
  while(radio.available()){
      radio.read((void*)&data,sizeof(data));
      /*if(data.id == 4){
        int state = data.field1;
        if(state == 1){
          digitalWrite(10, LOW);
        }else{
          digitalWrite(10, HIGH);
        }*/
        if(state){
      digitalWrite(10, HIGH);
      state = false;
    }else{
      digitalWrite(10, LOW);
      state = true;
    }
    
  }
}
