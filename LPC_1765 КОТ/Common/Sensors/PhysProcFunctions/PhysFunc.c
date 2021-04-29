#include "Config.h"
#if(_SENSORS_)
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "SensorCore.h"
#include "PhysFunc.h"
#include "main.h"
#include "math.h"
#include "User_Error.h"


PHYS_FUNC_LIST physfunc_list;

void PutValueToOutBuf(SENSOR_INFO *pSensInfo, float data);


#if _APROC_FUNC_
    static float Aproximat(SENSOR_HANDLE handle);
    static int InitAproximation(SENSOR_HANDLE handle, uint8_t *buff, uint16_t count);
    PHYS_FUNC Aproc = {.Addr = (SENSOR_PROC_FUNC *)&Aproximat, .Init_Proc = (INIT_PROC *)&InitAproximation, .Name = "Aproc", .pNext = NULL, .pFuncList = NULL};
#endif

#if _LAPROC_FUNC_
    static float LinearAproximat(SENSOR_HANDLE handle);
    static int InitLineAproximation(SENSOR_HANDLE handle, uint8_t *buff, uint16_t count);
    PHYS_FUNC LAproc = {.Addr = (SENSOR_PROC_FUNC *)&LinearAproximat, .Init_Proc = (INIT_PROC *)&InitLineAproximation, .Name = "LAproc", .pNext = NULL, .pFuncList = NULL};
#endif

#if _NULL_FUNC_
static float NullFunc(SENSOR_HANDLE handle);
static int InitNullFunc(SENSOR_HANDLE handle, uint8_t *buff, uint16_t count);
PHYS_FUNC Nullproc = {.Addr = (SENSOR_PROC_FUNC *)&NullFunc, .Init_Proc = (INIT_PROC *)&InitNullFunc, .Name = "NProc", .pNext = NULL, .pFuncList = NULL};
#endif

#if _TANK_FUNC_
static float TankFunc(SENSOR_HANDLE handle);
static int InitTankFunc(SENSOR_HANDLE handle, uint8_t *buff, uint16_t count);
PHYS_FUNC Tankproc = {.Addr = (SENSOR_PROC_FUNC *)&TankFunc, .Init_Proc = (INIT_PROC *)&InitTankFunc, .Name = "TProc", .pNext = NULL, .pFuncList = NULL};
#endif


#if _APROC_FUNC_
float Aproximat(SENSOR_HANDLE handle)
{
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatPhysProc);

    uint16_t i, k;
    float temp;
    float x, x1, x2, y1, y2, top_level, bottom_level;


    bottom_level = ((float *)(pProcInfo->Data))[0];
    top_level = ((float *)(pProcInfo->Data))[1];

    i = pProcInfo->Flag - 4;
    x = pSensInfo->Data;

	if(!IS_QNaN(x))
	{
	    if(x < bottom_level) x = bottom_level;
	    if(x > top_level) x = top_level;
	
	    while((x < ((float *)(pProcInfo->Data))[i]) && i > 2) i = i - 2;
	
	    k = i + 1;
	
	    x1 = ((float *)(pProcInfo->Data))[i];
	    x2 = ((float *)(pProcInfo->Data))[i + 2];
	
	    y1 = ((float *)(pProcInfo->Data))[k];
	    y2 = ((float *)(pProcInfo->Data))[k + 2];
	
	    temp  = (y2 - y1) * (x - x1);
	    temp /= (x2 - x1);
	    temp += (y1);
	}
	else SET_QNaN(&temp); 
		

    PutValueToOutBuf(pSensInfo, temp);
    return temp;
	   
}
#endif // _APROC_FUNC_


#if _LAPROC_FUNC_
float LinearAproximat(SENSOR_HANDLE handle)
{
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatPhysProc);
    float top_level, bottom_level;

    bottom_level = ((float *)(pProcInfo->Data))[0];
    top_level = ((float *)(pProcInfo->Data))[1];

    float temp = pSensInfo->Data;

	if(!IS_QNaN(temp))
	{

    	if(temp < bottom_level) temp = bottom_level;
    	if(temp > top_level) temp = top_level;

    	temp = temp * ((float *)(pProcInfo->Data))[2] + ((float *)(pProcInfo->Data))[3];
	}
    PutValueToOutBuf(pSensInfo, temp);

    return temp;
}

#endif  //  _LAPROC_FUNC_


#if _NULL_FUNC_
float NullFunc(SENSOR_HANDLE handle)
{
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
//    PROC_INFO *pProcInfo;
  //  pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatPhysProc);
    
    PutValueToOutBuf(pSensInfo, pSensInfo->Data);

    return pSensInfo->Data;
}
#endif  //   _NULL_FUNC_


#if _TANK_FUNC_

#define DELTA_H_DOT     0.005
float TankFunc(SENSOR_HANDLE handle)
{
    float F_min = 0, F_max = 0, H_konus = 0, H_dot = 0, R_tank = 0, L_tank = 0, H_flanc = 0, H_max_sens = 0;

    float h_dot_fuel = 0, temp_1 = 0, temp_2 = 0, Freq = 0, h_fuel = 0;

    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatPhysProc);

    F_min = ((float *)(pProcInfo->Data))[0];
    F_max = ((float *)(pProcInfo->Data))[1];
    H_dot = ((float *)(pProcInfo->Data))[2];
    R_tank = ((float *)(pProcInfo->Data))[3] / 2;
    L_tank = ((float *)(pProcInfo->Data))[4];
    H_flanc = ((float *)(pProcInfo->Data))[5];
    H_konus = ((float *)(pProcInfo->Data))[6];

    H_max_sens = H_dot - 0.01;

    Freq = pSensInfo->Data;  //  текущая частота

    if(Freq < F_min) Freq = F_min;
    if(Freq > F_max) Freq = F_max;

    h_dot_fuel = ((Freq - F_min) / (F_max - F_min)) * H_dot;  // уровень погружения датчика

    temp_1 = R_tank * 2 + H_flanc - (H_max_sens - h_dot_fuel);
    if(temp_1 > R_tank * 2)
    {
        h_fuel = R_tank * 2;
    }
    else
    {
        h_fuel = R_tank * 2 + H_flanc - (H_max_sens - h_dot_fuel + DELTA_H_DOT);
    }

    temp_1 = sqrtf((2 * R_tank * h_fuel) - (h_fuel * h_fuel));
    temp_1 = temp_1 * (R_tank - h_fuel);

    temp_2 = acosf((R_tank - h_fuel) / R_tank);
    temp_2 = temp_2 * R_tank * R_tank;

    temp_1 = temp_2 - temp_1;

    temp_1 = (temp_1 * (L_tank + H_konus / 3));

    PutValueToOutBuf(pSensInfo, temp_1);

    return temp_1;

}


int InitTankFunc(SENSOR_HANDLE handle, uint8_t *data, uint16_t count)
{
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatPhysProc);
    float *p;
    int len = 0, k = 0;
    uint8_t buf[11];
    uint8_t *ch;
    count = 0;
    float *pBuf;


    pBuf = (float *)pvPortMalloc((size_t)7*4); //  Память для хранения 62 значений 
    if( pBuf == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }

    ch = data;

    do
    {
        while(ch[len] != ';' && len < 11) 
        {
            len++;
            k++;
        }
        if(len >= 11)
        {
            AddError(ERR_INCORRECT_SENS_FILE_STR);
            return -1;
        }
        memcpy((uint8_t *)&buf, (uint8_t *)&data[k - len], len);
        buf[len] = 0;
        pBuf[count] = atof((uint8_t *)buf);
        len = 0;
        count++;
        ch = (uint8_t *)&data[++k];
    }
    while(data[k] != 0 && count < 7);


    if(count != 7)   // ВЫДАТЬ ОШИБКУ
    {
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }


    p = pvPortMalloc((size_t)count*4);
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }

    memcpy(p, pBuf, count*4);
    pProcInfo->Data = (void *)p;
	pProcInfo->Buf = NULL;

    pProcInfo->Flag = count;
    vPortFree(pBuf);
    return 0;
}
#endif //  _TANK_FUNC_


#if _NULL_FUNC_
int InitNullFunc(SENSOR_HANDLE handle, uint8_t *buff, uint16_t count)
{
	SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatPhysProc);

	pProcInfo->Data = NULL;
	pProcInfo->Buf = NULL;
    return 0;
}
#endif  //  _NULL_FUNC_


#if _APROC_FUNC_
int InitAproximation(SENSOR_HANDLE handle, uint8_t *data, uint16_t count)
{
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatPhysProc);
//    float *p;
    int len = 0, k = 0, c = 0;
    uint8_t buf[11];
    uint8_t *ch;
    count = 0;
    float *pBuf;
	count = 0;


//    pBuf = (float *)pvPortMalloc((size_t)62*4); //  Память для хранения 62 значений 
//    if( pBuf == NULL )
//    {
//        AddError(ERR_MEM_SENS_FILE_STR);
//        return -1;
//    }

    ch = data;

    do
    {
        while(ch[len] != ';' && len < 11) 
        {
            len++;
            k++;
        }
        if(len >= 11)
        {
            AddError(ERR_INCORRECT_SENS_FILE_STR);
            return -1;
        }
        memcpy((uint8_t *)&buf, (uint8_t *)&data[k - len], len);
        buf[len] = 0;
		if(count == 0)
		{
			c = atoi((char *)buf);
			if(c > 32) return -1;
			c = c * 2 + 2;
			pBuf = (float *)pvPortMalloc((size_t)c*4); //  Память для хранения 62 значений 
		    if( pBuf == NULL )
		    {
		        AddError(ERR_MEM_SENS_FILE_STR);
		        return -1;
		    }
		}
		else
        	pBuf[count - 1] = atof((char *)buf);
        len = 0;
        count++;
        ch = (uint8_t *)&data[++k];
    }
    while(data[k] != 0 && count <= c);


    if(count >= 67 || count < 7)   // ВЫДАТЬ ОШИБКУ
    {
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }


    /*p = pvPortMalloc((size_t)count*4);
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }  

    memcpy(p, pBuf, count*4);*/
    pProcInfo->Data = (void *)pBuf;
	pProcInfo->Buf = NULL;

    pProcInfo->Flag = count - 1;
    //vPortFree(pBuf);
    return 0;
}
#endif  //  _APROC_FUNC_


#if _LAPROC_FUNC_
int InitLineAproximation(SENSOR_HANDLE handle, uint8_t *data, uint16_t count)
{
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatPhysProc);
    float *p;
    int len = 0, k = 0;
    uint8_t buf[11];
    uint8_t *ch;
    count = 0;
    float pBuf[4];


    //pBuf = (float *)pvPortMalloc((size_t)4*4); //  Память для хранения 62 значений 
    //if( pBuf == NULL )
    //{
    //    critical_error(ERR_MEM_SENS_FILE_STR);
    //    return -1;
    //}

    ch = data;

    do
    {
        while(ch[len] != ';' && len < 11) 
        {
            len++;
            k++;
        }
        if(len >= 11)
        {
            AddError(ERR_INCORRECT_SENS_FILE_STR);
            return -1;
        }
        memcpy((uint8_t *)&buf, (uint8_t *)&data[k - len], len);
        buf[len] = 0;
        pBuf[count] = atof((char *)buf);
        len = 0;
        count++;
        ch = (uint8_t *)&data[++k];
    }
    while(data[k] != 0 && count < 4);


    if(count != 4)   // ВЫДАТЬ ОШИБКУ
    {
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }


    p = pvPortMalloc((size_t)count*4);
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }

    memcpy(p, (uint8_t *)pBuf, count*4);
    pProcInfo->Data = (void *)p;
	pProcInfo->Buf = 0;
    pProcInfo->Flag = count;

    //vPortFree(pBuf);
    return 0;
}

#endif  //  _LAPROC_FUNC_




void GetPhysFuncName(SENSOR_PROC_FUNC addr, char *name)
{
    PHYS_FUNC *pFunc;
    pFunc = physfunc_list.pFirstFunc;
    while(pFunc != NULL)
    {
		if(pFunc->Addr == (SENSOR_PROC_FUNC *)addr){
			strcpy(name, (char *)pFunc->Name);
			return;
		}

        pFunc = pFunc->pNext;
    }
}


SENSOR_PROC_FUNC * GetPhysFuncAddr(char *name)
{
    PHYS_FUNC *pFunc;
    pFunc = physfunc_list.pFirstFunc;
    while(pFunc != NULL)
    {
        if(strcmp((char *)pFunc->Name, (char *)name) == 0)
        {
            return pFunc->Addr;
        }
        pFunc = pFunc->pNext;
    }
    return NULL;
}


INIT_PROC * GetInitPhysFuncAddr(char *name)
{
    PHYS_FUNC *pFunc;
    pFunc = physfunc_list.pFirstFunc;
    while(pFunc != NULL)
    {
        if(strcmp((char *)pFunc->Name, (char *)name) == 0)
        {
            return pFunc->Init_Proc;
        }
        pFunc = pFunc->pNext;
    }
    return NULL;
}

uint8_t AddPhysFun(PHYS_FUNC *pFunc)
{
    physfunc_list.NumberOfItems++;
    pFunc->pFuncList = &physfunc_list;
    if(physfunc_list.pFirstFunc == NULL)
    {
        physfunc_list.pFirstFunc = pFunc;
        pFunc->pNext = NULL;
    }
    else
    {
        pFunc->pNext = physfunc_list.pFirstFunc;
        physfunc_list.pFirstFunc = pFunc;
    }
    return 0;
}

void InitPhysFunctions(void)
{
#if _APROC_FUNC_
   AddPhysFun(&Aproc);
#endif

#if _LAPROC_FUNC_
   AddPhysFun(&LAproc);
#endif

#if _NULL_FUNC_
   AddPhysFun(&Nullproc);
#endif

#if _TANK_FUNC_
   AddPhysFun(&Tankproc);
#endif
}


void PutValueToOutBuf(SENSOR_INFO *pSensInfo, float data)
{
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatPhysProc);

    uint16_t k;
    float temp;
    for(k = pProcInfo->Depth; k > 0; k--)
    {
        if((k-2) >= 0)
        { 
            temp = ((float *)(pProcInfo->Buf))[k-2];
            ((float *)(pProcInfo->Buf))[k-1] = temp;
        }
    }
    ((float *)(pProcInfo->Buf))[0] = data;
}

//  проверка памяти
/*
uint16_t CheckFreeMemory()
{
    uint16_t need_memory;
    uint8_t  multiplier;
    void  *p;

    for(need_memory = 3000; need_memory > 0; need_memory -= 50)
    {
        p = pvPortMalloc((size_t)EXP_BUF);
        if(p)
        {
            while(p)
        }
    }
}
*/
#endif





