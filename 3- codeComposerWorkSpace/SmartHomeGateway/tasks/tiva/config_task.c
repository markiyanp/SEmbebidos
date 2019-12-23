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
#include <tasks/mqtt/mqtt_snd_task.h>
#include <tasks/mqtt/mqtt_rcv_task.h>
#include "mqtt/mqtt_client.h"
#include "tasks/esp/esp_rcv_task.h"
#include "utils/uart_config.h"
#include "tasks/mqtt/ping_task.h"
#include "tasks/esp/esp_logic_task.h"
#include <tasks/nrf/nrf_rcv_task.h>
#include "tasks/nrf/nrf_snd_task.h"
#include "tasks/tiva/manager_task.h"

#define ESPTASKSTACKSIZE        128         // Stack size in words

extern bool configurationRunning;
xSemaphoreHandle g_pATResponseSemaphore;
TaskHandle_t xHandle = NULL;
bool at, cwmode, cwjap, cipstart, cipmode, cipmux = false;
extern bool configurationRunning;
void configESP()
{
    while (!(at && cwmode && cwjap && cipstart && cipmode && cipmux))
    {
        at = false;
        cwmode = false;
        cipstart = false;
        cwjap = false;
        cipstart = false;
        cipmux = false;
        cipmode = false;

        while (!at) //AT atention
        {
            at = ATesp();
            vTaskDelay(100);
        }

        RSTesp(); //RST resets the esp
        vTaskDelay(1000);

        while (!cwmode) //CWMODE set the mode to station
        {
            cwmode = CWMODEesp();
            vTaskDelay(100);
        }

        while (!cipmux) //CIPMUX configure to single connection
        {
            cipmux = CIPMUXesp();
            vTaskDelay(100);
        }

        while (!cwjap) //CWJAP join the AP
        {
            cwjap = CWJAPesp();
            vTaskDelay(100);
        }
        while (!cipstart) //CIPSTART starts the TCP connection with broker. Needed for MQTT
        {
            cipstart = CIPSTARTesp();
            vTaskDelay(100);
        }

        while (!cipmode) //CIPMODE sets normal TCP operation mode. Disables the TCP pass-throuhg
        {
            cipmode = CIPMODEEesp();
            vTaskDelay(100);
        }
    }
}

static void ConfigTask(void *pvParameters)
{
    safePrintf("STARTING THE CONFIGURATION");
    configESP();
    configurationRunning = false; //Disable config mode. Changes the uart handler operation mode
    //clearReceiveQueue(); //Clearing received messages from UART

    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x04);

    MQTT_CONNECT(); //TODO this should return boolean!
    vTaskDelay(500);
    MQTT_SUB("semb/relay");
    EspPingTaskInit(); //Initializing the TCP ping task. This helps keep the connection with broker Use the handle to delete the task.

    ManagerTaskInit(); //Initialize the manager task
    MqttSndTaskInit(); //Initializing MQTT send task
    MqttRcvTaskInit();
    NrfSndTaskInit();
    NrfRcvTaskInit();

    if (xHandle != NULL)
    {
        vTaskDelete(xHandle);
    }
}

uint32_t ConfigTaskInit(void)
{
    g_pATResponseSemaphore = xSemaphoreCreateBinary(); //TODO check if its ok here
    if (xTaskCreate(ConfigTask, (const portCHAR *)"CONFIG", ESPTASKSTACKSIZE, NULL,
            tskIDLE_PRIORITY + PRIORITY_CONFIG_TASK, &xHandle) != pdTRUE)
    {
        return (1);
    }
    return (0);
}
