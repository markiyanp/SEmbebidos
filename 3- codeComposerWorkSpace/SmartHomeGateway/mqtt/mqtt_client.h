#ifndef MQTT_MQTT_CLIENT_H_
#define MQTT_MQTT_CLIENT_H_

void MQTT_PUB(const char MQTT_topic[15], const char MQTT_messege[15]);
void MQTT_SUB(const char MQTT_topic[15]);
bool MQTT_CONNECT();

#endif /* MQTT_MQTT_CLIENT_H_ */
