#include "Config.h"
#if _MODBUS_
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "User_ModBus.h"
#include "SensorCore.h"
#include "DriversCore.h"
#include "MB_client.h"
#include "MB_server_device.h"
#include "SetParametrs.h"
#include "main.h"

MB_REGISTR *MB_reg;

uint8_t GetNumberOfSensors(void);
SENSOR_HANDLE GetFirstSensor(void);

static int MBRegSize; 

void mem_reverce(void *data, uint8_t count)
{
	uint8_t i, temp;
	for(i = 0; i < count/2; i++)
	{
		temp = ((uint8_t *)data)[ i ];
		((uint8_t *)data)[ i ] = ((uint8_t *)data)[ count - i - 1 ];
		((uint8_t *)data)[ count - i - 1 ] = temp;
	}
}

float Revfloat(float in)
{
    mem_reverce(&in, 4);
    return in;
}

uint32_t Revlong(uint32_t in)
{
    mem_reverce(&in, 4);
    return in;
}

unsigned short Revshort(unsigned short in)
{
    mem_reverce(&in, 2);
    return in;
}

int ReadInputRegistersProc(uint16_t requestAddress, uint16_t requestRegister, char *buf, uint8_t *size)
{
	if(requestAddress < (MBRegSize*sizeof(float)/2))
    {
        if((requestAddress + requestRegister) <= (MBRegSize*sizeof(float)/2))
        {
            memcpy(buf, ((uint8_t *)MB_reg + requestAddress * 2), requestRegister * 2);
            *size = 2 + requestRegister * 2;
			return MB_NO_ERROR;
        }
        else 
        {
            return (ILLEGAL_DATA_VALUE);	
        }
    } 
    else 
    {
        return (ILLEGAL_DATA_ADRESS);
    }
}

void InitModBusReg(void)
{
	MBRegSize = GetNumberOfSensors();
	MB_reg = (MB_REGISTR *)pvPortMalloc(MBRegSize * sizeof(float));	
}

void RegModBusPool(void)
{
	SENSOR *Sensor = (SENSOR *)GetFirstSensor();	

	if(Sensor == 0) 
		return;

	int i = 0;
	float val;
	while(Sensor != 0)
	{
		ReadSensor(Sensor, &val, 1);
		((float *)MB_reg)[i++] = Revfloat(val);		

		Sensor = Sensor->pNext;
	}

/*	int i;
	for(i = 0; i < mbIDs.mbIDsCnt; i++)
	{
		SENSOR_HANDLE sensHandle1, sensHandle2;
		float val1, val2;
	
		switch(i)
		{
			case 0: sensHandle1 = OpenSensorProc("S0");
					sensHandle2 = OpenSensorProc("S1");
					break;
			case 1:	sensHandle1 = OpenSensorProc("S2");
					sensHandle2 = OpenSensorProc("S3");
					break;
			case 2:	sensHandle1 = OpenSensorProc("S4");
					sensHandle2 = OpenSensorProc("S5");
					break;
			case 3:	sensHandle1 = OpenSensorProc("S6");
					sensHandle2 = OpenSensorProc("S7");
					break;
			default: return;
		}

		if(sensHandle1 == 0)
			val1 = -1.0f;
		else
			ReadSensor(sensHandle1, &val1, 1);

		if(sensHandle2 == 0)
			val2 = -1.0f;
		else
			ReadSensor(sensHandle2, &val2, 1);
		
		switch(i)
		{
			case 0: ((float *)MB_reg)[0] = Revfloat(val1);
					((float *)MB_reg)[1] = Revfloat(val2);
					break;
			case 1:	((float *)MB_reg)[2] = Revfloat(val1);
					((float *)MB_reg)[3] = Revfloat(val2);
					break;
			case 2:	((float *)MB_reg)[4] = Revfloat(val1);
					((float *)MB_reg)[5] = Revfloat(val2);
					break;
			case 3:	((float *)MB_reg)[6] = Revfloat(val1);
					((float *)MB_reg)[7] = Revfloat(val2);
					break;
		}		
	}*/
}

// 0 - MB TCP
// 1 - MB через удаленный COM
int MBCMethod = 0;

void MBCMethodConfig(void)
{
	char *res;

	if(GetSettingsFileStatus())
	{
		res = (char *)GetPointToElement("MB_Client", "Method");
		if(res == 0)	
			return;

		if(strcmp(res, "COM") == 0)
			MBCMethod = 1;	
	}	
}


void MBCPortConfig(void)
{
	char *res;

	if(GetSettingsFileStatus())
	{
		res = (char *)GetPointToElement("INFO", "Port");
		if(res == 0)	
			return;
		MBPort = GetNumericPort((uint8_t *)res);	
	}	
}

int GetMBCMethod(void)
{
	return MBCMethod;	
}
 
#endif

