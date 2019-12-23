#ifndef ESP_LOGIC_TASK_H_
#define ESP_LOGIC_TASK_H_

uint32_t EspLogicTaskInit(void);
void SendATCommand(char *cmd);
int indexOfMessageEnd(char *source, char *resp);
void clearReceiveQueue();

#endif /* ESP_LOGIC_TASK_H_ */
