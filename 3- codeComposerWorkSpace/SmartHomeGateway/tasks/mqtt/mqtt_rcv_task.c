#include <stdbool.h>
#include <stdint.h>
#include "stdio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "drivers/buttons.h"
#include "utils/uartstdio.h"
#include "switch_task.h"
#include "led_task.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "esp/esp_uart.h"
#include "at/at.h"
#include "driverlib/uart.h"
#include <string.h>
#include "tasks/esp/esp_logic_task.h"
#include "mqtt/mqtt_client.h"
#include "utils/uart_config.h"

//*****************************************************************************
//
// The stack size for the display task.
//
//*****************************************************************************
#define ESPTASKSTACKSIZE        256         // Stack size in words

#define RADIO_SEND_ITEM_SIZE       sizeof(nrfsnd)
#define MQTT_ITEM_SIZE           sizeof(brokermessage)
#define MQTT_QUEUE_SIZE          5

char *subscribedTopic = "semb/relay";

xQueueHandle g_pEspMqttRcvQueue;
xQueueHandle g_pManagerRadioToSendQueue;

char brokermessage[128];
char payload[128];
char topic[128];


/*struct DATA
{
    uint8_t id;
    uint8_t hum;
    uint8_t tmp;
} data;*/

struct NrfSendPacket
{
    char topic[128];
    char message[128];
} nrfsnd;

void clearTheRest(int pointer){
    int aux = pointer;
    while(payload[aux] != '\0'){
        payload[aux] = '\0';
        aux++;
    }
}

static void MqttRcvTask(void *pvParameters)
{
    while (1)
    {
        xQueueReceive(g_pEspMqttRcvQueue, &brokermessage, portMAX_DELAY);
        safePrintf(brokermessage);
        if (brokermessage[7] == 0xD0)
        {
            safePrintf("PING ACK");
        }
        if (brokermessage[7] == 0x20 && brokermessage[8] == 0x02)
        {
            safePrintf("CONNACK");
        }
        int a = indexOfMessageEnd(brokermessage, subscribedTopic);
        int aux = 0;
        if(a != (-1) && a < sizeof(brokermessage)){
           while(brokermessage[a] != '\0'){
               payload[aux] = brokermessage[a];
               a++;
               aux++;
           }
           clearTheRest(aux);
           strcpy(nrfsnd.topic, topic);
           strcpy(nrfsnd.message, payload);
           safePrintf("sending to queue...");
           xQueueSend(g_pManagerRadioToSendQueue, &nrfsnd, 0);
        }
    }
}

uint32_t MqttRcvTaskInit(void)
{
    g_pManagerRadioToSendQueue = xQueueCreate(MQTT_QUEUE_SIZE, RADIO_SEND_ITEM_SIZE);
    g_pEspMqttRcvQueue = xQueueCreate(MQTT_QUEUE_SIZE, MQTT_ITEM_SIZE);
    if (xTaskCreate(MqttRcvTask, (const portCHAR *)"MQTT_RCV", ESPTASKSTACKSIZE, NULL,
            tskIDLE_PRIORITY + PRIORITY_MQTT_RCV_TASK, NULL) != pdTRUE)
    {
        return (1);
    }
    return (0);
}