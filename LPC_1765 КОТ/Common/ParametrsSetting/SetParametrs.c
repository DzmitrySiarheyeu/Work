#include "config.h"
#include "SetParametrs.h"

char *global_setting_filename = NULL;
int CurTopOffset = 0;

int ReadSettingsFile(char *name)
{
	global_setting_filename = name;
	CurTopOffset = 0;
	return 0;
}

char * GetPointToElement(char *top, char *element)
{
	static char LocalBuffer[128];
	int len = ini_gets(top, element, "", LocalBuffer, sizeof(LocalBuffer), global_setting_filename);
	if(len == 0)
		return NULL;
	return LocalBuffer;
}

int SetParameter(char *name, char *top, char *element, char *value)
{
	if(ini_puts(top, element, value, name))
		return 0;
	return -1;
}

int GetCurrentTop(char *name, uint16_t size)
{
	int rez;
	rez = ini_getsection(CurTopOffset, name, size, global_setting_filename);
	CurTopOffset++;
	if(rez > 0)
		return 0;
	return -1;
}

int DeleteTop(char *name, char *top)
{
	if(ini_puts(top, NULL, NULL, name))
		return 0;
	return -1;
}
