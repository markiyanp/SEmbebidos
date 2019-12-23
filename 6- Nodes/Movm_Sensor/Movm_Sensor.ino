#include "RF24.h"
#include <stdio.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "data.h"
#include "avr/interrupt.h"

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

RF24 radio(1,3); 

volatile boolean f_wdt = 1;
const uint8_t addresses[][6] = {"1Node"}; //Define the communication channel here

volatile DATA data;



/*
ISR(WDT_vect){
  wdt_disable();  // disable watchdog
}*/

ISR(PCINT8_vect)
{
   check_and_send();
}

void configureInterrupts(){
  GIMSK = 0b00100000;    // turns on pin change interrupts
  PCMSK1 = 0b00000001;    // turn on interrupts on pin PB9
  sei();                 // enables interrupts
}
/*
void myWatchdogEnable(const byte interval){ 
  MCUSR = 0;                          // reset various flags
  WDTCSR |= 0b00011000;               // see docs, set WDCE, WDE
  WDTCSR =  0b01000000 | interval;    // set WDIE, and appropriate delay
 
  wdt_reset();
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  sleep_mode();            // now goes to Sleep and waits for the interrupt
}*/

void setup() {
  setup_watchdog(9);// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
                      // 6=1sec, 7=2 sec, 8=4 sec, 9= 8sec
  pinMode(10, INPUT);
  radio.begin();
  radio.openWritingPipe(addresses[0]);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  configureInterrupts();
}


void loop(){
    check_and_send();
    system_sleep(); 
}

void check_and_send(){
  int aux = digitalRead(10);
    if(aux){
      data = {2 , 1 , 1};
    }else{
      data = {2 , 0 , 0};
    }
    radio.write(&data, sizeof(data));
}

void system_sleep()
  {
    cbi(ADCSRA, ADEN);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_mode();
    sleep_disable();
    sbi(ADCSRA, ADEN);
  }

  void setup_watchdog(int ii)
  {
    byte bb;
    int ww;
    if (ii > 9 ) ii = 9;
    bb = ii & 7;
    if (ii > 7) bb |= (1 << 5);
    bb |= (1 << WDCE);
    ww = bb;
    MCUSR &= ~(1 << WDRF);
    WDTCSR |= (1 << WDCE) | (1 << WDE);
    WDTCSR = bb;
    WDTCSR |= _BV(WDIE);
  }
  ISR(WDT_vect)
  {
    f_wdt = 1;
  }
