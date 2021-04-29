/*----------------------------------------------------------------------------
 *      Name:    vcomdemo.c
 *      Purpose: USB virtual COM port Demo
 *      Version: V1.20
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

#include "LPC17xx.h"    
#include "lpc_types.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "cdc.h"
#include "cdcuser.h"
#include "serial.h"
#include "vcomdemo.h"

#include "driverscore.h"
#include "lpc17xx_gpio.h"

uint8_t buf[512];

DEVICE_HANDLE hUSB, hUART;

/*----------------------------------------------------------------------------
  Reads character from serial port buffer and writes to USB buffer
 *---------------------------------------------------------------------------*/
void VCOM_Serial2Usb(void) 
{
  uint16_t cnt;

  hUART = OpenDevice("UART1");
  DeviceIOCtl(hUART, RX_DATA_COUNT, (uint8_t *)&cnt);
  ReadDevice(hUART, buf, (int)cnt);

  if(cnt > 0)
  {
  	hUSB = OpenDevice("USBCDC");
	WriteDevice(hUSB, buf, (int)cnt);
	CloseDevice(hUSB);		
  }

  CloseDevice(hUART);
}

/*----------------------------------------------------------------------------
  Reads character from USB buffer and writes to serial port buffer
 *---------------------------------------------------------------------------*/
void VCOM_Usb2Serial(void) 
{
  uint16_t cnt;

  hUSB = OpenDevice("USBCDC");
  DeviceIOCtl(hUSB, RX_DATA_COUNT, (uint8_t *)&cnt);
  ReadDevice(hUSB, buf, (int)cnt);

  if(cnt > 0)
  {
  	hUART = OpenDevice("UART1");
	WriteDevice(hUART, buf, (int)cnt);
	CloseDevice(hUART);		
  }

  CloseDevice(hUSB);	
}


/*----------------------------------------------------------------------------
  checks the serial state and initiates notification
 *---------------------------------------------------------------------------*/
void VCOM_CheckSerialState (void) 
{

}

/*----------------------------------------------------------------------------
  Main Program
 *---------------------------------------------------------------------------*/
int main (void) {

  SystemInit();

  InitDriverSystem();

  GPIO_SetDir(2, (1 << 5), 1);
  GPIO_ClearValue(2, (1 << 5));

  GPIO_SetDir(1, (1 << 19), 1);
  GPIO_ClearValue(1, (1 << 19));

  while (1) 
  {                               // Loop forever
    VCOM_Serial2Usb();                      // read serial port and initiate USB event
    VCOM_CheckSerialState();
	VCOM_Usb2Serial();
  }											   
}  

