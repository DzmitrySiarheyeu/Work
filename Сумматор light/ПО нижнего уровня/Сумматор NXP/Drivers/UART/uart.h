/*****************************************************************************
 *   uart.h:  Header file for NXP LPC1xxx Family Microprocessors
 *
 *   Copyright(C) 2008, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.12.07  ver 1.00    Preliminary version, first Release
 *
******************************************************************************/
#ifndef __UART_H 
#define __UART_H

#include <stdint.h>

#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80

/* RS485 mode definition. */
#define RS485_NMMEN		(0x1<<0)
#define RS485_RXDIS		(0x1<<1)
#define RS485_AADEN		(0x1<<2)
#define RS485_SEL		(0x1<<3)
#define RS485_DCTRL		(0x1<<4)
#define RS485_OINV		(0x1<<5)

#define UART_BUFSIZE	(250)

typedef enum {Recv = 0, Error, Timeout} UartStatus;

typedef struct
{
	// ??????? ???????? ????
	volatile uint32_t UARTRxCount;
	// ?????
	uint8_t  UARTBuffer[UART_BUFSIZE];

    // ?????? ?????? ??? ????????
	volatile uint32_t UARTTxLength;
	// ??????? ???????????? ????
	volatile uint32_t UARTTxCount;

	// ??????
	volatile UartStatus  UARTStatus;
} __attribute__ ((__packed__)) UartCotrolStruct;

void UARTInit(uint32_t Baudrate);
void UARTSend(uint32_t Length);
int UARTCheckPacket(void);
void UARTResetRxBuf(void);

extern uint8_t *UartBuff;

#endif /* end __UART_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
