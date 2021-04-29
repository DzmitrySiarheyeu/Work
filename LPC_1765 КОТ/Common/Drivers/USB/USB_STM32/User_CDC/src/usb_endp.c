/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : usb_endp.c
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : Endpoint routines
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"

#include "config.h"
#include "usbcdc_stm32f10x.h"



uint8_t cdc_in_out_buf[VIRTUAL_COM_PORT_DATA_SIZE];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


void EP1_IN_Callback (void)
{
}


/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP2_IN_Callback (void)
{

	static signed portBASE_TYPE xHigherPriorityTaskWoken;

    uint16_t free_space = 0;
    USBCDC_INFO_STM32F1 *pUSBCDCInfo = (USBCDC_INFO_STM32F1 *)usbcdc_device.pDriverData;
    USBCDC_TX_BUF *p = &(pUSBCDCInfo->tx_buf);      //  буфер приема
	pUSBCDCInfo->busy = 0;
	StartTransmitUSB(pUSBCDCInfo);
    if(p->in >= p->out) free_space = USBCDC_BUF_TX_SIZE - (p->in - p->out);   //  выясняем сколько свободного места в буфере
    else free_space = p->out - p->in;

    if(free_space > (USBCDC_BUF_TX_SIZE/2)) 
		xSemaphoreGiveFromISR( pUSBCDCInfo->Sem_tx, &xHigherPriorityTaskWoken );   // Если кто-то в ожидании свободного места в буфере, прекращаем это ожидание   
}



void StartTransmitUSB(USBCDC_INFO_STM32F1 *pUSBCDCInfo)
{
    int i = 0;
    USBCDC_TX_BUF *p = &(pUSBCDCInfo->tx_buf);      //  буфер приема
	if(pUSBCDCInfo->busy ==1)
		return;
    while(i < VIRTUAL_COM_PORT_DATA_SIZE) // пока передающий буфер пуст или не закончатся данные
    {
        if(p->out == p->in)// && pUSBCDCInfo->tx_empty == 1)
        {
            break; // выходим, данные закончились
        }
		pUSBCDCInfo->busy = 1;
        cdc_in_out_buf[i++] = p->buf[p->out++];

        if(p->out == USBCDC_BUF_TX_SIZE)  p->out = 0;
    }
	if(i == 0)
		pUSBCDCInfo->busy = 0;
	else{
		USB_SIL_Write(EP2_IN, &cdc_in_out_buf[0], i); 
		SetEPTxValid(ENDP2); 
	}	
}


/*******************************************************************************
* Function Name  : EP3_OUT_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP3_OUT_Callback(void)
{
	USBCDC_INFO_STM32F1 *pUSBCDCInfo = (USBCDC_INFO_STM32F1 *)usbcdc_device.pDriverData;
    USBCDC_RX_BUF *p = &(pUSBCDCInfo->rx_buf);      //  буфер приема

	// свободное место в буфере
	uint16_t free_space = p->in >= p->out ? USBCDC_BUF_RX_SIZE - (p->in - p->out) : p->out - p->in;
	// если пакет не помещается в буфер
	if(free_space < VIRTUAL_COM_PORT_DATA_SIZE)
	{
		// ждем опустошения буфера
		pUSBCDCInfo->waitRxBufferFree = 1;
		return;	
	}

	int numBytesToRead, numBytesRead = 0;

  	// get data from USB into intermediate buffer
  	numBytesToRead = USB_SIL_Read(EP3_OUT, &cdc_in_out_buf[0]);//USB_ReadEP(CDC_DEP_OUT, &cdc_in_out_buf[0]);

	static signed portBASE_TYPE xHigherPriorityTaskWoken;

    do
    {
		p->buf[p->in++] = cdc_in_out_buf[numBytesRead++];

        if(p->in >= USBCDC_BUF_RX_SIZE) p->in = 0;
        if(p->in == p->out)  // Если дошли до конца буфера, будет перезатирание данных
        {
            p->out++;                          // увеличиваем указатель прочитанных байт (презапись байта приравнивается к его прочтению)
            pUSBCDCInfo->overflow = 1;           //  отмечаем что было переполнение
            if(p->out >= USBCDC_BUF_RX_SIZE) p->out = 0;
        }

    }while(numBytesRead < numBytesToRead);  // пока в буфере есть данные

    //pUartInfo->time_out = time_out;  //  Если это прерывание по таймауту, зафиксируем это (в данном случаи time_out будет отлична от нуля)
    xSemaphoreGiveFromISR( pUSBCDCInfo->Sem_rx, &xHigherPriorityTaskWoken );   // Если кто-то в ожидании данных, прекращаем это ожидание
	/* Enable the receive of data on EP3 */
  	SetEPRxValid(ENDP3);
}


/*******************************************************************************
* Function Name  : SOF_Callback / INTR_SOFINTR_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
#ifdef STM32F10X_CL
void INTR_SOFINTR_Callback(void)
#else
void SOF_Callback(void)
#endif /* STM32F10X_CL */
{
//  static uint32_t FrameCount = 0;
//  
//  if(bDeviceState == CONFIGURED)
//  {
//    if (FrameCount++ == VCOMPORT_IN_FRAME_INTERVAL)
//    {
//      /* Reset the frame counter */
//      FrameCount = 0;
//      
//      /* Check the data to be sent through IN pipe */
//      Handle_USBAsynchXfer();
//    }
//  }  
}
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

