/****************************************************************************
 *   $Id:: adc.h 4785 2010-09-03 22:39:27Z nxp21346                         $
 *   Project: NXP LPC11xx ADC example
 *
 *   Description:
 *     This file contains ADC code header definition.
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
#ifndef __ADC_H
#define __ADC_H

#define ADC_OFFSET		0x10
#define ADC_INDEX		4

#define ADC_DONE		0x80000000
#define ADC_OVERRUN		0x40000000
#define ADC_ADINT		0x00010000

#define ADC_NUM			8			/* for LPC11xx */
#define ADC_CLK			2400000		/* set to 2.4Mhz */

extern void ADCInit( uint32_t ADC_Clk );

#endif /* end __ADC_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
