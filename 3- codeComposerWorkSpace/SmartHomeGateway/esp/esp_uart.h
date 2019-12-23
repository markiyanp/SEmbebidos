/*
 * esp_uart.h
 *
 *  Created on: 10 дек. 2019 г.
 *      Author: Admin
 */

#ifndef ESP_ESP_UART_H_
#define ESP_ESP_UART_H_

bool makeRequest(char *request, char *response, int numReadLines,
        int responseTimeout);
void SendCharSequence(char *cmd);
//bool recvFindAndFilter(char *target, char *begin, char* end, char *data, int timeout);
#endif /* ESP_ESP_UART_H_ */
