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
#include "driverlib/uart.h"

#include "nrf/nrf24l01.h"

//*****************************************************************************
//
// The stack size for the display task.
//
//*****************************************************************************
#define NRFTASKSTACKSIZE        128         // Stack size in words

#define NRF_ITEM_SIZE           sizeof(data)
#define NRF_QUEUE_SIZE          5

xQueueHandle g_pRadioRcvQueue;

extern xSemaphoreHandle g_pUARTSemaphore;
extern bool configurationRunning;
extern struct nrf24l01p nrf;

struct DATA
{
    uint8_t id;
    uint8_t field1;
    uint8_t field2;
} data;

static void NrfRcvTask(void *pvParameters)
{
    while (1)
    {
        if (nrf24l01p_available(&nrf))
        {
            // Variable for the received timestamp
            while (nrf24l01p_available(&nrf))
            {                 // While there is data ready
                nrf24l01p_read(&nrf, &data, sizeof(data));    // Get the payload
                if (xQueueSend(g_pRadioRcvQueue, &data,
                        (TickType_t ) 0) != pdPASS)
                {
                    xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                    UARTprintf(
                            "NRF RCV queue is full. Data will be discarted.\n");
                    xSemaphoreGive(g_pUARTSemaphore);
                }
                else
                {
                    /*(g_pUARTSemaphore, portMAX_DELAY);
                    UARTprintf("GOT [ID: %d, F1: %d, F2: %d]\n",data.id, data.field1, data.field2);
                    xSemaphoreGive(g_pUARTSemaphore);*/
                }
            }
        }
        vTaskDelay(100);
    }
}

//*****************************************************************************
//
// Initializes the NRF RCV task.
//
//*****************************************************************************
uint32_t NrfRcvTaskInit(void)
{
    g_pRadioRcvQueue = xQueueCreate(NRF_QUEUE_SIZE, NRF_ITEM_SIZE);
    if (xTaskCreate(NrfRcvTask, (const portCHAR *)"RadioRCV", NRFTASKSTACKSIZE, NULL,
            tskIDLE_PRIORITY + PRIORITY_NRF_RCV_TASK, NULL) != pdTRUE)
    {
        return (1);
    }
    return (0);
}
