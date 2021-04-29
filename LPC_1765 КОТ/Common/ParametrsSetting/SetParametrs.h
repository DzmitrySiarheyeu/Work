#ifndef SETPARAMETRS_H
#define SETPARAMETRS_H

#include "minIni.h"

//extern int ChangeParametr(uint8_t *name, uint8_t *top, uint8_t *element, uint8_t *value, uint8_t size);
int SetParameter(char *name, char *top, char *element, char *value);
int DeleteTop(char *name, char *top);

int ReadSettingsFile(char *name);
char * GetPointToElement(char *top, char *element);
static inline uint8_t GetSettingsFileStatus(void){ return 1; }
int GetCurrentTop(char *name, uint16_t size);
#define ParametrsMemFree()


#endif


