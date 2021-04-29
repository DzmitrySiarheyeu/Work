#include "ssp_lpc17xx.h"
#if(_SSP_)

#include "lpc17xx_ssp.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_clkpwr.h"

#include <string.h>
#include <stdint.h>
#include <stdio.h>


#if(_SSP0_)

static DEVICE_HANDLE OpenSSP0(void);
static int CreateSSP0(void);

#endif

#if(_SSP1_)

static DEVICE_HANDLE OpenSSP1(void);
static int CreateSSP1(void);

#endif

static void CloseSSP(DEVICE_HANDLE handle);
static int ReadSSP(DEVICE_HANDLE handle, void * dst, int count);
static int WriteSSP(DEVICE_HANDLE handle,const void * dst, int count);
static int SeekSSP(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
static int WriteSSPText(DEVICE_HANDLE handle, void * pSrc, int count);
static int ReadSSPText(DEVICE_HANDLE handle, void * pDst, int count);
static int SeekSSPText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
static int IOCtlSSP(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData);

#if(_SSP0_)

static SSP_INFO_17xx ssp_info_0 = { .pSSP = LPC_SSP0, .Sem = 0};

SSPData  ssp0Data = {
    .tx_count = 0,
    .rx_count = 0,
    .DataPoint = 0
    };

const DEVICE_IO ssp0_device_io = {
    .DeviceID = SSP_0,
	.CreateDevice =	CreateSSP0,
	.CloseDevice = CloseSSP,
    .OpenDevice = OpenSSP0,
    .ReadDevice = ReadSSP,
    .WriteDevice = WriteSSP,
    .SeekDevice = SeekSSP,
    .ReadDeviceText = ReadSSPText,
    .WriteDeviceText = WriteSSPText,
    .SeekDeviceText = SeekSSPText,
	.DeviceIOCtl = IOCtlSSP,
    .DeviceName = "SSP0"
    };


DEVICE ssp0_device = {
    .device_io = (DEVICE_IO *)&ssp0_device_io,
    .flDeviceOpen = 0,
    .pDriverData = &ssp_info_0,
    .pData = &ssp0Data
    };

#endif

#if(_SSP1_)

static SSP_INFO_17xx ssp_info_1 = { .pSSP = LPC_SSP1, .Sem = NULL};

SSPData  ssp1Data = {
    .tx_count = 0,
    .rx_count = 0,
    .DataPoint = 0,
    };

const DEVICE_IO ssp1_device_io = {
    .DeviceID = SSP_1,
	.CreateDevice =	CreateSSP1,
	.CloseDevice = CloseSSP,
    .OpenDevice = OpenSSP1,
    .ReadDevice = ReadSSP,
    .WriteDevice = WriteSSP,
    .SeekDevice = SeekSSP,
    .ReadDeviceText = ReadSSPText,
    .WriteDeviceText = WriteSSPText,
    .SeekDeviceText = SeekSSPText,
	.DeviceIOCtl = IOCtlSSP,
    .DeviceName = "SSP1",
    };


DEVICE ssp1_device = {
    .device_io = (DEVICE_IO *)&ssp1_device_io,
    .flDeviceOpen = 0,
    .pDriverData = &ssp_info_1,
    .pData = &ssp1Data
    };

#endif

/************************************************************************/
/************************************************************************/

#if(_SSP0_)

int CreateSSP0(void)
{
	 PINSEL_CFG_Type PinCfg;
    SSP_CFG_Type SSP_ConfigStruct;

//	GPIO_SetDir(_SSP0_PORT_, (1 << _SSP0_SCK_PIN_), 1);
//	GPIO_SetDir(_SSP0_PORT_, (1 << _SSP0_MOSI_PIN_), 1);

	if(_SSP0_PORT_ == 0)
    	PinCfg.Funcnum = 2;
	else if(_SSP0_PORT_ == 1)
		PinCfg.Funcnum = 3;

    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PinCfg.Portnum = _SSP0_PORT_;
    PinCfg.Pinnum = _SSP0_SCK_PIN_;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = _SSP0_MISO_PIN_;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = _SSP0_MOSI_PIN_;
    PINSEL_ConfigPin(&PinCfg);

	CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_SSP0, CLKPWR_PCLKSEL_CCLK_DIV_2);

	if(_SSP0_CPHA_ == 0) // по фронту 
	{
		SSP_ConfigStruct.CPHA = SSP_CPHA_FIRST;
	}
	else if(_SSP0_CPHA_ == 1) // по срезу
	{
		SSP_ConfigStruct.CPHA = SSP_CPHA_SECOND;
	}

	if(_SSP0_CPOL_ == 0) // отрицательная
	{
		SSP_ConfigStruct.CPOL = SSP_CPOL_LO;
	}
	else if(_SSP0_CPOL_ == 1) // положительная
	{
		SSP_ConfigStruct.CPOL = SSP_CPOL_HI;
	}
	
    SSP_ConfigStruct.ClockRate = _SSP0_CLOCK_RATE_;
    SSP_ConfigStruct.Databit = SSP_DATABIT_8;
    SSP_ConfigStruct.Mode = SSP_MASTER_MODE;
    SSP_ConfigStruct.FrameFormat = SSP_FRAME_SPI;

    // Initialize SSP peripheral with parameter given in structure above
    SSP_Init(LPC_SSP0, &SSP_ConfigStruct);

    // Enable SSP peripheral
    SSP_Cmd(LPC_SSP0, ENABLE);

	create_sem(ssp_info_0.Sem);
	if(ssp_info_0.Sem == NULL)
	{
		PUT_MES_TO_LOG("SSP0: Semaphor = NULL", 0, 1);
		DEBUG_PUTS("SSP0: Semaphor = NULL\n");
		return DEVICE_ERROR;
	}
	DEBUG_PUTS("SSP0 Created\n");
	return 	DEVICE_OK;

}

DEVICE_HANDLE OpenSSP0(void)
{

	take_sem(ssp_info_0.Sem, portMAX_DELAY);

	//DEBUG_PUTS("SSP0 OPEN\n");

	return 	(DEVICE_HANDLE)&ssp0_device;
}

#endif

#if(_SSP1_)

int CreateSSP1(void)
{
	PINSEL_CFG_Type PinCfg;
    SSP_CFG_Type SSP_ConfigStruct;

    PinCfg.Funcnum = 2;
    PinCfg.OpenDrain = 0;
    PinCfg.Pinmode = 0;
    PinCfg.Portnum = _SSP1_PORT_;
    PinCfg.Pinnum = _SSP1_SCK_PIN_;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = _SSP1_MISO_PIN_;
    PINSEL_ConfigPin(&PinCfg);
    PinCfg.Pinnum = _SSP1_MOSI_PIN_;
    PINSEL_ConfigPin(&PinCfg);

    if(_SSP1_CPHA_ == 0) // по фронту 
		SSP_ConfigStruct.CPHA = SSP_CPHA_FIRST;
	else if(_SSP1_CPHA_ == 1) // по срезу
		SSP_ConfigStruct.CPHA = SSP_CPHA_SECOND;

	if(_SSP1_CPOL_ == 0) // отрицательная
		SSP_ConfigStruct.CPOL = SSP_CPOL_LO;
	else if(_SSP1_CPOL_ == 1) // положительная
		SSP_ConfigStruct.CPOL = SSP_CPOL_HI;
	
    SSP_ConfigStruct.ClockRate = _SSP1_CLOCK_RATE_;
    SSP_ConfigStruct.Databit = SSP_DATABIT_8;
    SSP_ConfigStruct.Mode = SSP_MASTER_MODE;
    SSP_ConfigStruct.FrameFormat = SSP_FRAME_SPI;

    // Initialize SPI peripheral with parameter given in structure above
    SSP_Init(LPC_SSP1, &SSP_ConfigStruct);

    // Enable SSP peripheral
    SSP_Cmd(LPC_SSP1, ENABLE);

	create_sem(ssp_info_1.Sem);
	if(ssp_info_1.Sem == NULL)
	{
		PUT_MES_TO_LOG("SSP1: Semaphor = NULL", 0, 1);
		DEBUG_PUTS("SSP1: Semaphor = NULL\n");
		return DEVICE_ERROR;
	}

	DEBUG_PUTS("SSP1 Created\n");
	return 	DEVICE_OK;	
}

DEVICE_HANDLE OpenSSP1(void)
{

	take_sem(ssp_info_1.Sem, portMAX_DELAY);

	return 	(DEVICE_HANDLE)&ssp1_device;
}

#endif

int SeekSSP(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
    return -1;
}


int WriteSSPText(DEVICE_HANDLE handle, void * pSrc, int count)
{
    return -1;
}

int ReadSSPText(DEVICE_HANDLE handle, void * pDst, int count)
{
#if(_TEXT_)
    SSPData *pData = (SSPData *)(((DEVICE *)handle)->pData);
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

int SeekSSPText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
#if(_TEXT_)
        SSPData *pData = (SSPData *)(((DEVICE *)handle)->pData);
        switch(origin)
        {
            case 0:
                if(offset < MAX_SSP_DATA_OFFSET)
                {
                    pData->DataPoint = offset;
                }
                else
                {
                    pData->DataPoint = 0;
                }
                break;
            case 1:
                if((pData->DataPoint + offset) < MAX_SSP_DATA_OFFSET)
                {
                    pData->DataPoint = pData->DataPoint + offset;
                }
                else
                {
                    pData->DataPoint = 0;
                } 
                break;
            case 2:
                if(offset < MAX_SSP_DATA_OFFSET)
                {
                    pData->DataPoint = MAX_SSP_DATA_OFFSET - offset;
                }
                else
                {
                    pData->DataPoint = 0;
                } 
                break;
            default:
                pData->DataPoint = 0;
                return 0;            
        }
        return pData->DataPoint;
#else
	return 0;
#endif
}

static int IOCtlSSP(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData)
{
	return -1;
}


void CloseSSP(DEVICE_HANDLE handle)
{
	SSP_INFO_17xx *pSpiInfo = (SSP_INFO_17xx *)(((DEVICE *)handle)->pDriverData);
	give_sem(pSpiInfo->Sem);
	//DEBUG_PUTS("SSP0 CLOSE\n");
}


int WriteSSP(DEVICE_HANDLE handle,const void * pSrc, int count)
{
    uint8_t *temp = (uint8_t *)pSrc;

    SSP_INFO_17xx *pSpiInfo = (SSP_INFO_17xx *)(((DEVICE *)handle)->pDriverData);
    SSPData *pData = (SSPData *)(((DEVICE *)handle)->pData);

    int i = 0;
    for(i = 0; i < count; i++)
    {
        SSP_SendData((LPC_SSP_TypeDef *)pSpiInfo->pSSP, *(temp + i));
        while(SSP_GetStatus((LPC_SSP_TypeDef *)pSpiInfo->pSSP, SSP_SR_TNF)==0);
        pData->tx_count++;
    }
    while(SSP_GetStatus((LPC_SSP_TypeDef *)pSpiInfo->pSSP, SSP_SR_BSY)==SET);
    do
    {
        SSP_ReceiveData((LPC_SSP_TypeDef *)pSpiInfo->pSSP);
    }
    while(SSP_GetStatus((LPC_SSP_TypeDef *)pSpiInfo->pSSP, SSP_SR_RNE));
    return count;
}

int ReadSSP(DEVICE_HANDLE handle, void * pDst, int count) {

    uint8_t temp;
    SSP_INFO_17xx *pSpiInfo = (SSP_INFO_17xx *)(((DEVICE *)handle)->pDriverData);
    SSPData *pData = (SSPData *)(((DEVICE *)handle)->pData);


    int i = 0;
    uint8_t *buf = (uint8_t *)pDst;
    do
    {
        temp = (uint8_t)SSP_ReceiveData((LPC_SSP_TypeDef *)pSpiInfo->pSSP);
    }
    while(SSP_GetStatus((LPC_SSP_TypeDef *)pSpiInfo->pSSP, SSP_SR_RNE));
    for(i = 0; i < count; i++)
    {
        SSP_SendData((LPC_SSP_TypeDef *)pSpiInfo->pSSP, 0);
        //while(SSP_GetStatus((LPC_SSP_TypeDef *)pSpiInfo->pSPI, SSP_SR_BSY));
        while(SSP_GetStatus((LPC_SSP_TypeDef *)pSpiInfo->pSSP, SSP_SR_RNE) == 0);
        temp = (uint8_t)SSP_ReceiveData((LPC_SSP_TypeDef *)pSpiInfo->pSSP);
        *buf = temp;
        buf++;
        pData->rx_count++;
    }
    return count;
}


#endif




