/******************************************************************************/
/* RETARGET.C: 'Retarget' layer for target-dependent low level functions      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2010 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include "Config.h"

#include <stdio.h>
#include <rt_misc.h>

#include "LPC17XX.h"
#include "core_cm3.h"

#pragma import(__use_no_semihosting_swi)

volatile int ITM_RxBuffer = ITM_RXBUFFER_EMPTY;

struct __FILE { int handle; /* Add whatever you need here */ };
FILE std_dbg_out;

// поток вывода для дебага
#ifdef DEBUG
FILE std_dbg_out;
#endif

// финкция вывода для дисплея 
#if (_DISPLAY_ == DEVICE_ON)
extern int LCD_putchar(uint8_t ch);
#endif


/*----------------------------------------------------------------------------
  Receive a character via Debugger (blocking)     (used from Retarget.c)
 *----------------------------------------------------------------------------*/
int getkey(void){
  while (ITM_CheckChar() != 1);
  return (ITM_ReceiveChar());
}


int fputc(int ch, FILE *f) {

#ifdef DEBUG
	if(f == &std_dbg_out) 
		return ITM_SendChar(ch);
#endif

#if (_DISPLAY_ == DEVICE_ON)
	// для дисплея используется поток по умолчанию
	return LCD_putchar((uint8_t)ch);
#else
	return 0;
#endif
}


int fgetc(FILE *f) {
  return (getkey());
}


int ferror(FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}


void _ttywrch(int ch) {
  ITM_SendChar(ch);
}


void _sys_exit(int return_code) {
label:  goto label;  /* endless loop */
}
