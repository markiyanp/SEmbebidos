#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/fpu.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/timer.h"
#include "driverlib/adc.h"
#include "utils/uartstdio.h"
#include "utils/cmdline.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "at/at.h"
#include "systick.h"
#include "utils/uart_config.h"

extern xSemaphoreHandle g_pEspRcvBufferSemaphore;

char COMMAND[128];
char ReceivedData[512];
char str[128];
bool process = true;

char *Substring(char *src, char *dst, int start, int stop);
int SearchIndexOf(char src[], char str[]);
char* itoa(int i, char b[]);
void ftoa(float f, char *buf);

extern int i;

extern xQueueHandle g_pLogicTasksQueue;
extern xSemaphoreHandle g_pEspRcvBufferSemaphore;
extern xSemaphoreHandle g_pATResponseSemaphore;

struct Task
{
    char request[128];
    char response[128];
    int numRetries;
} task;

void SendCharSequence(char *cmd)
{
    while (UARTBusy(UART1_BASE))
    {
      /*  safePrintf("BUSY2");*/
    }
    while (*cmd != '\0')
    {
        UARTCharPut(UART1_BASE, *cmd++);
    }

}

bool makeRequest(char *request, char *response, int numReadLines,
                 int responseTimeout)
{
    strcpy(task.request, request);
    strcpy(task.response, response);
    task.numRetries = numReadLines;
    xQueueSend(g_pLogicTasksQueue, &task, portMAX_DELAY);
    if (xSemaphoreTake(g_pATResponseSemaphore,
                       (TickType_t ) responseTimeout) == pdFALSE)
    {
        return false;
    }
    return true;
}

void clearReceivedData()
{
    xSemaphoreTake(g_pEspRcvBufferSemaphore, portMAX_DELAY);
    i = 0;
    memset(ReceivedData, 0, sizeof(ReceivedData));
    xSemaphoreGive(g_pEspRcvBufferSemaphore);
}

/*int checkIfContains(char *resp)
 {
 int i;
 for (i = 0; i <= strlen(ReceivedData) - strlen(resp); i++)
 {
 int j;
 for (j = 0; j < strlen(resp); j++)
 {
 if (resp[j] == ReceivedData[i + j])
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
 */
/*bool recvFind(char *find)
 {
 //xSemaphoreTake(g_pEspRcvBufferSemaphore, portMAX_DELAY);
 int resp = checkIfContains(find);
 //xSemaphoreGive(g_pEspRcvBufferSemaphore);
 if (resp != -1)
 {
 return true;
 }
 else
 {
 return false;
 }
 }
 */
/*bool recvFindAndFilter(char *target, char *begin, char* end, char *data,
 int timeout)
 {
 //recvString(target, ReceivedData, timeout, true);

 if (SearchIndexOf(ReceivedData, target) != -1)
 {
 int index1 = SearchIndexOf(ReceivedData, begin);
 int index2 = SearchIndexOf(ReceivedData, end);

 if (index1 != -1 && index2 != -1)
 {
 index1 += strlen(begin);
 Substring(ReceivedData, data, index1, index2);
 return true;
 }
 }
 data = "";
 return false;
 }*/

/*void ProcessCommand(char *CommandText)
 {
 long Status;
 char *array[10];
 int i = 0;

 array[i] = strtok(CommandText, ":");

 while (array[i] != NULL)
 {
 array[++i] = strtok(NULL, ":");
 }

 memset(COMMAND, 0, sizeof(COMMAND));
 strncpy(COMMAND, array[1], (strlen(array[1]) - 1));

 UARTprintf("CMD->%s\n", COMMAND);

 Status = CmdLineProcess(COMMAND);

 if (Status == CMDLINE_BAD_CMD)
 {
 UARTprintf("Bad command!\n");
 }
 }*/

/*void QuitProcess(void)
 {
 process = false;
 }*/

//------------------------------------------------------------------------------------------------------------------------------------
/*
 bool ProcessRoutine()
 {
 bool status;
 process = true;

 UARTprintf("Waiting Server...\n");

 while (true)
 {
 status = CIPSTARTesp();
 if (status)
 {
 UARTprintf("Communication is established!\n");
 break;
 }

 delay(50);
 }

 SetServoAngle(90);

 GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x00);
 GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x08); //TurnOn Green Led-> READY

 int i = 0;
 char a;
 memset(ReceivedData, 0, sizeof(ReceivedData)); //Clear
 unsigned long start;

 while (process)
 {
 if (UARTCharsAvail(UART1_BASE))
 {
 if (SearchIndexOf(ReceivedData, "+IPD,") != -1)
 {
 i = 0;
 memset(ReceivedData, 0, sizeof(ReceivedData));

 start = millis();

 while (millis() - start < 5000)
 {
 while (UARTCharsAvail(UART1_BASE))
 {
 a = UARTCharGet(UART1_BASE);
 if (a == '\0')
 continue;
 ReceivedData[i] = a;
 i++;
 }

 if (SearchIndexOf(ReceivedData, "\n") != -1)
 {
 break;
 }
 }

 ProcessCommand(ReceivedData);

 i = 0;
 memset(ReceivedData, 0, sizeof(ReceivedData));
 }
 else
 {
 a = UARTCharGet(UART1_BASE);
 if (a == '\0')
 continue;
 ReceivedData[i] = a;
 i++;
 }
 }
 }

 return true;
 }*/

/*char *Substring(char *src, char *dst, int start, int stop)
 {
 int len = stop - start;
 strncpy(dst, src + start, len);

 return dst;
 }

 int SearchIndexOf(char src[], char str[])
 {
 int i, j, firstOcc;
 i = 0, j = 0;

 while (src[i] != '\0')
 {

 while (src[i] != str[0] && src[i] != '\0')
 i++;

 if (src[i] == '\0')
 return (-1);

 firstOcc = i;

 while (src[i] == str[j] && src[i] != '\0' && str[j] != '\0')
 {
 i++;
 j++;
 }

 if (str[j] == '\0')
 return (firstOcc);
 if (src[i] == '\0')
 return (-1);

 i = firstOcc + 1;
 j = 0;
 }

 return (-1);
 }

 char* itoa(int i, char b[])
 {
 char const digit[] = "0123456789";
 char* p = b;
 if (i < 0)
 {
 *p++ = '-';
 i *= -1;
 }
 int shifter = i;
 do
 {
 ++p;
 shifter = shifter / 10;
 }
 while (shifter);
 *p = '\0';
 do
 {
 *--p = digit[i % 10];
 i = i / 10;
 }
 while (i);
 return b;
 }

 void ftoa(float f, char *buf)
 {
 int pos = 0, ix, dp, num;
 if (f < 0)
 {
 buf[pos++] = '-';
 f = -f;
 }
 dp = 0;
 while (f >= 10.0)
 {
 f = f / 10.0;
 dp++;
 }
 for (ix = 1; ix < 8; ix++)
 {
 num = f;
 f = f - num;
 if (num > 9)
 buf[pos++] = '#';
 else
 buf[pos++] = '0' + num;
 if (dp == 0)
 buf[pos++] = '.';
 f = f * 10.0;
 dp--;
 }
 }*/
