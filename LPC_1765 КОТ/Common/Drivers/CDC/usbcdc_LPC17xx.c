#include "Config.h"
#if(_USBCDC_)

#include <LPC17xx.H>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "usbcdc_lpc17xx.h"
#include "debug_system.h"

#include "type.h"
#include "usbhw.h"
#include "usbcfg.h"
#include "cdcuser.h"
#include "usbcore.h"

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

USBCDC_INFO_LPC17XX usbcdc_info = {
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
    USB_Init();                               // USB Initialization
    USB_Connect(TRUE);                        // USB Connect

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
		//PUT_MES_TO_LOG("UART RE_CLOSE", 0, 2);
		DEBUG_PUTS("USBCDC RE_CLOSE\n");
	}
}

/*static int ReadUSBCDC(DEVICE_HANDLE handle, void * dst, int count)
{
    USBCDC_INFO_LPC17XX *pUSBCDCInfo = (USBCDC_INFO_LPC17XX *)(((DEVICE *)handle)->pDriverData);
    if(xSemaphoreTake( pUSBCDCInfo->Sem, 1000 ) == pdTRUE)
    {
        USBCDC_BUF *p = &(pUSBCDCInfo->rx_buf);
        USBCDCData *pData = (USBCDCData *)(((DEVICE *)handle)->pData);
        int i;

        for(i = 0; i < count; i++)
        {
            if (p->in == p->out)
            {
              xSemaphoreGive(pUSBCDCInfo->Sem);
              return i;
            }
            ((uint8_t *)dst)[i] = p->buf[p->out++];
            if(p->out == USBCDC_BUF_SIZE) p->out = 0;
            pData->rx_count++;
        }
        xSemaphoreGive(pUSBCDCInfo->Sem);
        return count;
    }
    return -1;
}

static int WriteUSBCDC(DEVICE_HANDLE handle, void * dst, int count)
{
    USBCDC_INFO_LPC17XX *pUSBCDCInfo = (USBCDC_INFO_LPC17XX *)(((DEVICE *)handle)->pDriverData);
    USBCDC_BUF *p = &(pUSBCDCInfo->tx_buf);
    USBCDCData *pData = (USBCDCData *)(((DEVICE *)handle)->pData);
    uint8_t buff_full_flag = 0;

	if( !USB_Configuration )
		return -1;

    if(xSemaphoreTake( pUSBCDCInfo->Sem, 1000 ) == pdTRUE)
    {
        // Максимальный размер, который мы можем передать без занесения в буфер это USB_CDC_BUFSIZE
        if(count > USB_CDC_BUFSIZE)
        {
            // Еслт в буфере нет места
            if ((p->in + 1) == p->out)
            {
                DEBUG_PUTS("USBCDC: Transmit buffer is full (64 bytes transmitted)");
                // Устанавливаем флаг, но не выходим, т.к. мы можем передать USB_CDC_BUFSIZE байт в любом случае
                buff_full_flag = 1;
            }

            
            int i;
            // Все, что больше USB_CDC_BUFSIZE байт надо занести в буфер
            for(i = USB_CDC_BUFSIZE; i < count; i++)
            {
                p->buf[p->in++] = ((uint8_t *)dst)[i];
                if(p->in >= USBCDC_BUF_SIZE) p->in = 0;
                pData->tx_count++;
            }

            // Отсылаем USB_CDC_BUFSIZE байт
            USB_WriteEP(CDC_DEP_IN, (unsigned char *)dst, USB_CDC_BUFSIZE);
            pData->tx_count += USB_CDC_BUFSIZE;

            if(buff_full_flag == 1)
            {
                xSemaphoreGive(pUSBCDCInfo->Sem);
                return - 1;
            }
        }
        else
        {
            USB_WriteEP(CDC_DEP_IN, (unsigned char *)dst, count);
            pData->tx_count += count;
        }

        xSemaphoreGive(pUSBCDCInfo->Sem);
        return count;
    }
  
    return -1;
}*/
void StartTransmitUSB(USBCDC_INFO_LPC17XX *pUSBCDCInfo);

void StartTransmitUSB_CDC(USBCDC_INFO_LPC17XX *pUSBCDCInfo)
{
	vPortEnterCritical();

	if(pUSBCDCInfo->busy != 1){
	 	StartTransmitUSB(pUSBCDCInfo);
	}
	vPortExitCritical();
}
/*{
	USBCDC_BUF *p = &(pUSBCDCInfo->tx_buf);
	int count = 0;
		vPortEnterCritical();
		if(p->in >= p->out) count = p->in - p->out;   //  выясняем сколько свободного места в буфере
    	else count = USBCDC_BUF_SIZE - p->out + p->in;
		if(pUSBCDCInfo->busy == 0)
        {
			if(p->out != p->in)// || pUSBCDCInfo->tx_empty != 1)
			{
				// Если нам надо передать USBCDC_BUF_SIZE байт или меньше
		        if(count <= USB_CDC_BUFSIZE - 1)
				{
					// Устанавливаем флаг занятости
					pUSBCDCInfo->busy = 1;
					// Передаем без заненсения в буфер
					USB_WriteEP(CDC_DEP_IN, (unsigned char *)&(p->buf[p->out]), count);
					p->out += count;
				}
				// Нужно заносить в буфер
				else
				{
					pUSBCDCInfo->busy = 1;
					// Передаем максимально возможные пакет (USBCDC_BUF_SIZE) без заненсения в буфер,
					// чтобы вызвать прерывания. Остальное передается в прерываниях.
					USB_WriteEP(CDC_DEP_IN, (unsigned char *)&(p->buf[p->out]), USB_CDC_BUFSIZE - 1);
					p->out += (USB_CDC_BUFSIZE - 1);
				}
				
				if(p->out >= USBCDC_BUF_SIZE) p->out -= USBCDC_BUF_SIZE;
				if(p->out == p->in)
	        	{
					pUSBCDCInfo->tx_empty = 1; 
				}
			}	
        }
		vPortExitCritical();
}		*/


static int WriteUSBCDC (DEVICE_HANDLE handle, void * pSrc, int count)
{
    USBCDC_INFO_LPC17XX *pUSBCDCInfo = (USBCDC_INFO_LPC17XX *)(((DEVICE *)handle)->pDriverData);
    USBCDC_TX_BUF *p = &(pUSBCDCInfo->tx_buf);
	int i = 0; 
	int free_space = 0;
    for(i = 0; i < count; i++)
    {
		 if(p->in >= p->out) free_space = USBCDC_BUF_TX_SIZE - (p->in - p->out);   //  выясняем сколько свободного места в буфере
    	 else free_space = p->out - p->in;
		 while(free_space == 1)
         {
             StartTransmitUSB_CDC(pUSBCDCInfo);
             xSemaphoreTake(pUSBCDCInfo->Sem_tx, portMAX_DELAY );
			 if(p->in >= p->out) free_space = USBCDC_BUF_TX_SIZE - (p->in - p->out);   //  выясняем сколько свободного места в буфере
    	 	 else free_space = p->out - p->in;
         }
         p->buf[p->in++] = ((uint8_t *)pSrc)[i];
         if(p->in == USBCDC_BUF_TX_SIZE) p->in = 0;
     }
	 StartTransmitUSB_CDC(pUSBCDCInfo);
     return count;

  /*  for(i = 0; i < count; i++)
    {
		// Пока буфер полон
        while(p->in == p->out && pUSBCDCInfo->tx_empty == 0)
        {
			StartTransmitUSB(pUSBCDCInfo, i);
            xSemaphoreTake(pUSBCDCInfo->Sem_tx, portMAX_DELAY );
        }

		// Заносим в буфер
        p->buf[p->in++] = ((uint8_t *)pSrc)[i];
        if(p->in == USBCDC_BUF_SIZE) 
			p->in = 0;

		// Буфер не пуст
        pUSBCDCInfo->tx_empty = 0;
    }
	StartTransmitUSB(pUSBCDCInfo, i);
    return count; */
}

static int ReadUSBCDC(DEVICE_HANDLE handle, void * pDst, int count) {
 
    USBCDC_INFO_LPC17XX *pUSBCDCInfo = (USBCDC_INFO_LPC17XX *)(((DEVICE *)handle)->pDriverData);
    USBCDC_RX_BUF *p = &(pUSBCDCInfo->rx_buf);
    USBCDCData *pData = (USBCDCData *)(((DEVICE *)handle)->pData);
	uint32_t time_out = pUSBCDCInfo->time_out_value;
    int i = 0;
	
    for(i = 0; i < count; i++)
    {
		// Если счетчики равны - буфер пуст
        while(p->out == p->in) 
        {
			// Вместо таймаута 
			// Короткая пауза 5 мс
			if(xSemaphoreTake(pUSBCDCInfo->Sem_rx, time_out) == pdFALSE)
            {
				// Не смогли захватить семафор - таймаут
                goto read_exit;
            }
			//time_out = 5;
        }

		// Читаем данные из буфера
        ((uint8_t *)pDst)[i] = p->buf[p->out++];                             
        if(p->out == USBCDC_BUF_RX_SIZE) 
			p->out = 0;
		        
        pData->rx_count++;   //  Статистика принятых байт
    } 

	read_exit:
	// если ждем освобождения буфера и прочитали не нуль символов
	if(pUSBCDCInfo->waitRxBufferFree && i)
	{
		// отменяем ожидание
		pUSBCDCInfo->waitRxBufferFree = 0;
		// устанавливаем флаг прерывания и даем возможность попытаться записать в буфер
		LPC_USB->USBEpIntSet |= (1 << 4);
	}  
    return i;
}


static int SeekUSBCDC(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
    return -1;
}

static int USBCDCIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData)
{
	USBCDC_INFO_LPC17XX *pUSBCDCInfo = (USBCDC_INFO_LPC17XX *)(((DEVICE *)handle)->pDriverData);
    USBCDC_RX_BUF *p_rx = &(pUSBCDCInfo->rx_buf);
    USBCDC_TX_BUF *p_tx = &(pUSBCDCInfo->tx_buf);

    uint16_t count = 0;

    switch(code)
	{
		case DEVICE_VALID_CHECK:
			*pData = USB_ConnectionStatus;
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
  /*  USBCDC_INFO_LPC17XX *pUSBCDCInfo = (USBCDC_INFO_LPC17XX *)(((DEVICE *)handle)->pDriverData);
    if(xSemaphoreTake( pUSBCDCInfo->Sem, 1000 ) == pdTRUE)
    {
        USBCDCData *pData = (USBCDCData *)(((DEVICE *)handle)->pData);
        if(count >= 0)
        {
            memset(pDst, 0, count);
            if((pData->DataPoint + count) > MAX_USBCDC_DATA_OFFSET)
            {
                count = MAX_USBCDC_DATA_OFFSET - pData->DataPoint;
            }
            if(xSemaphoreTake(xDataText_Sem, 1000 ) == pdTRUE)
            {
                sprintf((char *)&BufferText[0], "%10lu", pData->rx_count);
                strcpy((char *)&BufferText[10], " rx;\n");
                sprintf((char *)&BufferText[15], "%10lu", pData->tx_count);
                strcpy((char *)&BufferText[25], " tx;\n");
                    
                memcpy(pDst, (uint8_t *)(((uint8_t *)BufferText) + pData->DataPoint), count);

                xSemaphoreGive(xDataText_Sem);
                xSemaphoreGive(pUSBCDCInfo->Sem);
                pData->DataPoint += count;
                return count;
            }
        }
        xSemaphoreGive(pUSBCDCInfo->Sem);
    }*/
    return -1;
#else
	return -1;
#endif
}

static int SeekUSBCDCText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
#if(_TEXT_)
   /* USBCDC_INFO_LPC17XX *pUSBCDCInfo = (USBCDC_INFO_LPC17XX *)(((DEVICE *)handle)->pDriverData);
    if(xSemaphoreTake( pUSBCDCInfo->Sem, 1000 ) == pdTRUE)
    {
        USBCDCData *pData = (USBCDCData *)(((DEVICE *)handle)->pData);
        switch(origin)
        {
            case 0:
                if(offset < MAX_USBCDC_DATA_OFFSET)
                {
                    pData->DataPoint = offset;
                }
                else
                {
                    pData->DataPoint = 0;
                    return -1;
                }
                break;
            case 1:
                if((pData->DataPoint + offset) < MAX_USBCDC_DATA_OFFSET)
                {
                    pData->DataPoint = pData->DataPoint + offset;
                }
                else
                {
                    pData->DataPoint = 0;
                    return -1;
                } 
                break;
            case 2:
                if(offset < MAX_USBCDC_DATA_OFFSET)
                {
                    pData->DataPoint = MAX_USBCDC_DATA_OFFSET - offset;
                }
                else
                {
                    pData->DataPoint = 0;
                    return -1;
                } 
                break;
            default:
                xSemaphoreGive(pUSBCDCInfo->Sem);
                pData->DataPoint = 0;
                return -1;
                break;             
        }
        xSemaphoreGive(pUSBCDCInfo->Sem);
        return pData->DataPoint;
    }*/
    return -1;
#else
	return -1;
#endif
}

#endif
