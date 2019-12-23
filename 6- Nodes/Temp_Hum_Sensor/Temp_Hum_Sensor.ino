#include <avr/sleep.h>
#include <avr/wdt.h>
#include "RF24.h"
#include "data.h"
#include <dht.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define DHT11_PIN 10
volatile boolean f_wdt = 1;
const uint8_t addresses[][6] = {"1Node"};
RF24 radio(1,3); 
dht DHT;

void setup() 
  {
    setup_watchdog(9);// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
                      // 6=1sec, 7=2 sec, 8=4 sec, 9= 8sec
    radio.begin();
    radio.openWritingPipe(addresses[0]);
    radio.setPALevel(RF24_PA_MIN);
    radio.stopListening(); 
  }

void loop()
  {
    if (f_wdt == 1)
    {
      f_wdt = 0;
      DHT.read11(DHT11_PIN);
      DATA data = {1 , DHT.humidity , (uint8_t)DHT.temperature};
      radio.write(&data,sizeof(data));
      system_sleep(); 
    }
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
  
