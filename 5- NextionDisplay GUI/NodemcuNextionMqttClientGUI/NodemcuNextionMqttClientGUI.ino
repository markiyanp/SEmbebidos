#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Nextion.h"
#include <time.h>
#include <TimeLib.h>
 
const char* ssid = "Magellan";
const char* password =  "1234567890";
const char* mqttServer = "m20.cloudmqtt.com";
const int mqttPort = 16824;
const char* mqttUser = "ogrnngtp";
const char* mqttPassword = "FFL9uK_43iFq";
int timezone = 1;
int dst = 0;
 
WiFiClient espClient;
PubSubClient client(espClient);

// Declare a button object [page id:0,component id:1, component name: "b0"]
NexDSButton bt0 = NexDSButton(3, 2, "relay");
//NexButton bt0 = NexButton(3, 3, "b0");
NexText temp = NexText(4, 6, "temp");
NexText hum = NexText(4, 7, "hum");
NexCrop wifiState = NexCrop(0, 9, "q3");
NexWaveform temp_wave = NexWaveform(4, 2, "s0");
NexWaveform hum_wave = NexWaveform(4, 3, "s1");

//Register a button object to the touch event list  
NexTouch *nex_listen_list[] = {
  &bt0,
  NULL
};

const char *tru = "1";
const char *temptopic = "semb/temp";
const char *humtopic = "semb/hum";
const char *doortopic = "semb/door";
const char *motiontopic = "semb/movm";
const char *relaytopic = "semb/relay";

// Button component push callback function 
void bt0Callback(void *ptr) {
  uint32_t val;
  bt0.getValue(&val);
  if(val == 1){
    client.publish("semb/relay", "1");
  }else{
    client.publish("semb/relay", "0");
  }
}

String messageTemp = "";
int totalVal;
int a = 0;
int b = 0;

int lastMinutes;

void updateTemp(){
     char *message = "";
     sprintf(message, "sensors.temp.txt=\"%d\"", totalVal);
     int atotalVal = map(totalVal, 0, 100, 0, 55);
     sendCommand(message);
     delay(100);
     char *aux = "";
     int maped = map(totalVal, 0, 99, 0, 55);
     sprintf(aux, "add 2,1,%i", maped);
     for(int i = 0; i < 5; i++){
      Serial.print(aux);
      Serial.write(0xFF);
      Serial.write(0xFF);
      Serial.write(0xFF);
     }
}


void updateHumi(){
     char *msg = "";
     sprintf(msg, "sensors.hum.txt=\"%d\"", totalVal);
     int vtotalVal = map(totalVal, 0, 100, 0, 55);
     sendCommand(msg);
     delay(100);
     char *aux = "";
     int maped = map(totalVal, 0, 99, 0, 55);
     sprintf(aux, "add 3,1,%i", maped);
     for(int i = 0; i < 5; i++){
      Serial.print(aux);
      Serial.write(0xFF);
      Serial.write(0xFF);
      Serial.write(0xFF);
     }
}

void updateDoor(int state){
  if(state == 1){
      sendCommand("sensors.t4.txt=\"OPEN\"");
      Serial.print("sensors.t4.pco=61440");
      Serial.write(0xFF);
      Serial.write(0xFF);
      Serial.write(0xFF);
     }else{
      sendCommand("sensors.t4.txt=\"CLOSED\"");
      Serial.print("sensors.t4.pco=5828");
      Serial.write(0xFF);
      Serial.write(0xFF);
      Serial.write(0xFF);
     }
}

void updateMovm(int state){
  if(state == 1){
      sendCommand("sensors.t5.txt=\"MOVMNT\"");
      Serial.print("sensors.t5.pco=61440");
      Serial.write(0xFF);
      Serial.write(0xFF);
      Serial.write(0xFF);
     }else{
      sendCommand("sensors.t5.txt=\"NO MOV\"");
      Serial.print("sensors.t5.pco=5828");
      Serial.write(0xFF);
      Serial.write(0xFF);
      Serial.write(0xFF);
     }
}

void callback(char* topic, byte* payload, unsigned int length) {
  messageTemp = "";
  totalVal = 0;
  a = 0;
  b = 0;
  if(length >1){
    a= ((char) payload[0]) - '0';
    b = ((char) payload[1]) - '0';
    totalVal = a*10 + b;
  }else{
    a= ((char) payload[0]) - '0';
    totalVal = b;
  }
  for (int i = 0; i < length; i++) {
    messageTemp += (char)payload[i];
  }

  if(strcmp(topic, temptopic) == 0){
    updateTemp();
  }
  if(strcmp(topic, humtopic) == 0){
    updateHumi();
  }
  if(strcmp(topic, doortopic) == 0){
    updateDoor((char)payload[0]- '0');
  }
  if(strcmp(topic, motiontopic) == 0){
    updateMovm((char)payload[0]- '0');
  }
  if(strcmp(topic, relaytopic) == 0){
    if(((char)payload[0] - '0') == 1){
      bt0.setValue(1);
    }else{
      bt0.setValue(0);
    }
  }
}

void setup() {
  nexInit();
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_BUILTIN, HIGH);
  bt0.attachPush(bt0Callback, &bt0);
  
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.println("Connecting to WiFi..");
    sendCommand("main.q3.picc=1");
    sendCommand("main.t2.txt=\"connecting\"");
  }
  //Serial.println("Connected to the WiFi network");
  sendCommand("main.q3.picc=2");
  sendCommand("main.t2.txt=\"Magellan\"");
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    if (client.connect("ESP32Client", mqttUser, mqttPassword )) {
    } else {
      delay(2000);
    }
  }
  client.subscribe("semb/temp");
  client.subscribe("semb/hum");
  client.subscribe("semb/door");
  client.subscribe("semb/movm");
  client.subscribe("semb/relay");

  configTime(0, 0, "pt.pool.ntp.org", "time.nist.gov");
  while (!time(nullptr)) {
    delay(1000);
  }
  pinMode(LED_BUILTIN, LOW);
}

void loop() {
time_t now = time(nullptr);
  
if(minute(now) != lastMinutes){
  char *toPrint = "";
  if(minute(now) <10){
    if(hour(now) < 10){
      sprintf(toPrint, "main.t0.txt=\"0%d:0%d\"", hour(now), minute(now));
    }else{
      sprintf(toPrint, "main.t0.txt=\"%d:0%d\"", hour(now), minute(now));
    }
  }else{
    if(hour(now) < 10){
      sprintf(toPrint, "main.t0.txt=\"0%d:%d\"", hour(now), minute(now));
    }else{
      sprintf(toPrint, "main.t0.txt=\"%d:%d\"", hour(now), minute(now));
    }
  }
  lastMinutes = minute(now);
  sendCommand(toPrint);
}
  client.loop();
  nexLoop(nex_listen_list);
}
