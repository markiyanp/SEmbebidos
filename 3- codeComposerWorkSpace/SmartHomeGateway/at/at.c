#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"
#include <string.h>
#include "tasks/esp/esp_logic_task.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "inc/hw_memmap.h"
#include "driverlib/uart.h"
#include "esp/esp_uart.h"
#include "utils/uart_config.h"

//****************************************************************************
//
// ESP8266 (ESP01) WiFi properties
//
//****************************************************************************

char *SSID = "Markiyan"; //TODO concat *char
char *PASSWD = "fr3t1212"; //TODO concat *char

//****************************************************************************

extern char *BROKER_IP; //TODO concat *char
extern char *BROKER_PORT; //TODO concat *char

extern xSemaphoreHandle g_pUART1TXSemaphore;

bool ATesp(void)
{
    return makeRequest("AT", "OK", 2, 200);
}

bool CIPMODEEesp(void)
{
    return makeRequest("AT+CIPMODE=0", "OK", 2, 200);
}

bool CWMODEesp(void)
{
    return makeRequest("AT+CWMODE=1", "OK", 2, 200);
}

bool RSTesp(void)
{
    SendATCommand("AT+RST");
    return true;
}

bool CWJAPesp(void)
{
    return makeRequest("AT+CWJAP=\"Magellan\",\"1234567890\"", "OK", 6, 3000);
}

bool CWQAPesp(void)
{
    return makeRequest("AT+CWQAP", "OK", 2, 200);
}

bool CIPMUXesp(void)
{
    return makeRequest("AT+CIPMUX=0", "OK", 2, 200);
}

bool CIPSTOesp(void)
{
    return makeRequest("AT+CIPSTO=10000", "OK", 2, 200);
}

bool CIPSTARTesp(void)
{
    return makeRequest("AT+CIPSTART=\"TCP\",\"m20.cloudmqtt.com\",16824", "OK",
                       4, 2000);
}

bool CIPCLOSEesp(void)
{
    return makeRequest("AT+CIPCLOSE", "OK", 2, 200);
}

void CIPSENDesp(int length)
{
    int aux, aux1 = 0;
    aux = length / 10;
    aux1 = length % 10;

    char *command = "AT+CIPSEND=";
   while (UARTBusy(UART1_BASE)){
  //      safePrintf("BUSY");
    }

    while (*command != '\0')
    {
        UARTCharPut(UART1_BASE, *command++);
    }
    UARTCharPut(UART1_BASE, aux + '0'); //CR
    UARTCharPut(UART1_BASE, aux1 + '0');
    UARTCharPut(UART1_BASE, '\r'); //CR
    UARTCharPut(UART1_BASE, '\n'); //LF
}

void SendATCommand(char *cmd)
{
    while (UARTBusy(UART1_BASE)){/*safePrintf("BUSY1");*/}
//    taskENTER_CRITICAL();
    while (*cmd != '\0')
    {
        UARTCharPut(UART1_BASE, *cmd++);
    }
    UARTCharPut(UART1_BASE, '\r'); //CR
    UARTCharPut(UART1_BASE, '\n'); //LF
//    taskEXIT_CRITICAL();
}