//=============================================================================
//=============================================================================

#include "config.h"

#include "bus.h"
#include "LPC11xx.h"
#include "wdt.h"


void delay(void)
{
	int i=0;
	for(;i<250000;i++);
}

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80

#define UART_LOAD_DLM(div)  			(((div) >> 8) & 0xFF)		/**< Macro for loading most significant halfs of divisors */
#define UART_LOAD_DLL(div)				((div) & 0xFF)

void busInit(void)
{
	LPC_IOCON->PIO1_6 &= ~0x07;    /*  UART I/O config */
	LPC_IOCON->PIO1_6 |= 0x01;     /* UART RXD */
	LPC_IOCON->PIO1_7 &= ~0x07;	
	LPC_IOCON->PIO1_7 |= 0x01;     /* UART TXD */



	/* Enable UART clock */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12);
	LPC_SYSCON->UARTCLKDIV = 0x1;     /* divided by 1 */

	LPC_UART->LCR = 0x83;

	delay();
	LPC_UART->DLM = UART_LOAD_DLM(0x7D);
	LPC_UART->DLL = UART_LOAD_DLL(0x7D);
	LPC_UART->LCR = 0x03;
	LPC_UART->FDR = 0x41;
	delay();
 

	LPC_UART->FCR = 0x07;			/* Enable and reset TX and RX FIFO. */
	LPC_UART->FCR |= 0x01 << 6;		/* Set FIFO trigger level. */

	/* Ensure a clean start, no data in either TX or RX FIFO. */
	while (( LPC_UART->LSR & (LSR_THRE|LSR_TEMT)) != (LSR_THRE|LSR_TEMT) );
	while ( LPC_UART->LSR & LSR_RDR )
	{
		uint32_t regVal = LPC_UART->RBR;	/* Dump data from RX FIFO */
	}
}


//=============================================================================
// Prepare bus (in this case, wait for the first byte to arrive).

int busPrepare(void)
{
	uint16_t init_code;

	*((unsigned char *)&init_code + 1) = busReceiveByte();
	*((unsigned char *)&init_code) = busReceiveByte();

//	delay();

	if(init_code == 0xAACE)
	{
		busReplyByte(ERROR_OK);
		return 0;
	}
	else if(init_code == 0xAA55)
	{
		busReplyByte(ERROR_OK);
		busReplyByte(PO_VERSION);
		return 1;
	}

	busReplyByte(ERROR_CRC);
	return 1;
}


//=============================================================================
// Reply byte

void busReplyByte(unsigned char data)
{
	LPC_UART->THR = (data & 0x000000FF);
}

//=============================================================================
// Receive byte

unsigned char busReceiveByte(void)
{
	while((LPC_UART->LSR & 0x01) == 0)
	{
		WDTFeed();
	}
	return (LPC_UART->RBR & 0x000000FF);	
}


//=============================================================================
// Set bus to a busy state. Requires no action in USART.

void busBusy(void)
{
	return;
}
