#include "i2c_lpc17xx.h"
#if(_I2C_)
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "lpc17xx_i2c.h"
#include "lpc17xx_pinsel.h"


#if(_I2C0_)

static DEVICE_HANDLE OpenI2C0(void);
static int CreateI2C0(void);

#endif

#if(_I2C1_)

static DEVICE_HANDLE OpenI2C1(void);
static int CreateI2C1(void);

#endif

#if(_I2C2_)

static DEVICE_HANDLE OpenI2C2(void);
static int CreateI2C2(void);

#endif

static void CloseI2C(DEVICE_HANDLE handle);
static int ReadI2C(DEVICE_HANDLE handle, void * dst, int count);
static int WriteI2C(DEVICE_HANDLE handle,const void * dst, int count);
static int SeekI2C(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
static int WriteI2CText(DEVICE_HANDLE handle, void * pSrc, int count);
static int ReadI2CText(DEVICE_HANDLE handle, void * pDst, int count);
static int SeekI2CText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
static int I2CIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData);


#if(_I2C0_)

static I2C_INFO_17xx i2c_info_0 = { .pI2C = LPC_I2C0, .Sem = 0, .Mem_Adr = 0, .Dev_Adr = 0};

I2CData  i2c0Data = {
    .tx_count = 0,
    .rx_count = 0,
    .DataPoint = 0,
    };

const DEVICE_IO i2c0_device_io = {
    .DeviceID = I2C_0,
	.CreateDevice = CreateI2C0,
	.CloseDevice = CloseI2C,
    .OpenDevice = OpenI2C0,
    .ReadDevice = ReadI2C,
    .WriteDevice = WriteI2C,
    .SeekDevice = SeekI2C,
    .ReadDeviceText = ReadI2CText,
    .WriteDeviceText = WriteI2CText,
    .SeekDeviceText = SeekI2CText,
	.DeviceIOCtl = I2CIOCtl,
    .DeviceName = "I2C0",
    };

DEVICE i2c0_device = {
    .device_io = (DEVICE_IO *)&i2c0_device_io,
    .flDeviceOpen = 0,
    .pDriverData = &i2c_info_0,
    .pData = &i2c0Data
    };

#endif

#if(_I2C1_)

static I2C_INFO_17xx i2c_info_1 = { .pI2C = LPC_I2C1, .Sem = 0, .Mem_Adr = 0, .Dev_Adr = 0};

I2CData  i2c1Data = {
    .tx_count = 0,
    .rx_count = 0,
    .DataPoint = 0,
    };

const DEVICE_IO i2c1_device_io = {
    .DeviceID = I2C_1,
	.CreateDevice = CreateI2C1,
	.CloseDevice = CloseI2C,
    .OpenDevice = OpenI2C1,
    .ReadDevice = ReadI2C,
    .WriteDevice = WriteI2C,
    .SeekDevice = SeekI2C,
    .ReadDeviceText = ReadI2CText,
    .WriteDeviceText = WriteI2CText,
    .SeekDeviceText = SeekI2CText,
	.DeviceIOCtl = I2CIOCtl,
    .DeviceName = "I2C1",
    };

DEVICE i2c1_device = {
    .device_io = (DEVICE_IO *)&i2c1_device_io,
    .flDeviceOpen = 0,
    .pDriverData = &i2c_info_1,
    .pData = &i2c1Data
    };

#endif

#if(_I2C2_)

static I2C_INFO_17xx i2c_info_2 = { .pI2C = LPC_I2C2, .Sem = 0, .Mem_Adr = 0, .Dev_Adr = 0};

I2CData  i2c2Data = {
    .tx_count = 0,
    .rx_count = 0,
    .DataPoint = 0,
    };

const DEVICE_IO i2c2_device_io = {
    .DeviceID = I2C_2,
	.CreateDevice = CreateI2C2,
	.CloseDevice = CloseI2C,
    .OpenDevice = OpenI2C2,
    .ReadDevice = ReadI2C,
    .WriteDevice = WriteI2C,
    .SeekDevice = SeekI2C,
    .ReadDeviceText = ReadI2CText,
    .WriteDeviceText = WriteI2CText,
    .SeekDeviceText = SeekI2CText,
	.DeviceIOCtl = I2CIOCtl,
    .DeviceName = "I2C2",
    };

DEVICE i2c2_device = {
    .device_io = (DEVICE_IO *)&i2c2_device_io,
    .flDeviceOpen = 0,
    .pDriverData = &i2c_info_2,
    .pData = &i2c2Data
    };

#endif

/************************************************************************/
/************************************************************************/

#if(_I2C0_)

int CreateI2C0(void)
{
    PINSEL_CFG_Type PinCfg;

    PinCfg.Funcnum = 1;
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PinCfg.Portnum = _I2C0_PORT_;
    PinCfg.Pinnum = _I2C0_SCL_PIN_;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = _I2C0_SDA_PIN_;
    PINSEL_ConfigPin(&PinCfg);

    I2C_Init(LPC_I2C0, _I2C0_CLOCK_RATE_);

    I2C_MonitorModeCmd(LPC_I2C0, DISABLE);

    I2C_Cmd(LPC_I2C0, ENABLE);


    create_sem(i2c_info_0.Sem);
	if(i2c_info_0.Sem == NULL)
	{
		PUT_MES_TO_LOG("I2C0: Semaphor = NULL", 0, 1);
		DEBUG_PUTS("I2C0: Semaphor = NULL\n");
		return DEVICE_ERROR;
	}

	DEBUG_PUTS("I2C0 Created\n");
	return 	DEVICE_OK;

}

DEVICE_HANDLE OpenI2C0(void)
{
	take_sem(i2c_info_0.Sem, portMAX_DELAY);

	return 	(DEVICE_HANDLE)&i2c0_device;
}

#endif

#if(_I2C1_)

int CreateI2C1(void)
{
    PINSEL_CFG_Type PinCfg;

    PinCfg.Funcnum = 3;
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PinCfg.Portnum = _I2C1_PORT_;
    PinCfg.Pinnum = _I2C1_SCL_PIN_;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = _I2C1_SDA_PIN_;
    PINSEL_ConfigPin(&PinCfg);

    I2C_Init(LPC_I2C1, _I2C1_CLOCK_RATE_);

    I2C_MonitorModeCmd(LPC_I2C1, DISABLE);

    I2C_Cmd(LPC_I2C1, ENABLE);

	create_sem(i2c_info_1.Sem);
    if(i2c_info_1.Sem == NULL)
	{
		PUT_MES_TO_LOG("I2C1: Semaphor = NULL", 0, 1);
		DEBUG_PUTS("I2C1: Semaphor = NULL\n");
		return DEVICE_ERROR;
	}

	DEBUG_PUTS("I2C1 Created\n");
	return 	DEVICE_OK;
}

DEVICE_HANDLE OpenI2C1(void)
{
	take_sem(i2c_info_1.Sem, portMAX_DELAY);
	//DEBUG_PUTS("I2C1 OPEN\n");

	return 	(DEVICE_HANDLE)&i2c1_device;
}

#endif

#if(_I2C2_)

int CreateI2C2(void)
{
    PINSEL_CFG_Type PinCfg;

    PinCfg.Funcnum = 2;
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PinCfg.Portnum = _I2C2_PORT_;
    PinCfg.Pinnum = _I2C2_SCL_PIN_;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = _I2C2_SDA_PIN_;
    PINSEL_ConfigPin(&PinCfg);

    I2C_Init(LPC_I2C2, _I2C2_CLOCK_RATE_);

    I2C_MonitorModeCmd(LPC_I2C2, DISABLE);

    I2C_Cmd(LPC_I2C2, ENABLE);

	create_sem(i2c_info_2.Sem);
    if(i2c_info_2.Sem == NULL)
	{
		PUT_MES_TO_LOG("I2C2: Semaphor = NULL", 0, 1);
		DEBUG_PUTS("I2C2: Semaphor = NULL\n");
		return DEVICE_ERROR;
	}

	DEBUG_PUTS("I2C2 Created\n");
	return 	DEVICE_OK;
}

DEVICE_HANDLE OpenI2C2(void)
{
	take_sem(i2c_info_2.Sem, portMAX_DELAY);

	return (DEVICE_HANDLE)&i2c2_device;
}

#endif


void CloseI2C(DEVICE_HANDLE handle)
{
	I2C_INFO_17xx *pI2CInfo = (I2C_INFO_17xx *)(((DEVICE *)handle)->pDriverData);
	give_sem(pI2CInfo->Sem);
	//DEBUG_PUTS("I2C1 CLOSE\n");
}


int ReadI2C(DEVICE_HANDLE handle, void * pDst, int count) {

    I2C_INFO_17xx *pI2CInfo = (I2C_INFO_17xx *)(((DEVICE *)handle)->pDriverData);
    I2C_M_SETUP_Type PacketCfg;

        I2CData *pData = (I2CData *)(((DEVICE *)handle)->pData);
        /*
         Читаем
         */
		PacketCfg.sl_addr7bit = pI2CInfo->Dev_Adr;
		PacketCfg.sl_mem_addr = pI2CInfo->Mem_Adr;
        PacketCfg.tx_data = 0;
        PacketCfg.tx_length = 0;
        PacketCfg.rx_data = (uint8_t *)pDst;
        PacketCfg.rx_length = (uint32_t)count;
        PacketCfg.retransmissions_max = 0;
  
        I2C_MasterTransferData((LPC_I2C_TypeDef *)pI2CInfo->pI2C, &PacketCfg, I2C_TRANSFER_POLLING);

        pData->rx_count += PacketCfg.rx_count;

       // give_sem(pI2CInfo->Sem);

        return (int)PacketCfg.rx_count;
}

int WriteI2C(DEVICE_HANDLE handle, const void * pSrc, int count)
{
    I2C_INFO_17xx *pI2CInfo = (I2C_INFO_17xx *)(((DEVICE *)handle)->pDriverData);
    I2C_M_SETUP_Type PacketCfg;

        I2CData *pData = (I2CData *)(((DEVICE *)handle)->pData);

		PacketCfg.sl_addr7bit = pI2CInfo->Dev_Adr;
        PacketCfg.sl_mem_addr = pI2CInfo->Mem_Adr;
        PacketCfg.tx_data = (uint8_t *)pSrc;
        PacketCfg.tx_length = (uint32_t)count;
        PacketCfg.rx_data = 0;
        PacketCfg.rx_length = 0;
        PacketCfg.retransmissions_max = 0;
  
        I2C_MasterTransferData((LPC_I2C_TypeDef *)pI2CInfo->pI2C, &PacketCfg, I2C_TRANSFER_POLLING);

        pData->tx_count += PacketCfg.tx_count;

        //give_sem(pI2CInfo->Sem);

        return (int)PacketCfg.tx_count;
}

int SeekI2C(DEVICE_HANDLE handle, uint32_t  mem, uint8_t dev)
{
    I2C_INFO_17xx *pI2CInfo = (I2C_INFO_17xx *)(((DEVICE *)handle)->pDriverData);

        pI2CInfo->Dev_Adr = dev;
        pI2CInfo->Mem_Adr = mem;
		return 0;

}


int ReadI2CText(DEVICE_HANDLE handle, void * pDst, int count)
{
#if(_TEXT_)	

    I2CData *pData = (I2CData *)(((DEVICE *)handle)->pData);
    char *BufferText;
	int sprintf_count;
	
	if(count >= 0)
	{
		memset(pDst, 0, count);
		BufferText = pvPortMalloc(300);
		if(BufferText != NULL)
        {
			sprintf_count = sprintf(BufferText, "Recive - %10u\n", pData->rx_count);
            sprintf_count += sprintf(BufferText + sprintf_count, "Transmit %10u\nRecive buf:\n", pData->tx_count);
                    
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

int WriteI2CText(DEVICE_HANDLE handle, void * pSrc, int count)
{
    return -1;
}

int SeekI2CText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
#if(_TEXT_)
//    I2C_INFO_17xx *pI2CInfo = (I2C_INFO_17xx *)(((DEVICE *)handle)->pDriverData);
        I2CData *pData = (I2CData *)(((DEVICE *)handle)->pData);
        switch(origin)
        {
            case 0:
                if(offset < MAX_I2C_DATA_OFFSET)
                {
                    pData->DataPoint = offset;
                }
                else
                {
                    pData->DataPoint = 0;
                }
                break;
            case 1:
                if((pData->DataPoint + offset) < MAX_I2C_DATA_OFFSET)
                {
                    pData->DataPoint = pData->DataPoint + offset;
                }
                else
                {
                    pData->DataPoint = 0;
                } 
                break;
            case 2:
                if(offset < MAX_I2C_DATA_OFFSET)
                {
                    pData->DataPoint = MAX_I2C_DATA_OFFSET - offset;
                }
                else
                {
                    pData->DataPoint = 0;
                } 
                break;
            default:
                //give_sem(pI2CInfo->Sem);

                pData->DataPoint = 0;
                return 0;             
        }
        return pData->DataPoint;
#else
	return 0;
#endif
}

static int I2CIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData)
{
	return -1;
}

#endif
