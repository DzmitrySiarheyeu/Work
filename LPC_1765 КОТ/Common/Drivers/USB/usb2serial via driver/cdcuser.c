/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    cdcuser.c
 *      Purpose: USB Communication Device Class User module 
 *      Version: V1.10
 *----------------------------------------------------------------------------
*      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on NXP Semiconductors LPC microcontroller devices only. Nothing else 
 *      gives you the right to use this software.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/
#include "config.h"

#if(_USBCDC_)

#include "lpc_types.h"

#include "usb.h"
#include "usbhw.h"
#include "usbcfg.h"
#include "usbcore.h"
#include "cdc.h"
#include "cdcuser.h"

#include "usbcdc_lpc23xx.h"

unsigned char NotificationBuf [10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t cdc_in_out_buf[USB_CDC_BUFSIZE];

void CDC_BulkOut(void)
{
	int numBytesToRead, numBytesRead = 0;

  	// get data from USB into intermediate buffer
  	numBytesToRead = USB_ReadEP(CDC_DEP_OUT, &cdc_in_out_buf[0]);

	USBCDC_INFO_LPC17XX *pUSBCDCInfo = (USBCDC_INFO_LPC17XX *)usbcdc_device.pDriverData;
    USBCDC_BUF *p = &(pUSBCDCInfo->rx_buf);      //  ����� ������

	static signed portBASE_TYPE xHigherPriorityTaskWoken;

    do
    {
		p->buf[p->in++] = cdc_in_out_buf[numBytesRead++];

        if(p->in >= USBCDC_BUF_SIZE) p->in = 0;
        if(p->in == p->out)  // ���� ����� �� ����� ������, ����� ������������� ������
        {
            p->out++;                          // ����������� ��������� ����������� ���� (��������� ����� �������������� � ��� ���������)
            pUSBCDCInfo->overflow = 1;           //  �������� ��� ���� ������������
            if(p->out >= USBCDC_BUF_SIZE) p->out = 0;
        }

    }while(numBytesRead < numBytesToRead);  // ���� � ������ ���� ������

    //pUartInfo->time_out = time_out;  //  ���� ��� ���������� �� ��������, ����������� ��� (� ������ ������ time_out ����� ������� �� ����)
    xSemaphoreGiveFromISR( pUSBCDCInfo->Sem_rx, &xHigherPriorityTaskWoken );   // ���� ���-�� � �������� ������, ���������� ��� ��������

}

void StartTransmitUSB(USBCDC_INFO_LPC17XX *pUSBCDCInfo)
{
    int i = 0;
    USBCDC_BUF *p = &(pUSBCDCInfo->tx_buf);      //  ����� ������
	if(pUSBCDCInfo->busy ==1)
		return;
    while(i < USB_CDC_BUFSIZE - 1) // ���� ���������� ����� ���� ��� �� ���������� ������
    {
        if(p->out == p->in)// && pUSBCDCInfo->tx_empty == 1)
        {
            break; // �������, ������ �����������
        }
		pUSBCDCInfo->busy = 1;
        cdc_in_out_buf[i++] = p->buf[p->out++];

        if(p->out == USBCDC_BUF_SIZE)  p->out = 0;
    }
	if(i == 0)
		pUSBCDCInfo->busy = 0;
	else	
		USB_WriteEP (CDC_DEP_IN, &cdc_in_out_buf[0], i);
}

void CDC_BulkIn(void)
{
    static signed portBASE_TYPE xHigherPriorityTaskWoken;

    uint16_t free_space = 0;
    USBCDC_INFO_LPC17XX *pUSBCDCInfo = (USBCDC_INFO_LPC17XX *)usbcdc_device.pDriverData;
    USBCDC_BUF *p = &(pUSBCDCInfo->tx_buf);      //  ����� ������
	pUSBCDCInfo->busy = 0;
	StartTransmitUSB(pUSBCDCInfo);
    if(p->in >= p->out) free_space = UART_BUF_SIZE - (p->in - p->out);   //  �������� ������� ���������� ����� � ������
    else free_space = p->out - p->in;

    if(free_space > (USBCDC_BUF_SIZE/2)) 
		xSemaphoreGiveFromISR( pUSBCDCInfo->Sem_tx, &xHigherPriorityTaskWoken );   // ���� ���-�� � �������� ���������� ����� � ������, ���������� ��� ��������    
}

/*----------------------------------------------------------------------------
  CDC SendEncapsulatedCommand Request Callback
  Called automatically on CDC SEND_ENCAPSULATED_COMMAND Request
  Parameters:   None                          (global SetupPacket and EP0Buf)
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_SendEncapsulatedCommand (void) {

  return (TRUE);
}

/*----------------------------------------------------------------------------
  CDC GetEncapsulatedResponse Request Callback
  Called automatically on CDC Get_ENCAPSULATED_RESPONSE Request
  Parameters:   None                          (global SetupPacket and EP0Buf)
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_GetEncapsulatedResponse (void) {

  /* ... add code to handle request */
  return (TRUE);
}


/*----------------------------------------------------------------------------
  CDC SetCommFeature Request Callback
  Called automatically on CDC Set_COMM_FATURE Request
  Parameters:   FeatureSelector
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_SetCommFeature (unsigned short wFeatureSelector) {

  /* ... add code to handle request */
  return (TRUE);
}


/*----------------------------------------------------------------------------
  CDC GetCommFeature Request Callback
  Called automatically on CDC Get_COMM_FATURE Request
  Parameters:   FeatureSelector
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_GetCommFeature (unsigned short wFeatureSelector) {

  /* ... add code to handle request */
  return (TRUE);
}


/*----------------------------------------------------------------------------
  CDC ClearCommFeature Request Callback
  Called automatically on CDC CLEAR_COMM_FATURE Request
  Parameters:   FeatureSelector
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_ClearCommFeature (unsigned short wFeatureSelector) {

  /* ... add code to handle request */
  return (TRUE);
}


/*----------------------------------------------------------------------------
  CDC SetLineCoding Request Callback
  Called automatically on CDC SET_LINE_CODING Request
  Parameters:   none                    (global SetupPacket and EP0Buf)
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_SetLineCoding (void) {
  
  return (TRUE);
}


/*----------------------------------------------------------------------------
  CDC GetLineCoding Request Callback
  Called automatically on CDC GET_LINE_CODING Request
  Parameters:   None                         (global SetupPacket and EP0Buf)
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_GetLineCoding (void) {

  return (TRUE);
}


/*----------------------------------------------------------------------------
  CDC SetControlLineState Request Callback
  Called automatically on CDC SET_CONTROL_LINE_STATE Request
  Parameters:   ControlSignalBitmap 
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_SetControlLineState (unsigned short wControlSignalBitmap) {

  /* ... add code to handle request */
  return (TRUE);
}

extern DEVICE_HANDLE hUART;
/*----------------------------------------------------------------------------
  CDC SendBreak Request Callback
  Called automatically on CDC Set_COMM_FATURE Request
  Parameters:   0xFFFF  start of Break 
                0x0000  stop  of Break
                0x####  Duration of Break
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_SendBreak (unsigned short wDurationOfBreak) {

  uint8_t state;
  if(wDurationOfBreak == 0xffff)
  {
  	GPIO_SetValue(1, 1<<19);
	state = 5;
  }
  else
  {
  	GPIO_ClearValue(1, 1<<19);
	state = 6;
  }
  DeviceIOCtl(hUART, 4, &state);

  return (TRUE);
}

/*----------------------------------------------------------------------------
  Get the SERIAL_STATE as defined in usbcdc11.pdf, 6.3.5, Table 69.
  Parameters:   none
  Return Value: SerialState as defined in usbcdc11.pdf
 *---------------------------------------------------------------------------*/
unsigned short CDC_GetSerialState (void) {

  return (0);
}


/*----------------------------------------------------------------------------
  Send the SERIAL_STATE notification as defined in usbcdc11.pdf, 6.3.5.
 *---------------------------------------------------------------------------*/
void CDC_NotificationIn (void) {

  USB_WriteEP (CDC_CEP_IN, &NotificationBuf[0], 10);   // send notification
}


#endif // _USBCDC_
