/****************************************************************************
 *   $Id:: adc.c 4785 2010-09-03 22:39:27Z nxp21346                         $
 *   Project: NXP LPC11xx ADC example
 *
 *   Description:
 *     This file contains ADC code example which include ADC 
 *     initialization, ADC interrupt handler, and APIs for ADC
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
#include "adc_drv.h"

/*****************************************************************************
** Function name:		ADCInit
**
** Descriptions:		initialize ADC channel
**
** parameters:			ADC clock rate
** Returned value:		None
** 
*****************************************************************************/
void ADCInit( uint32_t ADC_Clk )
{
  uint32_t i;

  /* Disable Power down bit to the ADC block. */  
  LPC_SYSCON->PDRUNCFG &= ~(0x1<<4);

  /* Enable AHB clock to the ADC. */
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<13);

  for ( i = 0; i < ADC_NUM; i++ )
  {
	ADCValue[i] = 0x0;
  }
  /* Unlike some other pings, for ADC test, all the pins need
  to set to analog mode. Bit 7 needs to be cleared according 
  to design team. */
  LPC_IOCON->R_PIO0_11 &= ~0x8F; /*  ADC I/O config */
  LPC_IOCON->R_PIO0_11 |= 0x02;  /* ADC IN0 */
  LPC_IOCON->R_PIO1_0  &= ~0x8F;	
  LPC_IOCON->R_PIO1_0  |= 0x02;  /* ADC IN1 */
  LPC_IOCON->R_PIO1_1  &= ~0x8F;	
  LPC_IOCON->R_PIO1_1  |= 0x02;  /* ADC IN2 */
  LPC_IOCON->R_PIO1_2 &= ~0x8F;	
  LPC_IOCON->R_PIO1_2 |= 0x02; /* ADC IN3 */
#ifdef __SWD_DISABLED
  LPC_IOCON->SWDIO_PIO1_3   &= ~0x8F;	
  LPC_IOCON->SWDIO_PIO1_3   |= 0x02;  /* ADC IN4 */
#endif
  LPC_IOCON->R_PIO0_11   = 0x02;	// Select AD0 pin function
  LPC_IOCON->R_PIO1_0    = 0x02;	// Select AD1 pin function
  LPC_IOCON->R_PIO1_1    = 0x02;	// Select AD2 pin function
  LPC_IOCON->R_PIO1_2    = 0x02;	// Select AD3 pin function
//  LPC_IOCON->ARM_SWDIO_PIO1_3    = 0x02;	// Select AD4 pin function
  LPC_IOCON->PIO1_4    = 0x01;	// Select AD5 pin function
  LPC_IOCON->PIO1_10   = 0x01;	// Select AD6 pin function
  LPC_IOCON->PIO1_11   = 0x01;	// Select AD7 pin function

  LPC_ADC->CR = ((SystemCoreClock/LPC_SYSCON->SYSAHBCLKDIV)/ADC_Clk-1)<<8;

  return;
}

/*****************************************************************************
** Function name:		ADCRead
**
** Descriptions:		Read ADC channel
**
** parameters:			Channel number
** Returned value:		Value read, if interrupt driven, return channel #
** 
*****************************************************************************/
uint32_t ADCRead( uint8_t channelNum )
{
  uint32_t regVal, ADC_Data;

  /* channel number is 0 through 7 */
  if ( channelNum >= ADC_NUM )
  {
	channelNum = 0;		/* reset channel number to 0 */
  }
  LPC_ADC->CR &= 0xFFFFFF00;
  LPC_ADC->CR |= (1 << 24) | (1 << channelNum);	
				/* switch channel,start A/D convert */

  while ( 1 )			/* wait until end of A/D convert */
  {
	regVal = *(volatile unsigned long *)(LPC_ADC_BASE 
			+ ADC_OFFSET + ADC_INDEX * channelNum);
	/* read result of A/D conversion */
	if ( regVal & ADC_DONE )
	{
	  break;
	}
  }	
        
  LPC_ADC->CR &= 0xF8FFFFFF;	/* stop ADC now */    
  if ( regVal & ADC_OVERRUN )	/* save data when it's not overrun, otherwise, return zero */
  {
	return ( 0 );
  }
  ADC_Data = ( regVal >> 6 ) & 0x3FF;
  return ( ADC_Data );	/* return A/D conversion value */
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
