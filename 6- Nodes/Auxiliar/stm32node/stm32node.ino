/*
 * NRF24L01       Arduino_ Uno    Arduino_Mega    Blue_Pill(stm32f01C)
 * ____________________________________________________________________________
 * VCC        |       3.3v      |     3.3v      |     3.3v
 * GND        |       GND       |     GND       |      GND
 * CSN        |   Pin10 SPI/SS  | Pin10 SPI/SS  |     A4 NSS1 (PA4) 3.3v
 * CE         |   Pin9          | Pin9          |     B0 digital (PB0) 3.3v
 * SCK        |   Pin13         | Pin52         |     A5 SCK1   (PA5) 3.3v
 * MOSI       |   Pin11         | Pin51         |     A7 MOSI1  (PA7) 3.3v
 * MISO       |   Pin12         | Pin50         |     A6 MISO1  (PA6) 3.3v
 * 
 * 
 */

#include <SPI.h>
#include "RF24.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
//#include "STM32LowPower.h"


#define DHTPIN_GND       PA2
#define DHTPIN           PA0
#define DHTPIN_VCC       PC15
#define DHTTYPE          DHT11

DHT_Unified dht(DHTPIN, DHTTYPE);

RF24 radio(PB0, PA4); // CE, CSN on Blue Pill 
const uint8_t addresses[][6] = {"1Node", "2Node"};

//uint32_t delayMS;
//unsigned long delTime = 10000;


void setup() {
pinMode(DHTPIN_GND, OUTPUT);
pinMode(DHTPIN_VCC, OUTPUT);
digitalWrite(DHTPIN_GND, LOW);
digitalWrite(DHTPIN_VCC, HIGH);
pinMode(PC13, OUTPUT);
Serial.begin(9600);
dht.begin();
sensor_t sensor;
radio.begin();
radio.openWritingPipe(addresses[0]);
radio.setPALevel(RF24_PA_MIN);
radio.stopListening();
//LowPower.begin();
}

void loop(){
  digitalWrite(PC13, LOW);
  const char text[] = "Your Button State is HIGH";
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  Serial.print("Temperature: ");
  Serial.print(event.temperature);
  Serial.println(" *C");
  
  dht.humidity().getEvent(&event);
  Serial.print("Humidity: ");
  Serial.print(event.relative_humidity);
  Serial.println("%");
  radio.write(&text, sizeof(text));  
  digitalWrite(PC13, HIGH);
 // LowPower.deepSleep(5000);
  delay(1000);
}
