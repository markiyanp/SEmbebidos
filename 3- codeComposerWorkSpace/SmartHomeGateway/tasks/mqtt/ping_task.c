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

#define ESPPINGTASKSTACKSIZE        64         // Stack size in words

extern bool brokerConnection;
extern bool configurationRunning;
extern xSemaphoreHandle g_pUART1TXSemaphore;
extern xSemaphoreHandle g_pCanTransmitMqttSemaphore;

static void EspPingTask(void *pvParameters)
{
    while (1)
    {
        vTaskDelay(10000);
        xSemaphoreTake(g_pUART1TXSemaphore, portMAX_DELAY);
        SendATCommand("AT+CIPSEND=2");
        xSemaphoreTake(g_pCanTransmitMqttSemaphore, 1000);
        UARTCharPut(UART1_BASE, 0xC0);
        UARTCharPut(UART1_BASE, 0x00);
        xSemaphoreGive(g_pUART1TXSemaphore);
    }
}

uint32_t EspPingTaskInit(void)
{
    if (xTaskCreate(EspPingTask, (const portCHAR *)"ESPPING", ESPPINGTASKSTACKSIZE, NULL,
            tskIDLE_PRIORITY + PRIORITY_PING_TASK, NULL) != pdTRUE)
    {
        return (1);
    }
    return (0);
}