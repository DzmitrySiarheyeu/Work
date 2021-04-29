/****************************************************************************
 *   $Id:: timer32.c 4785 2010-09-03 22:39:27Z nxp21346                     $
 *   Project: NXP LPC11xx 32-bit timer example
 *
 *   Description:
 *     This file contains 32-bit timer code example which include timer 
 *     initialization, timer interrupt handler, and related APIs for 
 *     timer setup.
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

#include "timer32.h"

/******************************************************************************
** Function name:		clear_interrupt_pending_timer32
**
** Descriptions:		Clears interrupt pending
**
** parameters:			timer number: 0 or 1
** 						channel number: 0, 1, 2, 3 - match channels; 4 - capture channel							 
** Returned value:		None
** 
******************************************************************************/

void clear_interrupt_pending_timer32(uint8_t timer_num, uint8_t ch_num)
{
	/* clear interrupt flag */
	if(timer_num)
		LPC_TMR32B1->IR = (1 << ch_num);				
	else
		LPC_TMR32B0->IR = (1 << ch_num);			
}

/******************************************************************************
** Function name:		get_interrupt_status_timer32
**
** Descriptions:		Returns an interrupt status > 0 if interrupt is active
**
** parameters:			timer number: 0 or 1
** 						channel number: 0, 1, 2, 3 - match channels; 4 - capture channel							 
** Returned value:		None
** 
******************************************************************************/

int get_interrupt_status_timer32(uint8_t timer_num, uint8_t ch_num)
{
	/* clear interrupt flag */
	if(timer_num)
		return (LPC_TMR32B1->IR & (1 << ch_num));				
	else
		return (LPC_TMR32B0->IR & (1 << ch_num));				
}

/******************************************************************************
** Function name:		enable_timer
**
** Descriptions:		Enable timer
**
** parameters:			timer number: 0 or 1
** Returned value:		None
** 
******************************************************************************/
void enable_timer32(uint8_t timer_num)
{
  if ( timer_num == 0 )
  {
    LPC_TMR32B0->TCR = 1;
  }
  else
  {
    LPC_TMR32B1->TCR = 1;
  }
}

/******************************************************************************
** Function name:		disable_timer
**
** Descriptions:		Disable timer
**
** parameters:			timer number: 0 or 1
** Returned value:		None
** 
******************************************************************************/
void disable_timer32(uint8_t timer_num)
{
  if ( timer_num == 0 )
  {
    LPC_TMR32B0->TCR = 0;
  }
  else
  {
    LPC_TMR32B1->TCR = 0;
  }
}

/******************************************************************************
** Function name:		reset_timer
**
** Descriptions:		Reset timer
**
** parameters:			timer number: 0 or 1
** Returned value:		None
** 
******************************************************************************/
void reset_timer32(uint8_t timer_num)
{
  uint32_t regVal;

  if ( timer_num == 0 )
  {
    regVal = LPC_TMR32B0->TCR;
    regVal |= 0x02;
    LPC_TMR32B0->TCR = regVal;
  }
  else
  {
    regVal = LPC_TMR32B1->TCR;
    regVal |= 0x02;
    LPC_TMR32B1->TCR = regVal;
  }
}

/******************************************************************************
** Function name:		init_timer
**
** Descriptions:		Initialize timer, set timer interval, reset timer,
**						install timer interrupt handler
**
** parameters:			timer number and timer interval
** Returned value:		None
** 
******************************************************************************/
void init_timer32(uint8_t timer_num, uint32_t TimerInterval) 
{
  if ( timer_num == 0 )
  {
    /* Some of the I/O pins need to be carefully planned if
    you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<9);

	LPC_TMR32B0->TC = 0;
	LPC_TMR32B0->PR = 0;
	LPC_TMR32B0->CTCR = 0;
    LPC_TMR32B0->MR0 = TimerInterval;
	LPC_TMR32B0->MCR = 3;			/* Interrupt and Reset on MR0 */

	/* Enable the TIMER1 Interrupt */
    NVIC_EnableIRQ(TIMER_32_0_IRQn);
  }
  else if ( timer_num == 1 )
  {
    /* Some of the I/O pins need to be clearfully planned if
    you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<10);

	LPC_TMR32B1->TC = 0;
	LPC_TMR32B1->PR = 0;
	LPC_TMR32B1->CTCR = 0;
    LPC_TMR32B1->MR0 = TimerInterval;
    LPC_TMR32B1->MCR = 3;			/* Interrupt and Reset on MR0 */

    /* Enable the TIMER1 Interrupt */
    NVIC_EnableIRQ(TIMER_32_1_IRQn);
  }
}

/******************************************************************************
**                            End Of File
******************************************************************************/
