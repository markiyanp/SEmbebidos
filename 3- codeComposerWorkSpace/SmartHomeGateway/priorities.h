#ifndef __PRIORITIES_H__
#define __PRIORITIES_H__

//*****************************************************************************
//
// The priorities of the various tasks.
//
//*****************************************************************************
#define PRIORITY_SWITCH_TASK       2
#define PRIORITY_LED_TASK          1

#define PRIORITY_ESP_RCV_TASK      3 //leave equal to config task
#define PRIORITY_MQTT_SND_TASK     13
#define PRIORITY_MQTT_RCV_TASK     1
#define PRIORITY_CONFIG_TASK       3 //must be equal to config task
#define PRIORITY_LOGIC_TASK        11//higher than esp rcv task works fine
#define PRIORITY_PING_TASK         12
#define PRIORITY_NRF_RCV_TASK      1
#define PRIORITY_NRF_SND_TASK      5
#define PRIORITY_MANAGER_TASK      1

#endif // __PRIORITIES_H__
