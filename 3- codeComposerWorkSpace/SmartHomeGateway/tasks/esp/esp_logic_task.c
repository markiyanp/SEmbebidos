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
#include "at/at.h"

//*****************************************************************************
//
// The stack size for the display task.
//
//*****************************************************************************
#define ESPTASKSTACKSIZE        512         // Stack size in words

extern xSemaphoreHandle g_pUARTSemaphore;
xQueueHandle g_pLogicTasksQueue;
xQueueHandle g_pEspRcvQueue;

char receiveBuffer[128];
extern bool configurationRunning;
extern xQueueHandle g_pEspMqttRcvQueue;
extern xSemaphoreHandle g_pATResponseSemaphore;
extern xSemaphoreHandle g_pUART1TXSemaphore;

struct Task
{
    char request[128];
    char response[128];
    int numRetries;
} Receivedtask;

void clearReceiveQueue()
{
    while (xQueueReceive(g_pEspRcvQueue, NULL, 0) != pdFALSE)
    {
    }
}


int checkIfContains(char *source, char *resp)
{
    int i;
    for (i = 0; i <= strlen(source) - strlen(resp); i++)
    {
        int j;
        for (j = 0; j < strlen(resp); j++)
        {
            if (resp[j] == source[i + j])
            {
                if (j == strlen(resp) - 1)
                {
                    return 0;
                }
            }
            else
            {
                j = strlen(resp) + 2;
            }
        }
    }
    return (-1);
}

int indexOfMessageEnd(char *source, char *resp)
{
    int i;
    for (i = 0; i <= strlen(source) - strlen(resp); i++)
    {
        int j;
        for (j = 0; j < strlen(resp); j++)
        {
            if (resp[j] == source[i + j])
            {
                if (j == strlen(resp) - 1)
                {
                    return (i +strlen(resp));
                }
            }
            else
            {
                j = strlen(resp) + 2;
            }
        }
    }
    return (-1);
}

static void EspLogicTask(void *pvParameters)
{
    while (1)
    {
        xQueueReceive(g_pLogicTasksQueue, &Receivedtask, portMAX_DELAY);
        clearReceiveQueue();
        SendATCommand(Receivedtask.request);
        int count;
        for (count = Receivedtask.numRetries; count > 0; count--)
        {
            if (xQueueReceive(g_pEspRcvQueue, &receiveBuffer,
                    (TickType_t ) 200) == pdTRUE)
            {
                if (checkIfContains(receiveBuffer, Receivedtask.response) == 0)
                {
                    while (xQueueReceive(g_pEspRcvQueue, &receiveBuffer,
                            0) != pdFALSE)
                        ;
                    xSemaphoreGive(g_pATResponseSemaphore);
                    count = 0;
                }
            }
        }
    }
}

uint32_t EspLogicTaskInit(void)
{
    g_pLogicTasksQueue = xQueueCreate(1, sizeof(struct Task));
    g_pEspRcvQueue = xQueueCreate(20, sizeof(receiveBuffer));
    if (xTaskCreate(EspLogicTask, (const portCHAR *)"ESPLOGIC", ESPTASKSTACKSIZE, NULL,
            tskIDLE_PRIORITY + PRIORITY_LOGIC_TASK, NULL) != pdTRUE)
    {
        return (1);
    }
    return (0);
}
