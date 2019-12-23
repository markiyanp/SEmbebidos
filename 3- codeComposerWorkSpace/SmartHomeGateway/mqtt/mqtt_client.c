#include <stdint.h>
#include "string.h"
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "tasks/esp/esp_logic_task.h"
#include "driverlib/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "esp/esp_uart.h"
#include "at/at.h"
#include "semphr.h"

//****************************************************************************
//
// MQTT Client properties
//
//****************************************************************************

char *MQTT_user = "ogrnngtp";
char *MQTT_pass = "FFL9uK_43iFq";
char *MQTT_type = "MQIsdp";
char *MQTT_CID = "GATEWAY";

char *BROKER_IP = "m20.cloudmqtt.com"; //TODO this will not have any effect. Change the function
char *BROKER_PORT = "16824";

//****************************************************************************

extern xSemaphoreHandle g_pUART1TXSemaphore;
extern xSemaphoreHandle g_pCanTransmitMqttSemaphore;

void MQTT_PUB(char *MQTT_topic, char *MQTT_messege)
{
    int len = strlen(MQTT_topic) + strlen(MQTT_messege) + 4;

    xSemaphoreTake(g_pUART1TXSemaphore, portMAX_DELAY);
    CIPSENDesp(len);
    xSemaphoreTake(g_pCanTransmitMqttSemaphore, 1000);
    UARTCharPut(UART1_BASE, 0x30);
    UARTCharPut(UART1_BASE, strlen(MQTT_topic) + strlen(MQTT_messege) + 2);
    UARTCharPut(UART1_BASE, 0x00);
    UARTCharPut(UART1_BASE, strlen(MQTT_topic));
    SendCharSequence(MQTT_topic);
    SendCharSequence(MQTT_messege);
    xSemaphoreGive(g_pUART1TXSemaphore);
}

void MQTT_SUB(char *MQTT_topic)
{
    xSemaphoreTake(g_pUART1TXSemaphore, portMAX_DELAY);
    int len = strlen(MQTT_topic) + 7;
    CIPSENDesp(len);
    xSemaphoreTake(g_pCanTransmitMqttSemaphore, 1000);
    UARTCharPut(UART1_BASE, 0x82);
    UARTCharPut(UART1_BASE, strlen(MQTT_topic) + 5);
    UARTCharPut(UART1_BASE, 0x00);
    UARTCharPut(UART1_BASE, 0x01);
    UARTCharPut(UART1_BASE, 0x00);
    UARTCharPut(UART1_BASE, strlen(MQTT_topic));
    SendCharSequence(MQTT_topic);
    UARTCharPut(UART1_BASE, 0x00);
    xSemaphoreGive(g_pUART1TXSemaphore);
}

//TODO make return bool. For sending control
void MQTT_CONNECT()
{
    xSemaphoreTake(g_pUART1TXSemaphore, portMAX_DELAY);
    int len = strlen(MQTT_type) + strlen(MQTT_CID) + strlen(MQTT_user) + strlen(MQTT_pass) + 14;
    CIPSENDesp(len);
    xSemaphoreTake(g_pCanTransmitMqttSemaphore, 1000);
    //FIXED HEADER
    UARTCharPut(UART1_BASE, 0x10); //Byte1 - Control packet type (CONNECT)
    UARTCharPut(UART1_BASE,strlen(MQTT_type) + strlen(MQTT_CID) + strlen(MQTT_user)
     + strlen(MQTT_pass) + 12); //Byte 2 - Variable header (10 bytes) plus the length of the Payload

    //VARIABLE HEADER
    UARTCharPut(UART1_BASE, 0x00);  //Byte 1 - MSB of protocol Length
    UARTCharPut(UART1_BASE, strlen(MQTT_type)); //Byte2 - LSB of protocol Length
    SendCharSequence(MQTT_type); //Bytes 3/9 - Protocol Name
    UARTCharPut(UART1_BASE, 0x03); //Byte 10 - Protocol level.
    UARTCharPut(UART1_BASE, 0xC2); //Byte 11 - Connection Flags
    UARTCharPut(UART1_BASE, 0x00); //Byte 12 - Keepalive MSB
    UARTCharPut(UART1_BASE, 0x3C); //Byte 13 - Keepalive LSB
    UARTCharPut(UART1_BASE, 0x00); //Byte 14 - Client ID MSB
    UARTCharPut(UART1_BASE, strlen(MQTT_CID)); //Byte 15 -Client ID LSB
    SendCharSequence(MQTT_CID); //Byte 15/22 - Client ID
    UARTCharPut(UART1_BASE, 0x00); //Byte 23 Username length MSB
    UARTCharPut(UART1_BASE, strlen(MQTT_user)); //Byte 24 - Username length LSB
    SendCharSequence(MQTT_user); //Byte 24/32 - Username
    UARTCharPut(UART1_BASE, 0x00); //Byte 33 - password length MSB
    UARTCharPut(UART1_BASE, strlen(MQTT_pass)); //Byte 34 - password LSB
    SendCharSequence(MQTT_pass); //Byte 35/47 - password
    xSemaphoreGive(g_pUART1TXSemaphore);
}