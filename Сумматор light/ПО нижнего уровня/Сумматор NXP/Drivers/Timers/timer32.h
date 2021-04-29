/****************************************************************************
 *   $Id:: timer32.h 4785 2010-09-03 22:39:27Z nxp21346                     $
 *   Project: NXP LPC11xx software example
 *
 *   Description:
 *     This file contains definition and prototype for 32-bit timer 
 *     configuration.
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
#ifndef __TIMER32_H 
#define __TIMER32_H

#include <stdint.h>
#include "lpc11xx.h"

void clear_interrupt_pending_timer32(uint8_t timer_num, uint8_t ch_num);
int get_interrupt_status_timer32(uint8_t timer_num, uint8_t ch_num);
void enable_timer32(uint8_t timer_num);
void disable_timer32(uint8_t timer_num);
void reset_timer32(uint8_t timer_num);
void init_timer32(uint8_t timer_num, uint32_t timerInterval);

#endif /* end __TIMER32_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
