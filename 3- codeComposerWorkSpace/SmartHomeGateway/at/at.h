#ifndef AT_AT_H_
#define AT_AT_H_

bool ATesp(void); //AT
bool RSTesp(void); //RESET
bool CWMODEesp(void); //MODE
bool CWJAPesp(void); //JOIN AP
bool CWQAPesp(void);
bool CIPMUXesp(void);
bool ATGMResp(char *version);
bool aCWMODEesp(char *list);
bool aCWLAPesp(char *list);
bool aCIFSResp(char *list);
bool CIPSTOesp(void);
bool CIPSTARTesp(void);
bool CIPCLOSEesp(void);
void CIPSENDesp(int length);
bool CIPMODEEesp(void);
void SendATCommand(char *cmd);

#endif /* AT_AT_H_ */
