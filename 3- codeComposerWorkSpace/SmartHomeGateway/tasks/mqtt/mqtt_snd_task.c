#include <stdbool.h>
#include <stdint.h>
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

#define MQTT_ITEM_SIZE           sizeof(pubPack)
#define MQTT_QUEUE_SIZE          15

xQueueHandle g_pEspMqttSndQueue;
extern bool configurationRunning;

struct PubPacket
{
    char *topic;
    char *message;
} pubPack;

static void MqttSndTask(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 100;
    while (1)
    {
        xLastWakeTime = xTaskGetTickCount();
        xQueueReceive(g_pEspMqttSndQueue, &pubPack, portMAX_DELAY);
        if (!configurationRunning)
        {
//            safePrintf(pubPack.message);
            MQTT_PUB(pubPack.topic, pubPack.message);
        }
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

uint32_t MqttSndTaskInit(void)
{
    g_pEspMqttSndQueue = xQueueCreate(MQTT_QUEUE_SIZE, MQTT_ITEM_SIZE);
    if (xTaskCreate(MqttSndTask, (const portCHAR *)"MQTT_SND", ESPTASKSTACKSIZE, NULL,
            tskIDLE_PRIORITY + PRIORITY_MQTT_SND_TASK, NULL) != pdTRUE)
    {

        return (1);
    }
    return (0);
}
