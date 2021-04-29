#include "Config.h"
#if _SENSORS_

#include <stdint.h>
#include "SensorCore.h"
//#include "discrete/discrete.h"
//#include "Freq/Freq_lm.h"
#include "DriversCore.h"
#include "DataFunc.h"
#include "main.h"
#include "User_Error.h"
#include "Events.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

DATA_FUNC_LIST datafunc_list;
void save_counters(uint8_t ch);

#if _BUTTON_FUNC_
    extern uint8_t Key;
    //extern xQueueHandle user_message_queue;
	extern xQueueHandle events_queue;
#endif


float compute(float value_1 , uint8_t operat, float value_2);
float match_compute(float value_1, uint8_t *operat, float value_2);
uint8_t is_match_operat(uint8_t *data);
uint8_t is_sensor(uint8_t *data);
uint8_t is_operat(uint8_t *data);
uint8_t is_const(uint8_t *data);

int cmp_val(float sens_val, float cmp_val, uint8_t cmp_op);

#if _EXP_FUNC_
    static float ExpIntermediate(SENSOR_HANDLE handle);
	static float ExpExIntermediate(SENSOR_HANDLE handle);
    static int InitExpProc(SENSOR_HANDLE handle, char *buf, uint16_t count);
#endif

#if _NULL_DATA_FUNC_
    static float NullDataFunc(SENSOR_HANDLE handle);
    static int InitNullDataFunc(SENSOR_HANDLE handle, char *buf, uint16_t count);
#endif


#if _MB_FUNC_
    static float MBfunc(SENSOR_HANDLE handle);
    static int InitMBProc(SENSOR_HANDLE handle, char *buf, uint16_t count);
#endif

#if _IMP_FUNC_
    static float ImpFunc(SENSOR_HANDLE handle);
    static int InitImpProc(SENSOR_HANDLE handle, char *buf, uint16_t count);
#endif

//#if _ADC_EXP_FUNC_
//    static float ADCExpIntermediate(SENSOR_HANDLE handle);
//    static int InitADCExpIntermediate(SENSOR_HANDLE handle, uint8_t *buf, uint16_t count);
//#endif

#if _DESCRETE_FUNC_
    static float DiscreteFunc(SENSOR_HANDLE handle);
    static int InitDiscreteFunc(SENSOR_HANDLE handle, char *buf, uint16_t count);
#endif

#if _CLOCK_SENS_FUNC_
    static float ClockSensFunc(SENSOR_HANDLE handle);
    static int InitClSensFunc(SENSOR_HANDLE handle, char *data, uint16_t count);
#endif

#if _VIRT_SENS_FUNC_
    static int InitVirSensFunc(SENSOR_HANDLE handle, char *data, uint16_t count);
    static float VirtualSensFunc(SENSOR_HANDLE handle);
#endif

#if _SLOW_FREC_FUNC_
    static float SlowFreqFunc(SENSOR_HANDLE handle);
    static int InitSlowFreqProc(SENSOR_HANDLE handle, char *data, uint16_t count);
#endif

#if _BUTTON_FUNC_
    static float ButtonFunc(SENSOR_HANDLE handle);
    static int InitButtonFunc(SENSOR_HANDLE handle, char *data, uint16_t count);
#endif

#if _AVERAGE_FUNC_
    static float AverageFunc(SENSOR_HANDLE handle);
    static int InitAverageFunc(SENSOR_HANDLE handle, char *data, uint16_t count);
#endif

#if _MB_QUAL_FUNC_
    static float MBQualFunc(SENSOR_HANDLE handle);
    static int InitMBQualFunc(SENSOR_HANDLE handle, char *data, uint16_t count);
#endif

#if _OMNI_SNIFFER_FUNC_
	static float OMNSFunc(SENSOR_HANDLE handle);
    static int InitOMNSFunc(SENSOR_HANDLE handle, char *data, uint16_t count);
#endif	

//float SlidingMean(SENSOR_HANDLE handle);
//int InitMeanProc(SENSOR_HANDLE handle, uint8_t *buf, uint16_t count);

//#if _EXP_FUNC_
//DATA_FUNC Exp = {.Addr = (SENSOR_PROC_FUNC *)&ExpIntermediate, .Init_Proc = (INIT_PROC *)&InitExpProc, .Name = "Exp", .pNext = NULL, .pFuncList = NULL};
//
//float ExpIntermediate(SENSOR_HANDLE handle)
//{
//    float Alpha = 0;
//    float CurenValue = 0, PreviousValue = 0;
//    int count = 0;
//
//    SENSOR *pSensor;
//    pSensor = (SENSOR *)handle;
//    SENSOR_INFO *pSensInfo;
//    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
//    PROC_INFO *pProcInfo;
//    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);
//
//    count = ReadDevice(pSensor->SensorDevise, (void *)&CurenValue, 1);
//    if(count == 0)
//    {
//        return (float)-1;
//    }
//
//    PreviousValue = *((float *)pProcInfo->Buf);
//
//    Alpha = *((float *)pProcInfo->Data);
//
//    CurenValue = PreviousValue +  Alpha * (CurenValue - PreviousValue);
//
//    *((float *)pProcInfo->Buf) = CurenValue;
//
//    return CurenValue;
//}
//
//int InitExpProc(SENSOR_HANDLE handle, uint8_t *data, uint16_t count)
//{
//    SENSOR_INFO *pSensInfo;
//    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
//    PROC_INFO *pProcInfo;
//    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);
//    float *p;
//    int len = 0, k = 0;
//    uint8_t buf[11];
//    uint8_t *ch;
//    count = 0;
//
//    p = (float *)pvPortMalloc((size_t)1*4); //  Память для хранения 4 значений 
//    if( p == NULL )
//    {
//        AddError(ERR_MEM_SENS_FILE_STR);
//        return -1;
//    }
//
//    ch = data;
//
//    do
//    {
//        while(ch[len] != ';' && len < 11) 
//        {
//            len++;
//            k++;
//        }
//        if(len >= 11)
//        {
//            AddError(ERR_INCORRECT_SENS_FILE_STR);
//            return -1;
//        }
//        memcpy((uint8_t *)&buf, (uint8_t *)&data[k - len], len);
//        buf[len] = 0;
//        p[count] = (float)atof((uint8_t *)buf);
//        len = 0;
//        count++;
//        ch = (uint8_t *)&data[++k];
//    }
//    while(data[k] != 0 && count < 1);
//
//    if(count != 1)   // ВЫДАТЬ ОШИБКУ
//    {
//        AddError(ERR_INCORRECT_SENS_FILE_STR);
//        return -1;
//    }
//
//    pProcInfo->Data = (void *)p;
//
//    p = pvPortMalloc((size_t)EXP_BUF);  //  Память для хранения прошлого значения
//    if( p == NULL )
//    {
//        AddError(ERR_MEM_SENS_FILE_STR);
//        return -1;
//    }
//    memset((uint8_t *)p, 0, EXP_BUF);
//    pProcInfo->Buf = (void *)p;
//
//    return 0;
//}
//#endif // _EXP_FUNC



#if _AVERAGE_FUNC_
DATA_FUNC Average = {.Addr = (SENSOR_PROC_FUNC *)&AverageFunc, .Init_Proc = (INIT_PROC *)&InitAverageFunc, .Name = "Average", .pNext = NULL, .pFuncList = NULL};

#define		 	AV_DEV_CH		0
#define		 	AV_BUF_SIZE		2
#define			AV_COUNT		4
#define			AV_FULL_FLAG	6

float AverageFunc(SENSOR_HANDLE handle)
{
    uint8_t *p_data = NULL;
	float *p_buf = NULL;

	int i = 0;
	int count = 0;

	float  CurenValue = 0, val = 0;
	float err = 0;
	SET_SNaN(&err);

    SENSOR *pSensor;
    pSensor = (SENSOR *)handle;
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

    p_data = (uint8_t *)pProcInfo->Data;
	p_buf = (float *)pProcInfo->Buf;

    count = SeekDevice(pSensor->SensorDevise, p_data[AV_DEV_CH], 0);
    if(count == -1) 
    {
        return err;
    }

    count = ReadDevice(pSensor->SensorDevise, (void *)&CurenValue, 1);
    if(count == 0)
    {
        return err;
    }

	if(IS_QNaN(CurenValue)) return err;

	p_buf[(*((uint16_t *)&p_data[AV_COUNT]))++] = CurenValue;
	if(*((uint16_t *)&p_data[AV_COUNT]) > *((uint16_t *)&p_data[AV_BUF_SIZE]))
	{
		*((uint16_t *)&p_data[AV_COUNT]) = 0;
		p_data[AV_FULL_FLAG] = 1;
	}

		//  если буфер еще не заполнен усредняем имеющееся количество
	uint16_t count_lim = p_data[AV_FULL_FLAG] ? *((uint16_t *)&p_data[AV_BUF_SIZE]) : *((uint16_t *)&p_data[AV_COUNT]);

	for(i = 0; i < count_lim; i++)
	{
		val += p_buf[i];
	}

	val /= count_lim;   

    return val;
}

int InitAverageFunc(SENSOR_HANDLE handle, char *data, uint16_t count)
{
    uint8_t i = 0;
	uint16_t temp;

    SENSOR_INFO *pSensInfo;
     pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

    uint8_t *p;
    int len = 0, k = 0;
    uint8_t buf[11];
//    uint8_t *ch;
    count = 0;

    p = (uint8_t *)pvPortMalloc((size_t)7); //  Память для хранения 
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }


    do
    {
        while(data[len + i] != ';' && len < 11) 
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
        ((uint16_t *)p)[count] = (uint16_t)atoi((char *)buf);
		len = 0;
        count++;
        i = ++k;
    }
    while(data[k] != 0 && count < 2);

    if(count != 2)   // ВЫДАТЬ ОШИБКУ
    {
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }


    if(*((uint16_t *)&p[AV_DEV_CH]) > 4)
    {
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }

	temp = *((uint16_t *)&p[AV_BUF_SIZE])	/ 0.5;
	if(temp < 1) temp = 1;
	*((uint16_t *)&p[AV_BUF_SIZE]) = temp;

	*((uint16_t *)&p[AV_COUNT]) = 0;
	p[AV_FULL_FLAG] = 0;


    pProcInfo->Data = (void *)p;

    p = pvPortMalloc((size_t)temp*4);  //  Память для хранения прошлого значения
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }
    memset((uint8_t *)p, 0, temp*4);
    pProcInfo->Buf = (void *)p;

    return 0;
}
#endif //  AVERAGE



#if _NULL_DATA_FUNC_
DATA_FUNC NullData = {.Addr = (SENSOR_PROC_FUNC *)&NullDataFunc, .Init_Proc = (INIT_PROC *)&InitNullDataFunc, .Name = "D_Null", .pNext = NULL, .pFuncList = NULL};

float NullDataFunc(SENSOR_HANDLE handle)
{
	SENSOR *pSensor;
    pSensor = (SENSOR *)handle;
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

	float err = 0;
	SET_SNaN(&err);

	if(((uint8_t *)pProcInfo->Buf)[0] == 0)
	{
		 ((uint8_t *)pProcInfo->Buf)[0] = 1;
		 return err;
	}


    float CurenValue = 0;
    int count, len = 0;
    int ch = 0;
	uint8_t offset = 0;


    ch = ((int *)pProcInfo->Data)[0]; // может быть не номером канала, а ModBus id
	offset = ((int *)pProcInfo->Data)[1];
	len = ((int *)pProcInfo->Data)[2];  

    count = SeekDevice(pSensor->SensorDevise, ch, offset);
    if(count == -1) 
    {
        return err;
    }

	if(len > 4) len = 4;

    count = ReadDevice(pSensor->SensorDevise, (void *)&CurenValue, len);
    if(count == 0)
    {
        return err;
    }
	((uint8_t *)pProcInfo->Buf)[0] = 0;

    return CurenValue;
}

int InitNullDataFunc(SENSOR_HANDLE handle, char *data, uint16_t count)
{
    uint8_t i = 0;
	int temp = 0;

    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

    uint8_t *p;
    int len = 0, k = 0;
    uint8_t buf[11];
    count = 0;

    p = (uint8_t *)pvPortMalloc((size_t)3*4); //  Память для хранения 3 значений 
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }

    do
    {
        while(data[len + i] != ';' && len < 11) 
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

		switch(count)
		{
			case 0: ((int *)p)[0] = atoi((char *)buf); break;				// первое число - номер канала или ModBus id
			case 1:	*((int *)&p[count * 4]) = atoi((char *)buf); break;	// второе число - необязательный целочисленный параметр
			case 2:	*((int *)&p[count * 4]) = atoi((char *)buf); break;	// третье число - необязательный целочисленный параметр
		}

        len = 0;
        count++;
        i = ++k;
    }
    while(data[k] != 0 && count < 3);

    if(count != 1 && count != 2 && count != 3)   // ВЫДАТЬ ОШИБКУ
    {
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }

    temp = ((int *)p)[0];

    if(temp < 0)
    {
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }

	// если нашли только 2 числа
	if(count == 1){
		// обнуляем третье
		((int *)p)[1] = 0;
		((int *)p)[2] = 1;
	}
	// иначе проверяем его
	else if(count == 2){
		temp = ((int *)p)[1];	
		if(temp < 0)
    	{
        	AddError(ERR_INCORRECT_SENS_FILE_STR);
        	return -1;
    	}
		((int *)p)[2] = 1;
	}
	else 
	{
		temp = ((int *)p)[2];	
		if(temp < 0)
    	{
        	AddError(ERR_INCORRECT_SENS_FILE_STR);
        	return -1;
    	}
	}

    pProcInfo->Data = (void *)p;

    p = pvPortMalloc((size_t)1);  //  Память для хранения прошлого значения и двух флагов
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }

    pProcInfo->Buf = (void *)p;
	*((uint8_t *)pProcInfo->Buf) = 0;

    return 0;
}

#endif // _NULL_DATA_FUNC_



#if _EXP_FUNC_
DATA_FUNC Exp = {.Addr = (SENSOR_PROC_FUNC *)&ExpIntermediate, .Init_Proc = (INIT_PROC *)&InitExpProc, .Name = "Exp", .pNext = NULL, .pFuncList = NULL};


float ExpIntermediate(SENSOR_HANDLE handle)
{
    float Alpha, CurenValue, PreviousValue;
    int count;
    int ch = 0;
	uint8_t offset = 0;

    SENSOR *pSensor;
    pSensor = (SENSOR *)handle;
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

	float err = 0;
	SET_SNaN(&err);

    ch = ((int *)pProcInfo->Data)[0]; // может быть не номером канала, а ModBus id
	Alpha = ((float *)pProcInfo->Data)[1]; 
	offset = (uint8_t)((int *)pProcInfo->Data)[2]; // третий необязательный параметр

    count = SeekDevice(pSensor->SensorDevise, ch, offset);
    if(count == -1) 
    {
        return err;
    }

    count = ReadDevice(pSensor->SensorDevise, (void *)&CurenValue, 1);
    if(count == 0)
    {
        return err;
    }
	if(IS_QNaN(CurenValue)) return CurenValue;

    PreviousValue = *((float *)pProcInfo->Buf);
	if(PreviousValue == 0)
		PreviousValue = CurenValue;

    CurenValue = PreviousValue +  Alpha*(CurenValue - PreviousValue);

    *((float *)pProcInfo->Buf) = CurenValue;

    return CurenValue;
}

DATA_FUNC ExpEx = {.Addr = (SENSOR_PROC_FUNC *)&ExpExIntermediate, .Init_Proc = (INIT_PROC *)&InitExpProc, .Name = "ExpEx", .pNext = NULL, .pFuncList = NULL};
float ExpExIntermediate(SENSOR_HANDLE handle)
{
    float Alpha, CurenValue, PreviousValue;
    int count;
    int ch = 0;
	uint16_t len = 0;
	uint32_t data = 0;

    SENSOR *pSensor;
    pSensor = (SENSOR *)handle;
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

	float err = 0;
	SET_SNaN(&err);

	if(((uint8_t *)pProcInfo->Buf)[4] == 0)
	{
		 ((uint8_t *)pProcInfo->Buf)[4] = 1;
		 return err;
	}


    ch = ((int *)pProcInfo->Data)[0]; 
	Alpha = ((float *)pProcInfo->Data)[1]; 
	len = (uint8_t)((int *)pProcInfo->Data)[2]; 

	if(len > 4) len = 4;


    count = SeekDevice(pSensor->SensorDevise, ch, 0);
    if(count == -1) 
    {
        return err;
    }

    count = ReadDevice(pSensor->SensorDevise, (void *)&data, len);
    if(count == 0)
    {
        return err;
    }

	CurenValue = (float)data;
	
	if(IS_QNaN(CurenValue)) return CurenValue;

	if(((uint8_t *)pProcInfo->Buf)[5] == 0)	{PreviousValue = CurenValue; ((uint8_t *)pProcInfo->Buf)[5] = 1;}
    else	PreviousValue = *((float *)pProcInfo->Buf);

    CurenValue = PreviousValue +  Alpha*(CurenValue - PreviousValue);

    *((float *)pProcInfo->Buf) = CurenValue;

	((uint8_t *)pProcInfo->Buf)[4] = 0;	 // 

    return CurenValue;
}

int InitExpProc(SENSOR_HANDLE handle, char *data, uint16_t count)
{
    uint8_t i = 0;
	int temp = 0;

    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

    uint8_t *p;
    int len = 0, k = 0;
    uint8_t buf[11];
    count = 0;

    p = (uint8_t *)pvPortMalloc((size_t)3*4); //  Память для хранения 3 значений 
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }

    do
    {
        while(data[len + i] != ';' && len < 11) 
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

		switch(count)
		{
			case 0: ((int *)p)[0] = atoi((char *)buf); break;				// первое число - номер канала или ModBus id
			case 1:	*((float *)&p[count * 4]) = atof((char *)buf); break;	// второе число - коэффициент alpha
			case 2:	*((int *)&p[count * 4]) = atoi((char *)buf); break;		// третье число - необязательный целочисленный параметр
		}

        len = 0;
        count++;
        i = ++k;
    }
    while(data[k] != 0 && count < 3);

    if(count != 2 && count != 3)   // ВЫДАТЬ ОШИБКУ
    {
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }

    temp = ((int *)p)[0];

    if(temp < 0)
    {
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }

	// если нашли только 2 числа
	if(count == 2)
		// обнуляем третье
		((int *)p)[2] = 0;
	// иначе проверяем его
	else 
	{
		temp = ((int *)p)[3];	
		if(temp < 0)
    	{
        	AddError(ERR_INCORRECT_SENS_FILE_STR);
        	return -1;
    	}
	}

    pProcInfo->Data = (void *)p;

    p = pvPortMalloc((size_t)EXP_BUF + 2);  //  Память для хранения прошлого значения и двух флагов
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }

    memset((char *)p, 0, EXP_BUF + 2);
    pProcInfo->Buf = (void *)p;
	*((float *)pProcInfo->Buf) = 0.0;

    return 0;
}

#endif // _EXP_FUNC_



#if _MB_FUNC_
DATA_FUNC MBf = {.Addr = (SENSOR_PROC_FUNC *)&MBfunc, .Init_Proc = (INIT_PROC *)&InitMBProc, .Name = "MBf", .pNext = NULL, .pFuncList = NULL};


float MBfunc(SENSOR_HANDLE handle)
{
    int ch = 0;
	uint8_t offset = 0;
	uint16_t buf[2];
	int count;

    SENSOR *pSensor;
    pSensor = (SENSOR *)handle;
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

	float err = 0;
	SET_SNaN(&err);

    ch = ((int *)pProcInfo->Data)[0]; // может быть не номером канала, а ModBus id
	offset = (uint8_t)((int *)pProcInfo->Data)[1]; // третий необязательный параметр

    count = SeekDevice(pSensor->SensorDevise, ch, offset);
    if(count == -1) 
    {
        return err;
    }

    count = ReadDevice(pSensor->SensorDevise, (void *)buf, 1);
    if(count == 0)
    {
        return err;
    }


    return (float)buf[1];
}

int InitMBProc(SENSOR_HANDLE handle, char *data, uint16_t count)
{
    uint8_t i = 0;
	int temp = 0;

    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

    uint8_t *p;
    int len = 0, k = 0;
    uint8_t buf[11];
    count = 0;

    p = (uint8_t *)pvPortMalloc((size_t)3*4); //  Память для хранения 2 значений 
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }

    do
    {
        while(data[len + i] != ';' && len < 11) 
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

		((int *)p)[count] = atoi((char *)buf);
        len = 0;
        count++;
        i = ++k;
    }
    while(data[k] != 0 && count < 2);

    if(count != 2)   // ВЫДАТЬ ОШИБКУ
    {
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }

    temp = ((int *)p)[0];

    if(temp < 0)
    {
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }

    pProcInfo->Data = (void *)p;

    p = pvPortMalloc((size_t)EXP_BUF);  //  Память для хранения прошлого значения
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }

    memset((char *)p, 0, EXP_BUF);
    pProcInfo->Buf = (void *)p;
	*((float *)pProcInfo->Buf) = 0.0;

    return 0;
}

#endif // _MB_FUNC_

#if _DESCRETE_FUNC_
DATA_FUNC DFunc = {.Addr = (SENSOR_PROC_FUNC *)&DiscreteFunc, .Init_Proc = (INIT_PROC *)&InitDiscreteFunc, .Name = "DFunc", .pNext = NULL, .pFuncList = NULL};

float DiscreteFunc(SENSOR_HANDLE handle)
{
    uint8_t number_of_0;
	uint16_t  number_of_1, buf_number;
    uint8_t current_state = 0;
    uint16_t prev_value, counter_of_0, counter_of_1;
    int count;
    
    SENSOR *pSensor;
    pSensor = (SENSOR *)handle;
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

    buf_number = (uint8_t)((uint16_t *)pProcInfo->Data)[0];
    count = SeekDevice(pSensor->SensorDevise, buf_number, 0);

	float err = 0;
	SET_SNaN(&err);

    if(count == -1) 
    {
        return err;
    }

    count = ReadDevice(pSensor->SensorDevise, &current_state, 1);
    if(count == 0)
    {
        return err;
    }

    number_of_0 = ((uint16_t *)pProcInfo->Data)[1];
    number_of_1 = ((uint16_t *)pProcInfo->Data)[2];

    prev_value = ((uint16_t *)pProcInfo->Buf)[0];
    counter_of_0 = ((uint16_t *)pProcInfo->Buf)[1];
    counter_of_1 = ((uint16_t *)pProcInfo->Buf)[2];

    if( current_state == 0 ) 
    {
        counter_of_0 += 1;
        counter_of_1 = 0;
        if( counter_of_0 >= number_of_0 ) 
        {
            counter_of_0 = 0;
            prev_value = 0;
        }
    }
    else if( current_state == 1 ) 
    {
        counter_of_1 += 1;
        counter_of_0 = 0;
        if( counter_of_1 >= number_of_1 ) 
        {
            counter_of_1 = 0;
            prev_value = 1;
        }
    }

    ((uint16_t *)pProcInfo->Buf)[0] = prev_value;
    ((uint16_t *)pProcInfo->Buf)[1] = counter_of_0;
    ((uint16_t *)pProcInfo->Buf)[2] = counter_of_1;

    return (float)prev_value;
}

int InitDiscreteFunc(SENSOR_HANDLE handle, char *data, uint16_t count)
{
//    uint8_t temp;
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);
    uint16_t *p;

    int len = 0, k = 0;
    uint8_t buf[11];
    char *ch;
    count = 0;

    p = (uint16_t *)pvPortMalloc((size_t)DISCRETE_DATA);  //   Память для хранения номера входа, кол-ва нулей, кол-ва единиц
    if( p == NULL )
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
        if(count == 0)p[count] = (uint16_t)atoi((char *)buf);
        else p[count] = ((uint16_t)atoi((char *)buf));
        len = 0;
        count++;
        ch = &data[++k];
    }
    while(data[k] != 0 && count < 3);

    if(count != 3)   // ВЫДАТЬ ОШИБКУ
    {
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }

    if( ((uint16_t *)p)[0] > 4)               // ВЫДАТЬ ОШИБКУ
    {
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }

    pProcInfo->Data = (void *)p;

    p = pvPortMalloc((size_t)DISCRETE_BUF);  //  Память для хранения прошлого значения, счетчика нулей и единиц
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }
    memset((uint16_t *)p, 0, DISCRETE_BUF);
    pProcInfo->Buf = (void *)p;


    return 0;
}

#endif //_DESCRETE_FUNC_

/*
DATA_FUNC Mean = {.Addr = (SENSOR_PROC_FUNC *)&SlidingMean, .Init_Proc = (INIT_PROC *)&InitMeanProc, .Name = "Mean", .pNext = NULL, .pFuncList = NULL};

float SlidingMean(SENSOR_HANDLE handle)
{
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);
    MEANINFO *pMeanInfo;
    pMeanInfo = (MEANINFO *)(pProcInfo->Buf);

    uint16_t size, i;
    float DataValue = 0;
    
    pMeanInfo->array[pMeanInfo->in++] = pMeanInfo->CurrentValue;

    if(pMeanInfo->in == pProcInfo->Depth)
    {
        pMeanInfo->in = 0; 
        if(pProcInfo->Flag == 0) pProcInfo->Flag = 1;
    }

    if(pProcInfo->Flag == 0)
    {
        size = pMeanInfo->in;
    }
    else
    {
        size = pProcInfo->Depth;
    }

    for(i = 0; i < size; i++)
    {
        DataValue = DataValue + (float)(((uint32_t *)(pMeanInfo->array))[i]);
    }

    DataValue = DataValue/size;

    pMeanInfo->CurrentValue = 0;

    return DataValue;

}
*/

#if _IMP_FUNC_
DATA_FUNC Imp = {.Addr = (SENSOR_PROC_FUNC *)&ImpFunc, .Init_Proc = (INIT_PROC *)&InitImpProc, .Name = "Imp", .pNext = NULL, .pFuncList = NULL};

float ImpFunc(SENSOR_HANDLE handle)
{
    uint32_t CurenValue;
    int count;
    uint8_t ch = 0;
	IMP_COUNT_INFO *impCntInfo;

    SENSOR *pSensor;
    pSensor = (SENSOR *)handle;
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

	float err = 0;
	SET_SNaN(&err);

    ch = *((uint8_t *)pProcInfo->Data);

	impCntInfo = (IMP_COUNT_INFO *)pProcInfo->Buf;

	impCntInfo->save_time_counter++;   //  Наращиваем счетчик для отсчета времени для записи
	if(impCntInfo->save_time_counter >= SENS_SAVE_TIME)
	{
		if(impCntInfo->previous_counter != impCntInfo->current_counter)
		{
			impCntInfo->save_time_counter = 0;
			impCntInfo->previous_counter = impCntInfo->current_counter;
			save_counters(ch);
		}
	}

    count = SeekDevice(pSensor->SensorDevise, ch, 0);
    if(count == -1) 
        return err;
    count = ReadDevice(pSensor->SensorDevise, (uint32_t *)&CurenValue, 1);
    if(count == 0)
        return err;

	impCntInfo->current_counter = CurenValue;
	return (float)impCntInfo->current_counter;
}

int InitImpProc(SENSOR_HANDLE handle, char *buf, uint16_t count)
{
    SENSOR *pSensor;
    pSensor = (SENSOR *)handle;
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

    uint8_t buff[11];
//    int err = 0;
    uint8_t *p;
    int len = 0, k = 0;

    p = pvPortMalloc((size_t)4);  //   Память для хранения номера входа, кол-ва нулей, кол-ва единиц
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }


    while(buf[len] != ';' && len < 11) 
    {
         len++;
         k++;
    }
    if(len >= 11)
    {
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }
    memcpy((uint8_t *)&buff, (uint8_t *)&buf[k - len], len);
    buff[len] = 0;
    p[0] = (uint8_t )atoi((char *)buff);

    if(p[0] > 4)
    {
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }
    pProcInfo->Data = (void *)p;


    p = pvPortMalloc((size_t)12);  //  Память для хранения текущего значения, счетчика для сохранения во флэш и прошлого значения
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }
    memset((uint8_t *)p, 0, 12);	 
    pProcInfo->Buf = (void *)p;

//	SeekDevice(pSensor->SensorDevise, *((uint8_t *)pProcInfo->Data), 0);
//	ReadDevice(pSensor->SensorDevise, (uint8_t *)pProcInfo->Buf, 1);
																       
    DeviceIOCtl(pSensor->SensorDevise, SET_VALID_DATA, (uint8_t *)pProcInfo->Data);

    return 0;
}
#endif  //  _IMP_FUNC_


#if _CLOCK_SENS_FUNC_
#include "clock_drv.h"
DATA_FUNC CSens = {.Addr = (SENSOR_PROC_FUNC *)&ClockSensFunc, .Init_Proc = (INIT_PROC *)&InitClSensFunc, .Name = "ClSens", .pNext = NULL, .pFuncList = NULL};

struct Cl_Sens_Name
{
    uint8_t name[3];
};

void reset_sens_counters(uint8_t ch)
{

    //uint8_t i = 0;
    uint32_t temp_counter = 0;


    struct Cl_Sens_Name Sens[4] = {{.name = "C0"}, {.name = "C1"}, {.name = "C2"}, {.name = "C3"}};
    SENSOR_HANDLE Sensor;
    SENSOR_INFO *pSensInfo;
    PROC_INFO *pProcInfo;


    Sensor = OpenSensorProc((char *)Sens[ch].name);
    if(Sensor != NULL)
    {
        pSensInfo = (SENSOR_INFO*)(((SENSOR *)Sensor)->pSensorData);
        pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

        *((float *)((uint8_t *)pProcInfo->Buf + 5)) = 0;
    }

	DEVICE_HANDLE hSaver;
	hSaver = OpenDevice("SAVER");


    SeekDevice(hSaver, 0, TIME_1_SEG + ch);
    WriteDevice(hSaver, (void *)&temp_counter, 1);

	CloseDevice(hSaver);
}

// Сохраняем время отсчитанное датчикам времени
void save_cl_sens_counters(uint8_t num, float cuurent_value)
{
	DEVICE_HANDLE hSaver;
	hSaver = OpenDevice("SAVER");

    SeekDevice(hSaver, 0, num + TIME_1_SEG);
    WriteDevice(hSaver, (void *)&cuurent_value, 1);

	CloseDevice(hSaver);  
}


void init_cl_sens_counters(uint8_t num, float *cuurent_value)
{
    int err = 0;
	int i = 0;
	DEVICE_HANDLE hSaver;
	hSaver = OpenDevice("SAVER");

    SeekDevice(hSaver, 0, num + TIME_1_SEG);
    err = ReadDevice(hSaver, (void *)cuurent_value, 1);

	CloseDevice(hSaver); 
	//*cuurent_value = 0;

    if(err == -1)
    {
        *(float *)cuurent_value = 0;
		for(i = 0; i < 4; i++)
        	reset_sens_counters(i);
    }		
}


float ClockSensFunc(SENSOR_HANDLE handle)
{

    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

    uint8_t min_c, sec_c, sub;
	uint32_t dop_sec = 0;

	float err = 0, val = 0;
	SET_SNaN(&err);

	CL_COUNT_INFO *pInfo = (CL_COUNT_INFO *)pProcInfo->Buf;


	pInfo->save_counter++;  //  Наращиваем счетчик для отсчета времени для записи
	val = opz_calc((char *)pProcInfo->Data);
	
	//if(IS_QNaN(val))

    if(!IS_QNaN(val) && val == 1)   //  Присутствует ли заданное событие
    { 
        if(pInfo->not_first_call)  //  Если это не первый вызов данной функции
        {
			DEVICE_HANDLE  hClock = OpenDevice("Clock");

            SeekDevice(hClock, SEC_D, 0);
            ReadDevice(hClock, (void *)&sec_c, 1);

            SeekDevice(hClock, MIN_D, 0);
            ReadDevice(hClock, (void *)&min_c, 1);

			CloseDevice(hClock);

            sub = min_c - pInfo->min;
            if(sub)
            {
                if(sec_c >= pInfo->sec)
                {
                    dop_sec = sec_c - pInfo->sec + sub * 60;
                }
                else
                {
                    dop_sec = 60 - pInfo->sec + sec_c;
                }
            }
            else dop_sec = sec_c - pInfo->sec;

            if(dop_sec)
            {
				pInfo->hour_value += (float)dop_sec / 3600.0;
				if(pInfo->save_counter >= SENS_SAVE_TIME)
				{
					pInfo->save_counter = 0;
					save_cl_sens_counters(pInfo->ch_num, pInfo->hour_value);  //  Сохраняем новое значение в памяти
				}
                pInfo->sec = sec_c;
                pInfo->min = min_c;
                return pInfo->hour_value;
            }
            return err;
        }
        else
        {   
			DEVICE_HANDLE  hClock = OpenDevice("Clock");			

			//  Если Функция вызывается первый раз после инициализации
            SeekDevice(hClock, SEC_D, 0);
            ReadDevice(hClock, (void *)&sec_c, 1);

            SeekDevice(hClock, MIN_D, 0);
            ReadDevice(hClock, (void *)&min_c, 1);

			CloseDevice(hClock);

            pInfo->not_first_call = 1;
            pInfo->sec = sec_c;
        	pInfo->min = min_c;
            return pInfo->hour_value;
        }
    }
    pInfo->not_first_call = 0;
    if(pInfo->pool_flag)
    {
        pInfo->pool_flag = 0;
        return pInfo->hour_value;
    }
    else
    {
        pInfo->pool_flag = 1;
        return err;
    }
}

int InitClSensFunc(SENSOR_HANDLE handle, char *data, uint16_t count)
{
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);
	CL_COUNT_INFO *p = NULL;

	if(data[1] != ';')
	{
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }

	uint8_t len = strlen((char *)&data[2]);
	pProcInfo->Data = (void *)opz_form((char *)&data[2], len);
	if(pProcInfo->Data == NULL)
	{
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }

    

    p = (CL_COUNT_INFO *)pvPortMalloc((size_t)(sizeof(CL_COUNT_INFO) + len));  //  Память для хранения флага первого считывания, флага одного пула, секундб минут, прошлого значения и счетчика пулов датчика и номера канала
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }
    memset((uint8_t *)p, 0, sizeof(CL_COUNT_INFO));

	p->ch_num = (uint8_t)atoi((char *)data);

    init_cl_sens_counters(p->ch_num, &p->hour_value);
	strcpy(((char *)p + sizeof(CL_COUNT_INFO)), (char *)&data[2]);
    pProcInfo->Buf = (void *)p;

    return 0;
}
#endif //  _CLOCK_SENS_FUNC_



#if _VIRT_SENS_FUNC_
DATA_FUNC VSens = {.Addr = (SENSOR_PROC_FUNC *)&VirtualSensFunc, .Init_Proc = (INIT_PROC *)&InitVirSensFunc, .Name = "VSens", .pNext = NULL, .pFuncList = NULL};

float VirtualSensFunc(SENSOR_HANDLE handle)
{
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

    uint8_t temp_flag;

	float err = 0;
	SET_SNaN(&err);

    temp_flag = *((uint8_t *)(pProcInfo->Buf));
    if(temp_flag)
    {
		((uint8_t *)(pProcInfo->Buf))[0] = 0;
        return opz_calc((char *)pProcInfo->Data);
    }
    else
    {
        ((uint8_t *)(pProcInfo->Buf))[0] = 1;
        return err;
    }
}


int InitVirSensFunc(SENSOR_HANDLE handle, char *data, uint16_t count)
{
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);
	char *p = NULL;

    uint8_t len = strlen((char *)data);
    if(len > 64 || len < 4)
    {
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }


    pProcInfo->Data = (void *)opz_form((char *)data, len);
	if(pProcInfo->Data == NULL)
	{
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    } 

    p = pvPortMalloc((size_t)1 + len);  
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }
    memset((uint8_t *)p, 0, 1);
	strcpy((((char *)p) + 1), data);
    pProcInfo->Buf = (void *)p;

    return 0;

}
#endif //  _VIRT_SENS_FUNC_

/*
#if _SLOW_FREC_FUNC_
DATA_FUNC SlowFreq = {.Addr = (SENSOR_PROC_FUNC *)&SlowFreqFunc, .Init_Proc = (INIT_PROC *)&InitSlowFreqProc, .Name = "SFreq", .pNext = NULL, .pFuncList = NULL};

float SlowFreqFunc(SENSOR_HANDLE handle)
{
    float limit, freq;

    struct __attribute__ ((__packed__)) tagFreqInfo
    {
        float T_last;
        float T_between;
    }FreqInfo;

    int count;

    SENSOR *pSensor;
    pSensor = (SENSOR *)handle;
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

    uint8_t temp_flag = *((uint8_t *)(pProcInfo->Buf));
    if(temp_flag)
    {
        ((uint8_t *)(pProcInfo->Buf))[0] = 0;

        limit = ((float *)(pProcInfo->Data))[0];
        
        count = ReadDevice(pSensor->SensorDevise, (void *)&freq, 2);
    
        if(freq > 10)
        {
            return freq;
        }
        else
        {
            DeviceIOCtl(pSensor->SensorDevise, SLOW_FREQ_DATA, (uint8_t *)&FreqInfo);
    
            if(FreqInfo.T_last >= limit)
            {
                return 0;
            }
            else
            {
                if(FreqInfo.T_last > FreqInfo.T_between)
                {
                    return (float)(1/FreqInfo.T_last);
                }
                else
                {
                    return (float)(1/FreqInfo.T_between);
                }
            }
        }
    }
    else
    {
        ((uint8_t *)(pProcInfo->Buf))[0] = 1;
        return -1;
    }
}


int InitSlowFreqProc(SENSOR_HANDLE handle, uint8_t *data, uint16_t count)
{
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);
    float *p;
    int len = 0, k = 0;
    uint8_t buf[11];
    uint8_t *ch;
    count = 0;

    p = (float *)pvPortMalloc((size_t)1*4); //  Память для хранения 4 значений 
    if( p == NULL )
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
        p[count] = atof((uint8_t *)buf);
        len = 0;
        count++;
        ch = (uint8_t *)&data[++k];
    }
    while(data[k] != 0 && count < 1);

    if(count != 1)   // ВЫДАТЬ ОШИБКУ
    {
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }

    pProcInfo->Data = (void *)p;

    p = pvPortMalloc((size_t)1);  //  Память для хранения флага
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }
    memset((uint8_t *)p, 0, 1);
    pProcInfo->Buf = (void *)p;

    return 0;
}
#endif  //   _SLOW_FREC_FUNC_
*/

#if _BUTTON_FUNC_
DATA_FUNC BFunc = {.Addr = (SENSOR_PROC_FUNC *)&ButtonFunc, .Init_Proc = (INIT_PROC *)&InitButtonFunc, .Name = "BFunc", .pNext = NULL, .pFuncList = NULL};

float ButtonFunc(SENSOR_HANDLE handle)
{
    uint8_t number_of_0, number_of_1, buf_number;
    uint32_t current_state = 0;
    uint8_t prev_value, counter_of_0, counter_of_1, key_down, t_val = 0;
    int count;
    
    SENSOR *pSensor;
    pSensor = (SENSOR *)handle;
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

    EVENTS_MESSAGE message;
	float err = 0;
	SET_SNaN(&err);

    buf_number = *((uint8_t *)pProcInfo->Data);
    count = SeekDevice(pSensor->SensorDevise, buf_number, 0);
    if(count == -1) 
    {
        return err;
    }

    count = ReadDevice(pSensor->SensorDevise, (uint32_t *)&current_state, 1);
    if(count == 0)
    {
        return err;
    }

    number_of_0 = *((uint8_t *)pProcInfo->Data + 1);
    number_of_1 = *((uint8_t *)pProcInfo->Data + 2);

    t_val = prev_value = ((uint8_t *)pProcInfo->Buf)[0];
    counter_of_0 = ((uint8_t *)pProcInfo->Buf)[1];
    counter_of_1 = ((uint8_t *)pProcInfo->Buf)[2];
    key_down = ((uint8_t *)pProcInfo->Buf)[3];

    if( current_state == 0 ) 
    {
        counter_of_0 += 1;
        counter_of_1 = 0;
        if( counter_of_0 >= number_of_0 ) 
        {
            //counter_of_0 = 0;
            prev_value = 0;
            key_down = 1;
        }
        if( counter_of_0 == LONG_PRESS_INTERVAL )
        {
            Key = KEY_ON_2000ms;// Если кнопка удерживается более 2 с то определяем состояние как KEY_ON_200ms
            message.ID = BUTTON;
            message.data = Key;
            //xQueueSend(user_message_queue, ( void * )&message, pdFALSE);
			xQueueSend(events_queue, ( void * )&message, pdFALSE);

            //counter_of_0 = 0;
            key_down = 0;
        }
        if( counter_of_0 > LONG_PRESS_INTERVAL )
        {
            key_down = 0;
        }
    }
    else if( current_state == 1 ) 
    {
        counter_of_1 += 1;
        counter_of_0 = 0;
        if( counter_of_1 >= number_of_1 ) 
        {
            counter_of_1 = 0;
            if(prev_value == 0)
            {
                if(key_down)
                {
                    Key = KEY_ON_100ms; // Если кнопка удерживается более 100 мс то определяем состояние как KEY_ON_100ms
                    message.ID = BUTTON;
            		message.data = Key;				
                    xQueueSend(events_queue, ( void * )&message, pdFALSE);

                    key_down = 0;
                }
            }
            prev_value = 1;
        }
    }

    ((uint8_t *)pProcInfo->Buf)[0] = prev_value;
    ((uint8_t *)pProcInfo->Buf)[1] = counter_of_0;
    ((uint8_t *)pProcInfo->Buf)[2] = counter_of_1;
    ((uint8_t *)pProcInfo->Buf)[3] = key_down;

	if(t_val != prev_value)	return (float)prev_value;
	else return err;
}

int InitButtonFunc(SENSOR_HANDLE handle, char *data, uint16_t count)
{
   // uint8_t temp;
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);
    uint8_t *p;

    int len = 0, k = 0;
    uint8_t buf[11];
    char *ch;
    count = 0;

    p = pvPortMalloc((size_t)DISCRETE_DATA);  //   Память для хранения номера входа, кол-ва нулей, кол-ва единиц
    if( p == NULL )
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
        p[count] = (uint16_t)atoi((char *)buf);
        len = 0;
        count++;
        ch = &data[++k];
    }
    while(data[k] != 0 && count < 3);

    if(count != 3)   // ВЫДАТЬ ОШИБКУ
    {
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }

    if( ((uint8_t *)p)[0] > 4)               // ВЫДАТЬ ОШИБКУ
    {
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }

    pProcInfo->Data = (void *)p;

    p = pvPortMalloc((size_t)4);  //  Память для хранения прошлого значения, счетчика нулей и единиц
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }
    memset((uint8_t *)p, 0, 4);
    pProcInfo->Buf = (void *)p;


    return 0;
}

#endif  // _BUTTON_FUNC_

#if _MB_QUAL_FUNC_
DATA_FUNC QFunc = {.Addr = (SENSOR_PROC_FUNC *)&MBQualFunc, .Init_Proc = (INIT_PROC *)&InitMBQualFunc, .Name = "QFunc", .pNext = NULL, .pFuncList = NULL};

#define POOLS_FOR_INIT     (60*2) // 30 c
#define POOLS_FOR_1_MINUTE (60*4) // 60 с

extern uint8_t startInitFlag;      // флаг окончания инициализации

typedef struct 
{
	uint32_t txPackets;
	uint32_t rxPackets;
	uint32_t poolsCounter;
	uint32_t poolsInterval;
	float    Qval;
} __attribute__ ((__packed__)) QFuncCount;

static float MBQualFunc(SENSOR_HANDLE handle)
{
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

	QFuncCount *qFuncCounters = (QFuncCount *)pProcInfo->Buf;
	float err = 0;
	SET_SNaN(&err);
	// если все отконфигурировано нормально эта функция никогда не возвратит -1
	// и мы зациклимся на пуле датчиков, поэтому через раз она будет возвращать -1 с помощью заглушки
	static uint8_t zaglushka = 0xff;
	
	zaglushka ^= 0xff;
	if(zaglushka) 
		return err;

	// начитаем счет
	if(qFuncCounters->poolsCounter == 0)
	{
		DEVICE_HANDLE hMBS = OpenDevice("MBS");
		if(hMBS == 0)
			return qFuncCounters->Qval;

		// читаем значение счетчиков
		int txCount, rxCount;
		txCount = rxCount = (int)(*(uint8_t *)pProcInfo->Data); // id 

		DeviceIOCtl(hMBS, 2, (uint8_t *)&txCount);	// вместо id вернется значение счетчика 
		DeviceIOCtl(hMBS, 3, (uint8_t *)&rxCount);

		CloseDevice(hMBS);

		if(txCount == -1 || rxCount == - 1)
			return qFuncCounters->Qval;	

		// запоминаем
		qFuncCounters->txPackets = txCount;
		qFuncCounters->rxPackets = rxCount;

		qFuncCounters->poolsCounter = 1;
	}
	// досчитали до конца
	else if(qFuncCounters->poolsCounter == qFuncCounters->poolsInterval)
	{
		if(!startInitFlag)
		{
			qFuncCounters->poolsInterval = POOLS_FOR_1_MINUTE; // стандартный интервал проверки - 60 с
			startInitFlag = 1;                  // инициализация завершена
		}

		DEVICE_HANDLE hMBS = OpenDevice("MBS");
		if(hMBS == 0)
			return qFuncCounters->Qval;

		// читаем значение счетчиков
		int txCount, rxCount;
		txCount = rxCount = (int)(*(uint8_t *)pProcInfo->Data); // id 

		DeviceIOCtl(hMBS, 2, (uint8_t *)&txCount);	// вместо id вернется значение счетчика 
		DeviceIOCtl(hMBS, 3, (uint8_t *)&rxCount);

		CloseDevice(hMBS);

		if(txCount == -1 || rxCount == - 1)
			return qFuncCounters->Qval;

		// вычисляем разницу
		txCount = txCount - qFuncCounters->txPackets;
		rxCount = rxCount - qFuncCounters->rxPackets;

		// вычисляем коэффициент
		qFuncCounters->Qval = (float)rxCount/(float)txCount * 100.f;

		qFuncCounters->poolsCounter = 0;
	}
	else
		qFuncCounters->poolsCounter++;	

    return qFuncCounters->Qval;		
}

static int InitMBQualFunc(SENSOR_HANDLE handle, char *data, uint16_t count)
{ 
    uint8_t *p = pvPortMalloc((size_t)1);  //   Память для хранения id устройства
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }

	char idBuf[4] = {0,0,0,0}; // буфер для строки с id (не более 3 символов)
	int len = 0;   // длина строки

    while(data[len] != ';') 
    {
		if(len < 3)
			idBuf[len] = data[len];
		if(++len > 3)
			break;
    }
    if(len > 3)
    {
        AddError(ERR_INCORRECT_SENS_FILE_STR);
        return -1;
    }
    *p = (uint8_t)atoi(idBuf);

	SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

    pProcInfo->Data = (void *)p;

    p = pvPortMalloc((size_t)sizeof(QFuncCount));  // память для хранения счетчиков пакетов и счетчика времени
    if( p == NULL )
    {
        AddError(ERR_MEM_SENS_FILE_STR);
        return -1;
    }
    memset((uint8_t *)p, 0, sizeof(QFuncCount));

	QFuncCount *qF = (QFuncCount *)p;
	qF->Qval = 200.f;
	qF->poolsInterval = POOLS_FOR_INIT;	// по включению питания 30 с на инициализацию и проверку связи

    pProcInfo->Buf = (void *)p;

    return 0;	
}

#endif //	_MB_QUAL_FUNC_

#if _OMNI_SNIFFER_FUNC_

	DATA_FUNC OmniSnifferFunc = {.Addr = (SENSOR_PROC_FUNC *)&OMNSFunc, .Init_Proc = (INIT_PROC *)&InitOMNSFunc, .Name = "OMNSFunc", .pNext = NULL, .pFuncList = NULL};

	static float OMNSFunc(SENSOR_HANDLE handle)
	{
		int count = 0;
		float err = 0;
		SET_SNaN(&err);

		static uint8_t zaglushka = 0x0;
		zaglushka ^= 0xff;
		if(!zaglushka)
			return err;

		DEVICE_HANDLE hDevice = OpenDevice("OMNS");
		if(!handle)	
			return err;

    	SENSOR_INFO *pSensInfo;
    	pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
   		PROC_INFO *pProcInfo;
    	pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);
		uint8_t ID = *(uint8_t *)pProcInfo->Data;

		uint16_t N;
		SeekDevice(hDevice, (uint32_t)ID, 0);
		count = ReadDevice(hDevice, &N, 1);	
		CloseDevice(hDevice);

		if(count == 0){
			pSensInfo->f_valid_data = 0;	
			return err;
		}
		pSensInfo->f_valid_data = 1;
		//if(res != 1)
		//	return err;
			
		return (float)N;	
	}

    static int InitOMNSFunc(SENSOR_HANDLE handle, char *data, uint16_t count)
	{
		uint8_t *p = pvPortMalloc((size_t)1);  //   Память для хранения id устройства и таймаута
	    if( p == NULL )
	    {
	        AddError(ERR_MEM_SENS_FILE_STR);
	        return -1;
	    }
	
		char idBuf[4] = {0,0,0,0}; // буфер для строки с id (не более 3 символов)
		int len = 0;   // длина строки
	
	    while(data[len] != ';') 
	    {
			if(len < 3)
				idBuf[len] = data[len];
			if(++len > 3)
				break;
	    }
	    if(len > 3)
	    {
	        AddError(ERR_INCORRECT_SENS_FILE_STR);
	        return -1;
	    }
	    *p = (uint8_t)atoi(idBuf);

	
		SENSOR_INFO *pSensInfo;
	    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
	    PROC_INFO *pProcInfo;
	    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);
	
	    pProcInfo->Data = (void *)p;
	
	    p = pvPortMalloc((size_t)sizeof(uint16_t));  // память для хранения величины N Omnicomm
	    if( p == NULL )
	    {
	        AddError(ERR_MEM_SENS_FILE_STR);
	        return -1;
	    }
	    memset((uint8_t *)p, 0, sizeof(uint16_t));
	
	    pProcInfo->Buf = (void *)p;

		DEVICE_HANDLE hOMNS = OpenDevice("OMNS");
		if(hOMNS == 0)
			return -1;

		DeviceIOCtl(hOMNS, SET_OMNI_DEVICE, (uint8_t *)pProcInfo->Data);
		CloseDevice(hOMNS);
	
	    return 0;	
	}

#endif

/*
int InitMeanProc(SENSOR_HANDLE handle, uint8_t *buff, uint16_t count)
{
    SENSOR_INFO *pSensInfo;
    pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    PROC_INFO *pProcInfo;
    pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

    float size;

    memcpy(&size, buff, 4);


    pProcInfo->Depth = (uint16_t)size;
    pProcInfo->Flag = 0;

    uint32_t *p;
    p = pvPortMalloc((size_t)(sizeof(MEANINFO) + (size*4)));
    if( p == NULL ) return -1;

    pProcInfo->Buf = (void *)p;
    memset((uint8_t *)p, 0, (sizeof(MEANINFO) + (size*4)));

    return 0;


}
*/

SENSOR_PROC_FUNC * GetDataFuncAddr(char *name)
{
    DATA_FUNC *pFunc;
    pFunc = datafunc_list.pFirstFunc;
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


void GetDataFuncName(SENSOR_PROC_FUNC addr, char *data)
{
    DATA_FUNC *pFunc;
    pFunc = datafunc_list.pFirstFunc;
    while(pFunc != NULL)
    {
		if(pFunc->Addr == (SENSOR_PROC_FUNC *)addr)
		{
			strcpy(data, (char *)pFunc->Name);
			return;
		}
        pFunc = pFunc->pNext;
    }
}



INIT_PROC * GetInitDataFuncAddr(char *name)
{
    DATA_FUNC *pFunc;
    pFunc = datafunc_list.pFirstFunc;
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


uint8_t AddDataFun(DATA_FUNC *pFunc)
{
    datafunc_list.NumberOfItems++;
    pFunc->pFuncList = &datafunc_list;
    if(datafunc_list.pFirstFunc == NULL)
    {
        datafunc_list.pFirstFunc = pFunc;
        pFunc->pNext = NULL;
    }
    else
    {
        pFunc->pNext = datafunc_list.pFirstFunc;
        datafunc_list.pFirstFunc = pFunc;
    }
    return 0;
}

void InitDataFunctions(void)
{
#if _EXP_FUNC_
    AddDataFun(&Exp);
	AddDataFun(&ExpEx);
#endif

#if _NULL_DATA_FUNC_
	AddDataFun(&NullData);
#endif

#if _TEMPER_FUNC_
extern    DATA_FUNC Temper;
    AddDataFun(&Temper);
#endif

#if _GZ_RADIUS_FUNC_
extern    DATA_FUNC GZRadius;
    AddDataFun(&GZRadius);
#endif

#if _GZ_RECT_FUNC_
	extern    DATA_FUNC GZRect;
    AddDataFun(&GZRect);
#endif

#if _IMP_FUNC_
    AddDataFun(&Imp);
#endif

#if _MB_FUNC_
    AddDataFun(&MBf);
#endif

//#if _ADC_EXP_FUNC_
//    AddDataFun(&ADCExp);
//#endif

#if _DESCRETE_FUNC_
    AddDataFun(&DFunc);
#endif
   // AddDataFun(&Mean);
#if _CLOCK_SENS_FUNC_
    AddDataFun(&CSens);
#endif

#if _VIRT_SENS_FUNC_
    AddDataFun(&VSens);
#endif


#if _BUTTON_FUNC_
    AddDataFun(&BFunc);
#endif

#if _AVERAGE_FUNC_
    AddDataFun(&Average);
#endif

#if _MB_QUAL_FUNC_
	AddDataFun(&QFunc);
#endif

#if _OMNI_SNIFFER_FUNC_
	AddDataFun(&OmniSnifferFunc);
#endif
}

uint8_t is_match_operat(uint8_t *data)
{
    if(strcmp((char *)data, "=") != 0 && strcmp((char *)data, "<") != 0 && strcmp((char *)data, ">") != 0 && strcmp((char *)data, "<=") != 0 && strcmp((char *)data, ">=") != 0)
    {
        return 0;
    }
    else return 1;
}

uint8_t is_operat(uint8_t *data)
{
    if(strcmp((char *)data, "+") != 0 && strcmp((char *)data, "-") != 0 && strcmp((char *)data, "*") != 0 && strcmp((char *)data, "/") != 0 && strcmp((char *)data, "&") != 0 && strcmp((char *)data, "|") != 0)
    {
        return 0;
    }
    else return 1;
}

uint8_t is_const(uint8_t *data)
{
    if(strtod((char *)data, NULL) == 0 && *(char *)data != '0')
    {
        return 0;
    }
    else return 1;
}

uint8_t is_sensor(uint8_t *data)
{
    SENSOR *pSensor = OpenSensorProc((char *)data);
    if(pSensor == NULL)
    {
        return 0;
    }
    else return 1;
}

float compute(float value_1 , uint8_t operat, float value_2)
{
    switch(operat)
    {
        case '+': return (value_1 + value_2);
        case '-': return (value_1 - value_2);
        case '*': return (value_1 * value_2);
        case '/': return (value_1 / value_2);
        case '&':
            if(value_1 > 2 || value_1 < 0 || value_2 > 2 || value_2 < 0) return (float)-1;
            else return (float)((uint8_t)value_1 && (uint8_t)value_2);
        case '|':
            if(value_1 > 2 || value_1 < 0 || value_2 > 2 || value_2 < 0) return (float)-1;
            else return (float)((uint8_t)value_1 || (uint8_t)value_2);
    }
    return -1;
}

float match_compute(float value_1, uint8_t *operat, float value_2)
{
    if(strcmp((char *)operat, "=") == 0)
                {
                    if(value_1 == value_2) return (float)1;
                    else return (float)0;
                }
    else if(strcmp((char *)operat, "<") == 0)
                {
                    if(value_1 < value_2) return (float)1;
                    else return (float)0;
                }
    else if(strcmp((char *)operat, ">") == 0)
                {
                    if(value_1 > value_2) return (float)1;
                    else return (float)0;
                }
    else if(strcmp((char *)operat, "<=") == 0)
                {
                    if(value_1 <= value_2) return (float)1;
                    else return (float)0;
                }
   else if(strcmp((char *)operat, ">=") == 0)
                {
                    if(value_1 >= value_2) return (float)1;
                    else return (float)0;
                }
  else return -1;
}


int cmp_val(float sens_val, float cmp_val, uint8_t cmp_op)
{
    switch(cmp_op)
    {
        case EQUAL:
            if(sens_val == cmp_val) return 1;
            else return 0;
        case MORE:
            if(sens_val > cmp_val) return 1;
            else return 0;
        case LESS:
            if(sens_val < cmp_val) return 1;
            else return 0;
        case E_MORE:
            if(sens_val >= cmp_val) return 1;
            else return 0;
        case E_LESS:
            if(sens_val <= cmp_val) return 1;
            else return 0;
        default:
            break;     
    }
    return 0;
}
/*
uint8_t FlagClockSens;

inline void SetFlagClockSens(void)
{
    FlagClockSens = 1;
}

inline void ClrFlagClockSens(void)
{
    FlagClockSens = 0;
}

inline uint8_t GetFlagClockSens(void)
{
    return FlagClockSens;
}

void ChangeFlagClockSens(void)
{
    FlagClockSens ^= FlagClockSens;
    FlagClockSens &= 0x01;
}
*/
#endif  //  _SENSORS_


