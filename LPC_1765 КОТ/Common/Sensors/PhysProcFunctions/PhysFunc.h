#include "Config.h"
#if(_SENSORS_)

#ifndef PHYSFUNC_H
#define PHYSFUNC_H


#define LENGTH_PHYS_FUNC_BUF     50
#define MAX_PHYS_FUNC_NAME       10




typedef struct tagPhysFuncList
{
    uint8_t NumberOfItems;
    struct tagPhysFunc *pFirstFunc;
}PHYS_FUNC_LIST;

typedef struct tagPhysFunc
{
    SENSOR_PROC_FUNC * Addr;
    INIT_PROC *Init_Proc;
    char Name[MAX_PHYS_FUNC_NAME];
    struct tagPhysFunc *pNext;
    PHYS_FUNC_LIST *pFuncList;
}PHYS_FUNC;


void InitPhysFunctions(void);
SENSOR_PROC_FUNC * GetPhysFuncAddr(char *name);
INIT_PROC * GetInitPhysFuncAddr(char *name);
void GetPhysFuncName(SENSOR_PROC_FUNC addr, char *name);

#endif
#endif





