#include "Config.h"
#if(_SENSORS_)

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "main.h"
#include "Events.h"
#include "SetParametrs.h"
#include "SensorCore.h"
#include "DriversCore.h"
#include "User_Error.h"
#include "User_Events.h"
#include "DataFunc.h"
#include "PhysFunc.h"
#include "WDT.h"

//#include "Indy_Log.h"
#if(_DEBUG_SYS_)
#include "debug_system.h"
#endif

//#include "display.h"

#if(_EVENT_LOG_)
	int AddLogMessage(uint8_t *message);
#endif


SENSOR_LIST sensor_list = { .NumberOfItems = 0, .pFirstSensor = NULL };
uint8_t log_buf[18];


uint16_t StrToFloat(uint8_t *Buf);
void PutValueToOutBuf(SENSOR_INFO *pSensInfo);
//int PutProcKoef(uint8_t *buf, uint32_t *pointer, uint16_t count);
//int form_log_event_mes(SENS_CHECK *pCheck, uint8_t *buf);

int ReadSensor(SENSOR_HANDLE handle, void *pDst, int count)
{
	uint32_t temp = QNaN;
	if(handle == NULL) return -1;
    SENSOR_INFO *pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
        if(count >= 0)
        {
			if(pSensInfo->f_valid_data == 0){
				memcpy(pDst, (char *)&temp, count*4);
				return 1;
			} 
            memset(pDst, 0, count*4);
            if(count > pSensInfo->PrivatPhysProc.Depth)
            {
                count = pSensInfo->PrivatPhysProc.Depth;
            }
            memcpy(pDst, (uint8_t *)((uint8_t *)pSensInfo->PrivatPhysProc.Buf), count*4);
            return count;
        }
    return -1;
}

#if(_TEXT_)
int ReadSensorText(SENSOR_HANDLE handle, void *pDst, int count)
{
	if(handle == NULL) return -1;
    SENSOR_INFO *pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    int sprintf_count;
	char *BufferText;
	
	if(count >= 0)
	{
		memset(pDst, 0, count);
		BufferText = pvPortMalloc(300);
		if(BufferText != NULL)
        { 
			sprintf_count = sprintf(BufferText, "HandledData: %5lf;\n", pSensInfo->Data);
			sprintf_count += sprintf(BufferText + sprintf_count, "PhysValue: %5lf;\n", pSensInfo->PhysValue);
	
            if((pSensInfo->TextDataPoint + count) > sprintf_count)
			{
				count = sprintf_count - pSensInfo->TextDataPoint;
			}
			memcpy(pDst, (char *)(((uint8_t *)BufferText) + pSensInfo->TextDataPoint), count);
			pSensInfo->TextDataPoint += count;
			vPortFree(BufferText);
                return count;
            }
	}
    return 0;
}

int SeekSensorText(SENSOR_HANDLE handle, uint16_t offset, uint8_t origin)
{
	if(handle == NULL) return -1;
    SENSOR_INFO *pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    //if( xSemaphoreTake( pSensInfo->Sem, 1000 ) == pdTRUE )
    //{
        switch(origin)
        {
            case 0:
                if(offset < MAX_SENSOR_DATA_TEXT_OFFSET)
                {
                    pSensInfo->TextDataPoint = offset;
                }
                else
                {
                    pSensInfo->TextDataPoint = 0;
                }
                break;
            case 1:
                if((pSensInfo->TextDataPoint + offset) < MAX_SENSOR_DATA_TEXT_OFFSET)
                {
                    pSensInfo->TextDataPoint = pSensInfo->TextDataPoint + offset;
                }
                else
                {
                    pSensInfo->TextDataPoint = 0;
                } 
                break;
            case 2:
                if(offset < MAX_SENSOR_DATA_TEXT_OFFSET)
                {
                    pSensInfo->TextDataPoint = MAX_SENSOR_DATA_TEXT_OFFSET - offset;
                }
                else
                {
                    pSensInfo->TextDataPoint = 0;
                } 
                break;
            default:
//                xSemaphoreGive(pSensInfo->Sem);
                pSensInfo->TextDataPoint = 0;
                return -1;                  
        }
       // xSemaphoreGive(pSensInfo->Sem);
        return pSensInfo->TextDataPoint;
   // }
   // return -1;
}
#endif //_TEXT_
float DataProcSensor(SENSOR_HANDLE handle)
{
	if(handle == NULL) return -1;
    return ((SENSOR *)handle)->DataProcSensor(handle);
}

int DataInit(SENSOR_HANDLE handle, char *buf, uint16_t count)
{
	if(handle == NULL) return -1;
    return ((SENSOR *)handle)->DataInit(handle, buf, count);
}

int PhysInit(SENSOR_HANDLE handle, char *buf, uint16_t count)
{
    return ((SENSOR *)handle)->PhysInit(handle, buf, count);
}

float PhysProcSensor(SENSOR_HANDLE handle)
{
	if(handle == NULL) return -1;
    return ((SENSOR *)handle)->PhysProcSensor(handle);
}

int SensorIOCtl(SENSOR_HANDLE handle, uint8_t code, uint8_t *pData)
{
	int i = 0;
	if(handle == NULL) return -1;
    SENSOR_INFO *pSensInfo = (SENSOR_INFO*)(((SENSOR *)handle)->pSensorData);
    SENSOR      *pSensor = (SENSOR *)handle;

    switch(code)
    {
        case SENSOR_DEVICE:
            memcpy((char *)&(pSensor->SensorDevise), pData, 4);
            break;
        case DATA_PROC:
            memcpy((char *)&(pSensor->DataProcSensor), pData, 4);
            break;
        case PHYS_PROC:
            memcpy((char *)&(pSensor->PhysProcSensor), pData, 4);
            break;
        case INIT_DATA_PROC:
            memcpy((char *)&(pSensor->DataInit), pData, 4);
            break;
        case INIT_PHYS_PROC:
            memcpy((char *)&(pSensor->PhysInit), pData, 4);
            break;
        case PHYS_PROC_OUTBUF:
            memcpy((char *)&(pSensInfo->PrivatPhysProc.Depth), pData, 2);
            pSensInfo->PrivatPhysProc.Buf = pvPortMalloc((size_t)pSensInfo->PrivatPhysProc.Depth * 4);
			for(i = 0; i < pSensInfo->PrivatPhysProc.Depth; i++)
			{
				SET_QNaN(&((float *)pSensInfo->PrivatPhysProc.Buf)[i]);
			}
            if( pSensInfo->PrivatPhysProc.Buf == NULL ) return -1;
            break;
        //case KOEF_DATA_PROC:
         //   memcpy((uint8_t *)&(pSensInfo->PrivatDataProc.Data), pData, 4);
       // case KOEF_PHYS_PROC:
       //     memcpy((uint8_t *)&(pSensInfo->PrivatPhysProc.Data), pData, 4);
      //      break;
        case ALL_SETTINGS:
      /*      if(SensorIOCtl(handle, SENSOR_DEVICE, pData))                          return -1;
            if(SensorIOCtl(handle, DATA_PROC, (uint8_t *)&pData[4]))               return -1;
            if(SensorIOCtl(handle, PHYS_PROC, (uint8_t *)&pData[8]))               return -1;
            //if(SensorIOCtl(handle, DATA_PROC_INBUF, (uint8_t *)&pData[12]))       return -1;
            if(SensorIOCtl(handle, PHYS_PROC_OUTBUF, (uint8_t *)&pData[14]))       return -1;
            if(SensorIOCtl(handle, KOEF_DATA_PROC, (uint8_t *)&pData[16]))          return -1;
            if(SensorIOCtl(handle, KOEF_PHYS_PROC, (uint8_t *)&pData[20]))          return -1;*/
            break;
        default:
            return -1;
    }
    return 0;
}

uint8_t AddSensor(SENSOR *pSensor)
{
    sensor_list.NumberOfItems++;
    pSensor->pSensorList = &sensor_list;
    if(sensor_list.pFirstSensor == NULL)
    {
        sensor_list.pFirstSensor = pSensor;
        pSensor->pNext = NULL;
    }
    else
    {
        pSensor->pNext = sensor_list.pFirstSensor;
        sensor_list.pFirstSensor = pSensor;
    }
    return SENSOR_OK;
}


int GetSensNumber(void)
{
	return sensor_list.NumberOfItems;
}

SENSOR_HANDLE AttachSensor(char *name)
{
    SENSOR *pSensor;
    pSensor = sensor_list.pFirstSensor;
    while(pSensor != NULL)
    {
        if(strcmp(pSensor->SensorName, name) == 0)
        {
            if(pSensor->flSensorOpen == SENSOR_OPEN)
            {
                return pSensor;
            }
            return NULL;
        }
        else
        {
            pSensor = pSensor->pNext;
        }
    }
    return NULL;
}

SENSOR_HANDLE OpenSensorProc(char *name)
{
    SENSOR *pSensor;
    pSensor = sensor_list.pFirstSensor;
    while(pSensor != NULL)
    {
        if(strcmp(pSensor->SensorName, name) == 0)
        {
            if(pSensor->flSensorOpen != SENSOR_CLOSE)
            {
                #if(_DEBUG_SYS_)
               // DEBUG_PRINTF_ONE("Sensor %s is already open /r", pSensor->SensorName);
                #endif
                return (SENSOR_HANDLE)pSensor;
            }
            pSensor->flSensorOpen = SENSOR_OPEN;
            #if(_DEBUG_SYS_)
           // DEBUG_PRINTF_ONE("Sensor %s is successfully open /r", pSensor->SensorName);
            #endif
            return (SENSOR_HANDLE)pSensor;
        }
                #if(_DEBUG_SYS_)
		//DEBUG_PRINTF_ONE("Device %s is not found /r", name);
                #endif
        pSensor = pSensor->pNext;
    }
    return NULL;
}


//SENSOR_HANDLE OpenSensor(char *name)
void InitSensorSystem(void)
{
    int err;
    uint16_t count;
    char name[SENSOR_NAME_MAX_LENGHT];
    uint32_t temp;
    DEVICE_HANDLE *pDevice;
    SENSOR *pSensor;
    char *p;
    SENSOR_INFO *pSensInfo;

    int CommonSens = 0, ClockSens = 0, VirtualSens = 0;

    if(GetSettingsFileStatus())
    {
        while((err = GetCurrentTop(name, SENSOR_NAME_MAX_LENGHT)) == 0)
        {
			ResetWDT();
            if(err == -1)  // Не удалось получить имя 
            {
                AddError(ERR_INCORRECT_SENS_FILE_STR);
                return;
            }

			CommonSens = 0;
			ClockSens = 0;
			VirtualSens = 0;
                         //  Узнаем кокого типа датчик  
            CommonSens |= !strncmp((char *)name,"S", 1);
			CommonSens |= !strncmp((char *)name,"G", 1);
            CommonSens |= !strncmp((char *)name,"O", 1);
			CommonSens |= !strncmp((char *)name,"A", 1);
			CommonSens |= !strncmp((char *)name,"F", 1);
			CommonSens |= !strncmp((char *)name,"I", 1);
			CommonSens |= !strncmp((char *)name,"D", 1);
			CommonSens |= !strncmp((char *)name,"If", 2);
            ClockSens = !strncmp((char *)name,"C", 1);
            VirtualSens = !strncmp((char *)name,"V", 1);

            if(!CommonSens && !ClockSens && !VirtualSens)  // Если неизвестный тип - ОШИБКА
            {
                AddError(ERR_INCORRECT_SENS_FILE_STR);
                return;
            }

            pSensor = (SENSOR *)pvPortMalloc((size_t)sizeof(SENSOR));
            if(pSensor == NULL)
            {
                AddError(ERR_MEM_SENS_FILE_STR);
                return;
            }
            pSensInfo = (SENSOR_INFO *)pvPortMalloc((size_t)sizeof(SENSOR_INFO));
            if(pSensInfo == NULL)
            {
                AddError(ERR_MEM_SENS_FILE_STR);
                return;
            }
            pSensor->pSensorData = pSensInfo;
            

            strcpy((char *)pSensor->SensorName, (char *)name);

            if(CommonSens)
            {
                p = GetPointToElement(name, "D");
                if(p == 0)
                {
                    AddError(ERR_INCORRECT_SENS_FILE_STR);
                    return;
                }
                pDevice = OpenDevice((char *)p);
                if(pDevice == 0)
                {
                    AddError(ERR_INCORRECT_SENS_FILE_STR);
                    return;
                }
				CloseDevice(pDevice);
                temp = (uint32_t)pDevice;
                SensorIOCtl(pSensor, SENSOR_DEVICE, (uint8_t *)&temp);
            }
            if(CommonSens)
            {
                p = GetPointToElement(name, "DF");
                if(p == 0)
                {
                    AddError(ERR_INCORRECT_SENS_FILE_STR);
                    return;
                }
                temp = (uint32_t)GetDataFuncAddr(p);
                if(temp == 0)
                {
                    AddError(ERR_INCORRECT_SENS_FILE_STR);
                    return;
                }
                SensorIOCtl(pSensor, DATA_PROC, (uint8_t *)&temp);
    
                temp = (uint32_t)GetInitDataFuncAddr(p);
                if(temp == 0)
                {
                    AddError(ERR_INCORRECT_SENS_FILE_STR);
                    return;
                }
                SensorIOCtl(pSensor, INIT_DATA_PROC, (uint8_t *)&temp);
            }
            else if(ClockSens)
            {
                temp = (uint32_t)GetDataFuncAddr("ClSens");
                SensorIOCtl(pSensor, DATA_PROC, (uint8_t *)&temp);
    
                temp = (uint32_t)GetInitDataFuncAddr("ClSens");
                SensorIOCtl(pSensor, INIT_DATA_PROC, (uint8_t *)&temp);
            }
            else if(VirtualSens)
            {
                temp = (uint32_t)GetDataFuncAddr("VSens");
                SensorIOCtl(pSensor, DATA_PROC, (uint8_t *)&temp);
    
                temp = (uint32_t)GetInitDataFuncAddr("VSens");
                SensorIOCtl(pSensor, INIT_DATA_PROC, (uint8_t *)&temp);
            }

            if(CommonSens)
            {
                p = GetPointToElement(name, "PF");
                if(p == 0)
                {
                    AddError(ERR_INCORRECT_SENS_FILE_STR);
                    return;
                }
                temp = (uint32_t)GetPhysFuncAddr(p);
                if(temp == 0)
                {
                    AddError(ERR_INCORRECT_SENS_FILE_STR);
                    return;
                }
                SensorIOCtl(pSensor, PHYS_PROC, (uint8_t *)&temp);
    
                temp = (uint32_t)GetInitPhysFuncAddr(p);
                if(temp == 0)
                {
                    AddError(ERR_INCORRECT_SENS_FILE_STR);
                    return;
                }
                SensorIOCtl(pSensor, INIT_PHYS_PROC, (uint8_t *)&temp);
            }
            else if(VirtualSens)
            {
                temp = (uint32_t)GetPhysFuncAddr("NProc");
                SensorIOCtl(pSensor, PHYS_PROC, (uint8_t *)&temp);
    
                temp = (uint32_t)GetInitPhysFuncAddr("NProc");
                SensorIOCtl(pSensor, INIT_PHYS_PROC, (uint8_t *)&temp);
            }
            else if(ClockSens)
            {
                temp = (uint32_t)GetPhysFuncAddr("NProc");
                SensorIOCtl(pSensor, PHYS_PROC, (uint8_t *)&temp);
    
                temp = (uint32_t)GetInitPhysFuncAddr("NProc");
                SensorIOCtl(pSensor, INIT_PHYS_PROC, (uint8_t *)&temp);
            }

            p = GetPointToElement(name, "Dd");
            if(p == 0)
            {
                AddError(ERR_INCORRECT_SENS_FILE_STR);
                return;
            }
            if(DataInit(pSensor, p, 1) == -1)
            {
                AddError(ERR_INCORRECT_SENS_FILE_STR);
                return;
            }
        //PutProcKoef(buf, &temp, count*4);
        //SensorIOCtl(pSensor, KOEF_DATA_PROC, (uint8_t *)&temp);
            if(CommonSens)
            {
                p = GetPointToElement(name, "Pd");
                if(p == 0)
                {
                    AddError(ERR_INCORRECT_SENS_FILE_STR);
                    return;
                }
            }
            else if(ClockSens)
            {
                p = 0;
            }
            else if(VirtualSens)
            {
                p = 0;
            }
            //count = StrToFloat(buf);
            if(PhysInit(pSensor, p, 1) == -1)
            {
                AddError(ERR_INCORRECT_SENS_FILE_STR);
                return;
            }
        //PutProcKoef(buf, &temp, count*4);
        //SensorIOCtl(pSensor, KOEF_PHYS_PROC, (uint8_t *)&temp);

            p = GetPointToElement(name, "OD");
            if(p == 0)
            {
                AddError(ERR_INCORRECT_SENS_FILE_STR);
                return;
            }
            count = atoi((char *)p);
            if(count < 1 || count > 5)
            {
                AddError(ERR_INCORRECT_SENS_FILE_STR);
                return;
            }
            SensorIOCtl(pSensor, PHYS_PROC_OUTBUF, (uint8_t *)&count);

			pSensInfo->f_valid_data = 1;

            AddSensor(pSensor);
            OpenSensorProc(pSensor->SensorName);
       // #if _MULTITASKING_
       //     vSemaphoreCreateBinary(pSensInfo->Sem);
      //  #endif //_MULTITASKING_
        }
    }
    else AddError(ERR_INCORRECT_SENS_FILE_STR);
    //pSensInfo->Sem = xSemaphoreCreateMutex();

}



uint16_t StrToFloat(uint8_t *Buf)
{
    uint16_t count = 0, i = 0, k = 0;
    float buf[10];
    do
    {
        if(Buf[i] == ';')
        {
            buf[count] = atof((char *)&Buf[k]);
            k = i + 1;
            count++;
        }
        i++;
    }while(Buf[i - 1] != 0);
    memcpy(Buf, (uint8_t *)&buf, count*4);
    return count;
}


void PoolSensor(void)
{
    SENSOR *pSensor = sensor_list.pFirstSensor;
    while(pSensor)
    {
        SENSOR_INFO *pSensInfo;
//        PROC_INFO *pProcInfo;
        pSensInfo = (SENSOR_INFO*)(pSensor->pSensorData);
     //   pProcInfo = (PROC_INFO *)&(pSensInfo->PrivatDataProc);

        float temp;

    //while(ReadDevice(pSensor->SensorDevise, (uint16_t *)(pProcInfo->Buf), 1) != 0)
        do
        {
            temp = DataProcSensor(pSensor);
            if(!IS_SNaN(temp))
            {
                pSensInfo->Data = temp;

                pSensInfo->PhysValue = PhysProcSensor(pSensor);

                //PutValueToOutBuf(pSensInfo);
            }
        }while(!IS_SNaN(temp));
        pSensor = pSensor->pNext;
    }
}

 

uint8_t GetNumberOfSensors(void)
{
    uint8_t count = 0;
    SENSOR *pSensor = sensor_list.pFirstSensor;


    while(pSensor != 0)
    {
        count++;
        pSensor = pSensor->pNext;
    }
    return count;
}

SENSOR_HANDLE GetFirstSensor(void)
{
    return sensor_list.pFirstSensor;
}

#if _OWI_
	void DeInitOwiFunc(void);
#endif

void DeleteAllSensors(void)
{
	SENSOR			*pCurSens = NULL;
	SENSOR 			*pSensor = sensor_list.pFirstSensor;
	SENSOR_INFO		*pSInfo = NULL;
	PROC_INFO		*pDFInfo = NULL;
	PROC_INFO		*pPFInfo = NULL;
    while(pSensor)
    {
		 pSInfo = pSensor->pSensorData;
		 pDFInfo = &pSInfo->PrivatDataProc;
		 pPFInfo = &pSInfo->PrivatPhysProc;

		 if(pPFInfo->Data != NULL) vPortFree(pPFInfo->Data);
		 if(pPFInfo->Buf != NULL) vPortFree(pPFInfo->Buf);

		 if(pDFInfo->Data != NULL) vPortFree(pDFInfo->Data);
		 if(pDFInfo->Buf != NULL) vPortFree(pDFInfo->Buf);

//		 if(pDFInfo->Buf != NULL) vPortFree(pDFInfo->Buf);
//		 if(pDFInfo->Data != NULL) vPortFree(pDFInfo->Data);
//
//		 if(pPFInfo->Buf != NULL) vPortFree(pPFInfo->Buf);
//		 if(pPFInfo->Data != NULL) vPortFree(pPFInfo->Data);

		 vPortFree(pSInfo);

		 pCurSens = pSensor;
		 pSensor = pCurSens->pNext;

		 vPortFree(pCurSens);
	}

	sensor_list.pFirstSensor = 0; 
	sensor_list.NumberOfItems = 0;

#if _OWI_
	DeInitOwiFunc();
#endif
}

#endif
