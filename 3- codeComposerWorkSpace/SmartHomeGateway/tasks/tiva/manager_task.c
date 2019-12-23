#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
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
#include "mqtt/mqtt_client.h"
#include "utils/uart_config.h"
#include "nrf/nrf24l01.h"
#include <string.h>

//*****************************************************************************
//
// The stack size for the display task.
//
//*****************************************************************************
#define MANAGERSTACKSIZE        512         // Stack size in words

#define NRF_ITEM_SIZE           sizeof(data)
#define NRF_QUEUE_SIZE          5

extern xQueueHandle g_pRadioRcvQueue;
extern xQueueHandle g_pRadioSndQueue;
extern xQueueHandle g_pManagerRadioToSendQueue;
extern xQueueHandle g_pEspMqttSndQueue;

struct DATA
{
    uint8_t id;
    uint8_t field1;
    uint8_t field2;
} data;

struct PubPacket
{
    char *topic;
    char *message;
};

struct NrfSendPacket
{
    char topic[128];
    char message[128];
} nrfsnd;

static void ManagerTask(void *pvParameters)
{
    while (1)
    {
        if ((xQueueReceive(g_pRadioRcvQueue, &data, 0) != pdFALSE))
        {
            struct PubPacket pack1;
            struct PubPacket pack2;
            struct PubPacket pack3;
            struct PubPacket pack4;
            char tem[6];
            char hum[6];
            char movm[6];
            char door[6];
            switch (data.id)
            {
            case 1:
                sprintf(tem, "%d", data.field2);
                pack1.topic = "semb/temp";
                pack1.message = tem;
                xQueueSend(g_pEspMqttSndQueue, &pack1, 0);
                sprintf(hum, "%d", data.field1);
                pack2.topic = "semb/hum";
                pack2.message = hum;
                xQueueSend(g_pEspMqttSndQueue, &pack2, 0);
                break;
            case 2:
                sprintf(movm, "%d", data.field1);
                pack3.topic = "semb/movm";
                pack3.message = movm;
                xQueueSend(g_pEspMqttSndQueue, &pack3, 0);
                break;
            case 3:
                sprintf(door, "%d", data.field1);
                pack4.topic = "semb/door";
                pack4.message = door;
                xQueueSend(g_pEspMqttSndQueue, &pack4, 0);
                break;
            default:
                break;
            }

        }
        if ((xQueueReceive(g_pManagerRadioToSendQueue, &nrfsnd, 0) != pdFALSE))
        {
            if (nrfsnd.message[0] == 1)
            {
                data.id = 4;
                data.field1 = 1;
                data.field2 = 1;
                safePrintf("ON");
            xQueueSend(g_pRadioSndQueue, &data, 0);
        }
        else
        {
            data.id = 4;
            data.field1 = 0;
            data.field2 = 0;
            safePrintf("OFF");     ///TODO  tirar esta martelada
        xQueueSend(g_pRadioSndQueue, &data, 0);
    }
}
}
}

//*****************************************************************************
//
// Initializes the Manager Task
//
//*****************************************************************************
uint32_t ManagerTaskInit(void)
{
if (xTaskCreate(ManagerTask, (const portCHAR *)"MANAGE", MANAGERSTACKSIZE, NULL,
    tskIDLE_PRIORITY + PRIORITY_MANAGER_TASK, NULL) != pdTRUE)
{
return (1);
}
return (0);
}

