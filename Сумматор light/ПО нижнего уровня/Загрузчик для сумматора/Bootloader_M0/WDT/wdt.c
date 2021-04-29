/****************************************************************************
 *   $Id:: wdt.c 4823 2010-09-07 18:39:49Z nxp21346                         $
 *   Project: NXP LPC11xx WDT(Watchdog timer) example
 *
 *   Description:
 *     This file contains WDT code example which include WDT 
 *     initialization, WDT interrupt handler, and APIs for WDT
 *     reading.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
****************************************************************************/
#include "wdt.h"
#include "gpio.h"

#define TOGGLE_EXT_WDT(s)		(GPIOSetValue(2, 0, s > 0 ? 0 : 1))

/*****************************************************************************
** Function name:		WDTInit
**
** Descriptions:		Initialize watchdog timer, install the
**						watchdog timer interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void WDTInit( void )
{
	/* Enable clock to WDT */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<15);
	
	LPC_SYSCON->PDRUNCFG &= ~(0x1<<6);
	
	LPC_SYSCON->WDTCLKSEL = 0x01;		/* Select watchdog osc */
	LPC_SYSCON->WDTCLKUEN = 0x01;		/* Update clock */
	LPC_SYSCON->WDTCLKUEN = 0x00;		/* Toggle update register once */
	LPC_SYSCON->WDTCLKUEN = 0x01;
	while ( !(LPC_SYSCON->WDTCLKUEN & 0x01) );		/* Wait until updated */
	LPC_SYSCON->WDTCLKDIV = 1;		/* Divided by 1 */

	LPC_WDT->TC = WDT_FEED_VALUE;	/* once WDEN is set, the WDT will start after feeding */
	
	LPC_WDT->FEED = 0xAA;		/* Feeding sequence */
	LPC_WDT->FEED = 0x55;    

	LPC_WDT->MOD |= 0x03;
}

/*****************************************************************************
** Function name:		WDTFeed
**
** Descriptions:		Feed watchdog timer to prevent it from timeout
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void WDTFeed( void )
{

#ifdef DEBUG_OFF
	static uint8_t ExtWDTState = 0;

	ExtWDTState ^= 0xFF;
	TOGGLE_EXT_WDT(ExtWDTState);	

  	LPC_WDT->FEED = 0xAA;		/* Feeding sequence */
  	LPC_WDT->FEED = 0x55;
  	return;
#endif
}

/******************************************************************************
**                            End Of File
******************************************************************************/
