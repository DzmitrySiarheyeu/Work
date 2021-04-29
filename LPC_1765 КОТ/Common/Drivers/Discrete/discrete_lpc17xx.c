#include "config.h"
#if(_DISCRETE_)

#include <lpc17xx_gpio.h>
#include <string.h>
#include <stdio.h>

#include "discrete_lpc17xx.h"
#include "driverscore.h"


static int CreateDiscrete(void);
static DEVICE_HANDLE OpenDiscrete(void);
static void CloseDiscrete(DEVICE_HANDLE handle);
int ReadDiscrete(DEVICE_HANDLE handle, void * pDst, int count);
int WriteDiscrete(DEVICE_HANDLE handle, const void * pDst, int count);
int SeekDiscreteText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
int ReadDiscreteText(DEVICE_HANDLE handle, void * pDst, int count);
int WriteDiscreteText(DEVICE_HANDLE handle, void * pDst, int count);
int SeekDiscrete(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);

int IOCtlDiscrete(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData);
void PoolDiscrete(DEVICE_HANDLE handle);


DISCRETE_INFO discrete_info = 
{ 
    .ReadMode =                       READ_WITH_SHIFT,
};

DiscreteData disData = {.DataPoint = 0};

const DEVICE_IO discrete_device_io = {
    .DeviceID = DISCRETE,
	.CreateDevice =	CreateDiscrete,
	.CloseDevice = CloseDiscrete,
    .OpenDevice = OpenDiscrete,
    .ReadDevice = ReadDiscrete,
    .WriteDevice = WriteDiscrete,
    .SeekDevice = SeekDiscrete,
    .ReadDeviceText = ReadDiscreteText,
    .WriteDeviceText = WriteDiscreteText,
    .SeekDeviceText = SeekDiscreteText,
    .DeviceIOCtl = IOCtlDiscrete,
    .DeviceName = "DISCRETE"
    };


DEVICE discrete_device = {
    .flDeviceOpen = 0,
    .device_io = (DEVICE_IO *)&discrete_device_io,
    .pDriverData = &discrete_info,
    .pData = (DiscreteData *)&disData
    };

int CreateDiscrete(void)
{
	int i = 0;
//	#if(NUMBER_OF_CH_DIS > 4)
//		#error "NUMBER_OF_CH_DIS must be less than 5"
//	#endif

	for(i = 0; i < NUMBER_OF_CH_DIS; i++)
	{
		discrete_info.buf[i].in = 0;
		discrete_info.buf[i].in = 0;
	}		

   DEBUG_PUTS("Discrete Created\n");
   return DEVICE_OK;
}


DEVICE_HANDLE OpenDiscrete(void)
{
	if(discrete_device.flDeviceOpen == DEVICE_CLOSE)
	{
		 discrete_device.flDeviceOpen = DEVICE_OPEN;
	}
	else
	{
		  DEBUG_PUTS("Discrete RE_OPEN\n");
	}

	return (DEVICE_HANDLE)&discrete_device;
}

void CloseDiscrete(DEVICE_HANDLE handle)
{
	if(discrete_device.flDeviceOpen == DEVICE_OPEN)
	{
		 discrete_device.flDeviceOpen = DEVICE_CLOSE;
	}
	else
	{
		  DEBUG_PUTS("Discrete RE_CLOSE\n");
	}
}

//читаем из буфера 
int ReadDiscrete(DEVICE_HANDLE handle, void * pDst, int count)
{
    DISCRETE_INFO *pInfo = (DISCRETE_INFO *)(((DEVICE *)handle)->pDriverData);

        uint8_t buf_number = pInfo->DataPoint;
        int i = 0;
        uint8_t temp_out, *p_out;

		if(pInfo->ReadMode == READ_WITH_SHIFT) // чтение со смещением out
            p_out = &pInfo->buf[buf_number].out;
		else if(pInfo->ReadMode == READ_LATEST) // чтение последенего записанного значения без учета состояний out и in
		{
			pInfo->ReadMode = READ_WITH_SHIFT;
			uint8_t in = pInfo->buf[buf_number].in;

			*(uint8_t *)pDst = (in != 0 ? pInfo->buf[buf_number].d_buf[in - 1] : pInfo->buf[buf_number].d_buf[DISCRETE_BUF_SIZE - 1]);

			 return 1;
		}
        else // чтение без смещения out
        {
            temp_out = pInfo->buf[buf_number].out;
            p_out = &temp_out;
        }

        for(i = 0; i < count; i++)
        {
            if (*p_out == pInfo->buf[buf_number].in)
            {
                return i;
            }

            ((uint8_t *)pDst)[i] = pInfo->buf[buf_number].d_buf[(*p_out)++];

            if(*p_out >= DISCRETE_BUF_SIZE) *p_out = 0;
        }

        pInfo->ReadMode = READ_WITH_SHIFT;

        return count;
}

int WriteDiscrete(DEVICE_HANDLE handle, const void * pDst, int count)
{
    return -1;
}

int SeekDiscrete(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
    DISCRETE_INFO *pInfo = (DISCRETE_INFO *)(((DEVICE *)handle)->pDriverData);

    if( offset > 3) 
        return -1;
    else pInfo->DataPoint = offset;

    return offset;
}




int WriteDiscreteText(DEVICE_HANDLE handle, void * pDst, int count)
{
    return -1;
}
int ReadDiscreteText(DEVICE_HANDLE handle, void * pDst, int count)
{
#if(_TEXT_)
    DiscreteData *pData = (DiscreteData *)(((DEVICE *)handle)->pData);
	int i;
    char *BufferText;
	int sprintf_count = 0;
	
	if(count >= 0)
	{
		memset(pDst, 0, count);
		BufferText = pvPortMalloc(300);
		if(BufferText != NULL)
        {
			for(i = 0; i < NUMBER_OF_CH_DIS; i++)
			{
				 sprintf_count += sprintf(BufferText + sprintf_count, "DIN%d %u\n", i + 1, pData->value[i]);
			}
                    
            if((pData->DataPoint + count) > sprintf_count)
			{
				count = sprintf_count - pData->DataPoint;
			}
			memcpy(pDst, (char *)(((uint8_t *)BufferText) + pData->DataPoint), count);
			pData->DataPoint += count;
			vPortFree(BufferText);
            return count;
		}
	}
	return 0;
#else
	return -1;
#endif
}

int SeekDiscreteText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
	#if(_TEXT_)
        DiscreteData *pData = (DiscreteData *)(((DEVICE *)handle)->pData);
        switch(origin)
        {
            case 0:
                if(offset < MAX_DISCRETE_DATA_OFFSET)
                {
                    pData->DataPoint = offset;
                }
                else
                {
                    pData->DataPoint = 0;
                    return 0;
                }
                break;
            case 1:
                if((pData->DataPoint + offset) < MAX_DISCRETE_DATA_OFFSET)
                {
                    pData->DataPoint = pData->DataPoint + offset;
                }
                else
                {
                    pData->DataPoint = 0;
                    return 0;
                } 
                break;
            case 2:
                if(offset < MAX_DISCRETE_DATA_OFFSET)
                {
                    pData->DataPoint = MAX_DISCRETE_DATA_OFFSET - offset;
                }
                else
                {
                    pData->DataPoint = 0;
                    return 0;
                } 
                break;
            default:
                pData->DataPoint = 0;
                return 0;          
        }
        return pData->DataPoint;
	#else
		return -1;
	#endif
}

int IOCtlDiscrete(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData)
{
    DISCRETE_INFO *pInfo = (DISCRETE_INFO *)(((DEVICE *)handle)->pDriverData);


        switch(code)
        {
            case POOL:
                PoolDiscrete(handle);
                break;
    
            case SET_READ_MODE:
                pInfo->ReadMode = *pData;
                break;
                     
            default:
                return -1;
        }

        return 0;
}

void PoolDiscrete(DEVICE_HANDLE handle)
{
    DISCRETE_INFO *pInfo = (DISCRETE_INFO *)(((DEVICE *)handle)->pDriverData);
    DiscreteData *pData = (DiscreteData *)(((DEVICE *)handle)->pData);

#if(NUMBER_OF_CH_FR_DIS > 0)
        if(( GPIO_ReadValue(DIS_CH_0_PORT) & (1 << DIS_CH_0_PIN)) > 0 ) 
            pInfo->buf[0].d_buf[pInfo->buf[0].in] = 0x01 ^ DISCRETE_ACTIVE_LEVEL;
		else 
            pInfo->buf[0].d_buf[pInfo->buf[0].in] = 0x00 ^ DISCRETE_ACTIVE_LEVEL;
        
        pData->value[0] = pInfo->buf[0].d_buf[pInfo->buf[0].in];
        
        if(++pInfo->buf[0].in >= DISCRETE_BUF_SIZE) 
            pInfo->buf[0].in = 0;
		if(pInfo->buf[0].in == pInfo->buf[0].out)
		{
			if(++pInfo->buf[0].out >= DISCRETE_BUF_SIZE)
				pInfo->buf[0].out = 0;
		}
#endif

 
#if(NUMBER_OF_CH_FR_DIS > 1)  
        if( (GPIO_ReadValue(DIS_CH_1_PORT) & (1 << DIS_CH_1_PIN)) > 0 ) //DIN2
            pInfo->buf[1].d_buf[pInfo->buf[1].in] = 0x01 ^ DISCRETE_ACTIVE_LEVEL;
		else 
            pInfo->buf[1].d_buf[pInfo->buf[1].in] = 0x00 ^ DISCRETE_ACTIVE_LEVEL;

        pData->value[1] = pInfo->buf[1].d_buf[pInfo->buf[1].in];
        
        if(++pInfo->buf[1].in >= DISCRETE_BUF_SIZE) 
            pInfo->buf[1].in = 0;
		if(pInfo->buf[1].in == pInfo->buf[1].out)
		{
			if(++pInfo->buf[1].out >= DISCRETE_BUF_SIZE)
				pInfo->buf[1].out = 0;
		}
#endif


#if(NUMBER_OF_CH_FR_DIS > 2) 
        if( (GPIO_ReadValue(DIS_CH_2_PORT) & (1 << DIS_CH_2_PIN)) > 0 ) //DIN3
            pInfo->buf[2].d_buf[pInfo->buf[2].in] = 0x01 ^ DISCRETE_ACTIVE_LEVEL;
		else 
            pInfo->buf[2].d_buf[pInfo->buf[2].in] = 0x00 ^ DISCRETE_ACTIVE_LEVEL;

        pData->value[2] = pInfo->buf[2].d_buf[pInfo->buf[2].in];
        
        if(++pInfo->buf[2].in >= DISCRETE_BUF_SIZE) 
            pInfo->buf[2].in = 0;
		if(pInfo->buf[2].in == pInfo->buf[2].out)
		{
			if(++pInfo->buf[2].out >= DISCRETE_BUF_SIZE)
				pInfo->buf[2].out = 0;
		}
#endif


#if(NUMBER_OF_CH_FR_DIS > 3)
        if( (GPIO_ReadValue(DIS_CH_3_PORT) & (1 << DIS_CH_3_PIN)) > 0 ) //DIN4
            pInfo->buf[3].d_buf[pInfo->buf[3].in] = 0x01 ^ DISCRETE_ACTIVE_LEVEL;
		else 
            pInfo->buf[3].d_buf[pInfo->buf[3].in] = 0x00 ^ DISCRETE_ACTIVE_LEVEL;

        pData->value[3] = pInfo->buf[3].d_buf[pInfo->buf[3].in];
        
        if(++pInfo->buf[3].in >= DISCRETE_BUF_SIZE) 
            pInfo->buf[3].in = 0;
		if(pInfo->buf[3].in == pInfo->buf[3].out)
		{
			if(++pInfo->buf[3].out >= DISCRETE_BUF_SIZE)
				pInfo->buf[3].out = 0;
		}
#endif
}

#endif // _DISCRETE_INPUT_

