/*****************************************************************************
 *   uart.c:  UART API file for NXP LPC11xx Family Microprocessors
 *
 *   Copyright(C) 2008, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.12.07  ver 1.00    Preliminary version, first Release
 *
******************************************************************************/
#include <stdint.h>
#include <string.h>

#include "lpc11xx.h"
#include "uart.h"

static UartCotrolStruct UartControl = {.UARTStatus = Recv,
									   .UARTRxCount = 0,
									   .UARTTxCount = 0 };

uint8_t *UartBuff = UartControl.UARTBuffer;

uint8_t tx_end_fl = 0;

/*****************************************************************************
** Function name:		UART_IRQHandler
**
** Descriptions:		UART interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void UART_IRQHandler(void)
{
	uint8_t IIRValue, LSRValue;
	uint8_t Dummy = Dummy;
	
	IIRValue = LPC_UART->IIR;
	
	IIRValue >>= 1;			/* skip pending bit in IIR */
	IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
	if (IIRValue == IIR_RLS)		/* Receive Line Status */
	{
		LSRValue = LPC_UART->LSR;
		/* Receive Line Status */
		if (LSRValue & (LSR_OE | LSR_PE | LSR_FE | LSR_RXFE | LSR_BI))
		{
			/* There are errors or break interrupt */
			/* Read LSR will clear the interrupt */
			Dummy = LPC_UART->RBR;	/* Dummy read on RX to clear 
									interrupt, then bail out */
			return;
		}
	}
	if (IIRValue == IIR_RDA)	/* Receive Data Available */
	{
		/* Receive Data Available */
		while(LPC_UART->LSR & 0x1)
		{
			UartControl.UARTBuffer[UartControl.UARTRxCount++] = LPC_UART->RBR;
			if(UartControl.UARTRxCount == UART_BUFSIZE)
				UartControl.UARTRxCount = 0;
		}
	}
	if (IIRValue == IIR_CTI)	/* Character timeout indicator */
	{
		while(LPC_UART->LSR & 0x1)
		{
			UartControl.UARTBuffer[UartControl.UARTRxCount++] = LPC_UART->RBR;
			if(UartControl.UARTRxCount == UART_BUFSIZE)
				UartControl.UARTRxCount = 0;
		}

		UartControl.UARTStatus = Timeout;
	}
	if (IIRValue == IIR_THRE)	/* THRE, transmit holding register empty */
	{
		if(UartControl.UARTTxCount < UartControl.UARTTxLength)			
			LPC_UART->THR = UartControl.UARTBuffer[UartControl.UARTTxCount++];
		else tx_end_fl = 1;	
	}
	return;
}

/*****************************************************************************
** Function name:		UARTInit
**
** Descriptions:		Initialize UART0 port, setup pin select,
**				clock, parity, stop bits, FIFO, etc.
**
** parameters:			UART baudrate
** Returned value:		None
** 
*****************************************************************************/


#define UART_ACCEPTED_BAUDRATE_ERROR	(3)							/*!< Acceptable UART baudrate error */
#define UART_LCR_DLAB_EN				((uint8_t)(1<<7))    		/*!< UART Divisor Latches Access bit enable */
#define UART_LOAD_DLM(div)  			(((div) >> 8) & 0xFF)		/**< Macro for loading most significant halfs of divisors */
#define UART_LOAD_DLL(div)				((div) & 0xFF)				/**< Macro for loading least significant halfs of divisors */
#define UART_FDR_DIVADDVAL(n)			((uint32_t)(n&0x0F))		/**< Baud-rate generation pre-scaler divisor */
#define UART_FDR_MULVAL(n)				((uint32_t)((n<<4)&0xF0))	/**< Baud-rate pre-scaler multiplier value */

void uart_set_divisors(LPC_UART_TypeDef *UARTx, uint32_t baudrate)
{
	uint32_t uClk = SystemCoreClock / 16;
	uint32_t calcBaudrate = 0;
	uint32_t temp = 0;

	uint32_t mulFracDiv, dividerAddFracDiv;
	uint32_t diviser = 0 ;
	uint32_t mulFracDivOptimal = 1;
	uint32_t dividerAddOptimal = 0;
	uint32_t diviserOptimal = 0;

	uint32_t relativeError = 0;
	uint32_t relativeOptimalError = 100000;

	/* In the Uart IP block, baud rate is calculated using FDR and DLL-DLM registers
	* The formula is :
	* BaudRate= uClk * (mulFracDiv/(mulFracDiv+dividerAddFracDiv) / (16 * (DLL)
	* It involves floating point calculations. That's the reason the formulae are adjusted with
	* Multiply and divide method.*/
	/* The value of mulFracDiv and dividerAddFracDiv should comply to the following expressions:
	* 0 < mulFracDiv <= 15, 0 <= dividerAddFracDiv <= 15 */
	for (mulFracDiv = 1 ; mulFracDiv <= 15 ;mulFracDiv++)
	{
	for (dividerAddFracDiv = 0 ; dividerAddFracDiv <= 15 ;dividerAddFracDiv++)
	{
	  temp = (mulFracDiv * uClk) / ((mulFracDiv + dividerAddFracDiv));

	  diviser = temp / baudrate;
	  if ((temp % baudrate) > (baudrate / 2))
		diviser++;

	  if (diviser > 2 && diviser < 65536)
	  {
		calcBaudrate = temp / diviser;

		if (calcBaudrate <= baudrate)
		  relativeError = baudrate - calcBaudrate;
		else
		  relativeError = calcBaudrate - baudrate;

		if ((relativeError < relativeOptimalError))
		{
		  mulFracDivOptimal = mulFracDiv ;
		  dividerAddOptimal = dividerAddFracDiv;
		  diviserOptimal = diviser;
		  relativeOptimalError = relativeError;
		  if (relativeError == 0)
			break;
		}
	  } /* End of if */
	} /* end of inner for loop */
	if (relativeError == 0)
	  break;
	} /* end of outer for loop  */

	if (relativeOptimalError < ((baudrate * UART_ACCEPTED_BAUDRATE_ERROR)/100))
	{
		UARTx->LCR |= UART_LCR_DLAB_EN;
		UARTx->DLM = UART_LOAD_DLM(diviserOptimal);
		UARTx->DLL = UART_LOAD_DLL(diviserOptimal);
		/* Then reset DLAB bit */
		UARTx->LCR &= (~UART_LCR_DLAB_EN) & 0xff;
		UARTx->FDR = (UART_FDR_MULVAL(mulFracDivOptimal) \
				| UART_FDR_DIVADDVAL(dividerAddOptimal)) & 0xff;
	}
}


void UARTInit(uint32_t baudrate)
{
	NVIC_DisableIRQ(UART_IRQn);
	
	LPC_IOCON->PIO1_6 &= ~0x07;    /*  UART I/O config */
	LPC_IOCON->PIO1_6 |= 0x01;     /* UART RXD */
	LPC_IOCON->PIO1_7 &= ~0x07;	
	LPC_IOCON->PIO1_7 |= 0x01;     /* UART TXD */

#if defined (USE_RS485_CONTROL)

	LPC_IOCON->PIO1_5 &= ~(0x07 | (0x03 << 3));   
	LPC_IOCON->PIO1_5 |= 0x01 | (0x02 << 3);     /* UART RTS */

#endif

	/* Enable UART clock */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12);
	LPC_SYSCON->UARTCLKDIV = 0x1;     /* divided by 1 */

	LPC_UART->LCR = 0x83;             /* 8 bits, no Parity, 1 Stop bit */

	uart_set_divisors(LPC_UART, baudrate);

	LPC_UART->FCR = 0x07;			/* Enable and reset TX and RX FIFO. */
	LPC_UART->FCR |= 0x01 << 6;		/* Set FIFO trigger level. */

	/* Ensure a clean start, no data in either TX or RX FIFO. */
	while (( LPC_UART->LSR & (LSR_THRE|LSR_TEMT)) != (LSR_THRE|LSR_TEMT) );
	while ( LPC_UART->LSR & LSR_RDR )
	{
		uint32_t regVal = LPC_UART->RBR;	/* Dump data from RX FIFO */
	}

#if defined (USE_RS485_CONTROL)
	
	LPC_UART->RS485CTRL = RS485_DCTRL | RS485_OINV;
	LPC_UART->RS485DLY = 1;
			
#endif
	
	LPC_UART->IER = IER_RBR | IER_THRE;	/* Enable UART interrupt */
	
	/* Enable the UART Interrupt */
	NVIC_EnableIRQ(UART_IRQn);
}

/*****************************************************************************
** Function name:		UARTSend
**
** Descriptions:		Send a block of data to the UART 0 port based
**				on the data length
**
** parameters:		buffer pointer, and data length
** Returned value:	None
** 
*****************************************************************************/
void UARTSend(uint32_t Length)
{
	UartControl.UARTTxLength = Length;
	UartControl.UARTTxCount = 0;
	while((LPC_UART->LSR & LSR_TEMT) == 0);				   //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	tx_end_fl = 0;
	LPC_UART->THR = UartControl.UARTBuffer[UartControl.UARTTxCount++];
	while(tx_end_fl == 0);
}

int UARTCheckPacket(void)
{
	if(UartControl.UARTStatus == Timeout)
		return UartControl.UARTRxCount;

	return 0;
}

void UARTResetRxBuf(void)
{
	UartControl.UARTStatus = Recv;
	UartControl.UARTRxCount = 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
