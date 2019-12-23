#include <stdbool.h>
#include <stdint.h>
#include <tasks/mqtt/mqtt_snd_task.h>
#include "driverlib/interrupt.h"
#include "driverlib/fpu.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "led_task.h"
#include "switch_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "inc/hw_ints.h"
#include "at/at.h"
#include "systick.h"
#include "esp/esp_uart.h"
#include "mqtt/mqtt_client.h"
#include "tasks/tiva/config_task.h"
#include "utils/uart_config.h"
#include <tasks/nrf/nrf_rcv_task.h>
#include "tasks/nrf/nrf_snd_task.h"
#include "tasks/esp/esp_logic_task.h"
#include "tasks/esp/esp_logic_task.h"
#include "nrf/nrf24l01.h"
#include "tasks/esp/esp_rcv_task.h"
#include "tasks/mqtt/ping_task.h"
#include "tasks/tiva/manager_task.h"
#include "tasks/mqtt/mqtt_rcv_task.h"

bool brokerConnection = false;
bool configurationRunning = true;
xSemaphoreHandle g_pUARTSemaphore;
xSemaphoreHandle g_pUART1TXSemaphore;

void vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    while (1)
    {
    }
}

void ConfigureRADIO()
{
    //ENABLE NRF
    if (configureRadio() == 0)
    {
        UARTprintf("\nNRF CONFIGURED!\n");
    }
    else
    {
        UARTprintf("\nNRF NOT CONFIGURED!\n");
        while (1)
        {
        }
    }
}

//*****************************************************************************
//
// Initialize FreeRTOS and start the initial set of tasks.
//
//*****************************************************************************
int main(void)
{
    ROM_FPUEnable();
    ROM_FPULazyStackingEnable();

    ROM_SysCtlClockSet(
    SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

    ConfigureUART();
    ConfigureRADIO();

    g_pUART1TXSemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(g_pUART1TXSemaphore);
    EspPingTaskInit(); //Initializing the TCP ping task. This helps keep the connection with broker Use the handle to delete the task.
    EspLogicTaskInit();
    EspRcvTaskInit();
    ConfigTaskInit();

    vTaskStartScheduler();

    while (1)
    {
    }
}