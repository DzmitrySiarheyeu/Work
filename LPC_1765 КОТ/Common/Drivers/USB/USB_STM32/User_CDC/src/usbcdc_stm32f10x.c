#include "Config.h"
#if(_USBCDC_)


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "usbcdc_stm32f10x.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "debug_system.h"


static int CreateUSBCDC(void);
static DEVICE_HANDLE OpenUSBCDC(void);
static void CloseUSBCDC(DEVICE_HANDLE handle);
static int ReadUSBCDC(DEVICE_HANDLE handle, void * dst, int count);
static int WriteUSBCDC(DEVICE_HANDLE handle, void * dst, int count);
static int SeekUSBCDC(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
static int WriteUSBCDCText(DEVICE_HANDLE handle, void * pSrc, int count);
static int ReadUSBCDCText(DEVICE_HANDLE handle, void * pDst, int count);
static int SeekUSBCDCText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
static int USBCDCIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData);

USBCDC_INFO_STM32F1 usbcdc_info = {
								 .rx_buf.in = 0, 
								 .rx_buf.out = 0, 
								 .tx_buf.in = 0, 
								 .tx_buf.out = 0, 
								 .waitRxBufferFree = 0,
								 .overflow = 0, 
								 .tx_empty = 1, 
								 .tx_full = 0, 
								 .Sem_rx = NULL, 
								 .Sem_tx = NULL, 
								 .time_out_value = USBCDC_RX_TIME_OUT_VALUE
								 };

USBCDCData  usbcdcData = {
    .tx_count = 0,
    .rx_count = 0,
    .DataPoint = 0,
    };

const DEVICE_IO usbcdc_device_io = {
    .DeviceID = USBCDC,
    .OpenDevice = OpenUSBCDC,
	.CreateDevice =	CreateUSBCDC,
	.CloseDevice = CloseUSBCDC,
    .ReadDevice = ReadUSBCDC,
    .WriteDevice = WriteUSBCDC,
    .SeekDevice = SeekUSBCDC,
    .ReadDeviceText = WriteUSBCDCText,
    .WriteDeviceText = ReadUSBCDCText,
    .SeekDeviceText = SeekUSBCDCText,
    .DeviceIOCtl = USBCDCIOCtl,
    .DeviceName = "USBCDC",
    };

DEVICE usbcdc_device = {
    .device_io = (DEVICE_IO *)&usbcdc_device_io,
    .flDeviceOpen = 0,
    .pDriverData = &usbcdc_info,
    .pData = &usbcdcData
    };

static int CreateUSBCDC(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
													
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);								
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;											
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;									
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;									
  	GPIO_Init(GPIOE, &GPIO_InitStructure);  		  //  	   ????????? ??????????? ??????

	Set_System();
    Set_USBClock();
  	USB_Interrupts_Config();
  	USB_Init();

	vSemaphoreCreateBinary(usbcdc_info.Sem_tx);
	if(usbcdc_info.Sem_tx == NULL)
	{
		//PUT_MES_TO_LOG("UART3: Semaphor_tx = NULL", 0, 1);
		DEBUG_PUTS("USBCDC: Semaphor_tx = NULL\n");
		return DEVICE_ERROR;
	}

	vSemaphoreCreateBinary(usbcdc_info.Sem_rx);
	if(usbcdc_info.Sem_rx == NULL)
	{
		//PUT_MES_TO_LOG("UART3: Semaphor_rx = NULL", 0, 1);
		DEBUG_PUTS("USBCDC: Semaphor_rx = NULL\n");
		return DEVICE_ERROR;
	}

	if(xSemaphoreTake(usbcdc_info.Sem_rx, 0) == pdFALSE)
	{
		DEBUG_PUTS("USBCDC: unable to take Semaphor_rx \n");
		return DEVICE_ERROR;
	}

    return DEVICE_OK; // true
}

static DEVICE_HANDLE OpenUSBCDC(void)
{
    if(usbcdc_device.flDeviceOpen == DEVICE_CLOSE)
	{
		 usbcdc_device.flDeviceOpen = DEVICE_OPEN;
	}
	else
	{
		//PUT_MES_TO_LOG("UART0 RE_OPEN", 0, 2);
		DEBUG_PUTS("USBCDC RE_OPEN\n");
	}
	return 	&usbcdc_device;
}

static void CloseUSBCDC(DEVICE_HANDLE handle)
{
	DEVICE *pDevice = (DEVICE *)handle;

	if(pDevice->flDeviceOpen == DEVICE_OPEN)
	{
		 pDevice->flDeviceOpen = DEVICE_CLOSE;
	}
	else
	{
		PUT_MES_TO_LOG("USBCDC RE_CLOSE", 0, 2);
		DEBUG_PUTS("USBCDC RE_CLOSE\n");
	}
}


void StartTransmitUSB_CDC(USBCDC_INFO_STM32F1 *pUSBCDCInfo)
{
	 vPortEnterCritical();
	 StartTransmitUSB(pUSBCDCInfo);
	 vPortExitCritical();
}


static int WriteUSBCDC (DEVICE_HANDLE handle, void * pSrc, int count)
{
    USBCDC_INFO_STM32F1 *pUSBCDCInfo = (USBCDC_INFO_STM32F1 *)(((DEVICE *)handle)->pDriverData);
    USBCDC_TX_BUF *p = &(pUSBCDCInfo->tx_buf);
	int i = 0; 
	int free_space = 0;
    for(i = 0; i < count; i++)
    {
		 if(p->in >= p->out) free_space = USBCDC_BUF_TX_SIZE - (p->in - p->out);   //  ???????? ??????? ?????????? ????? ? ??????
    	 else free_space = p->out - p->in;
		 while(free_space == 1)
         {
             StartTransmitUSB_CDC(pUSBCDCInfo);
             xSemaphoreTake(pUSBCDCInfo->Sem_tx, portMAX_DELAY );
			 if(p->in >= p->out) free_space = USBCDC_BUF_TX_SIZE - (p->in - p->out);   //  ???????? ??????? ?????????? ????? ? ??????
    	 	 else free_space = p->out - p->in;
         }
         p->buf[p->in++] = ((uint8_t *)pSrc)[i];
         if(p->in == USBCDC_BUF_TX_SIZE) p->in = 0;
     }
	 StartTransmitUSB_CDC(pUSBCDCInfo);
     return count;

}

static int ReadUSBCDC(DEVICE_HANDLE handle, void * pDst, int count) {
 
    USBCDC_INFO_STM32F1 *pUSBCDCInfo = (USBCDC_INFO_STM32F1 *)(((DEVICE *)handle)->pDriverData);
    USBCDC_RX_BUF *p = &(pUSBCDCInfo->rx_buf);
    USBCDCData *pData = (USBCDCData *)(((DEVICE *)handle)->pData);
	uint32_t time_out = pUSBCDCInfo->time_out_value;
    int i = 0;
	
    for(i = 0; i < count; i++)
    {
		// ???? ???????? ????? - ????? ????
        while(p->out == p->in) 
        {
			// ?????? ???????? 
			// ???????? ????? 5 ??
			if(xSemaphoreTake(pUSBCDCInfo->Sem_rx, time_out) == pdFALSE)
            {
				// ?? ?????? ????????? ??????? - ???????
                goto read_exit;
            }
			//time_out = 5;
        }

		// ?????? ?????? ?? ??????
        ((uint8_t *)pDst)[i] = p->buf[p->out++];                             
        if(p->out == USBCDC_BUF_RX_SIZE) 
			p->out = 0;
		        
        pData->rx_count++;   //  ?????????? ???????? ????
    } 

	read_exit:
	// ???? ???? ???????????? ?????? ? ????????? ?? ???? ????????
	if(pUSBCDCInfo->waitRxBufferFree && i)
	{
		// ???????? ????????
		pUSBCDCInfo->waitRxBufferFree = 0;
		// ????????????? ???? ?????????? ? ???? ??????????? ?????????? ???????? ? ?????
		SetEPRxValid(ENDP3);
	}  
    return i;
}


static int SeekUSBCDC(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
    return -1;
}

static int USBCDCIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData)
{
	USBCDC_INFO_STM32F1 *pUSBCDCInfo = (USBCDC_INFO_STM32F1 *)(((DEVICE *)handle)->pDriverData);
    USBCDC_RX_BUF *p_rx = &(pUSBCDCInfo->rx_buf);
    USBCDC_TX_BUF *p_tx = &(pUSBCDCInfo->tx_buf);

    uint16_t count = 0;

    switch(code)
	{
		case DEVICE_VALID_CHECK:
			*pData = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12);
			return 0; 

		case RX_DATA_COUNT:
            if(p_rx->in >= p_rx->out) count = p_rx->in - p_rx->out;
            else count = (USBCDC_BUF_RX_SIZE - p_rx->out + p_rx->in);
            memcpy(pData, (uint8_t *)&count, 2);
            break;

        case FREE_TX_SPACE:
            if(p_tx->out > p_tx->in) count = p_tx->out - p_tx->in;
            else count = (USBCDC_BUF_TX_SIZE - p_tx->in + p_tx->out);
            memcpy(pData, (uint8_t *)&count, 2);
            break;

		default: 
			return -1;
	}

	return -1;
}

static int WriteUSBCDCText(DEVICE_HANDLE handle, void * pSrc, int count)
{
#if(_TEXT_)
	return -1;
#else
    return -1;
#endif
}

static int ReadUSBCDCText(DEVICE_HANDLE handle, void * pDst, int count)
{
#if(_TEXT_)
    return -1;
#else
	return -1;
#endif
}

static int SeekUSBCDCText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
#if(_TEXT_)
    return -1;
#else
	return -1;
#endif
}

#endif
