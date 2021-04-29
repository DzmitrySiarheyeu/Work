#ifndef _USART_H_
#define _USART_H_

#include <avr/io.h>
#include <avr/eeprom.h>
#include "base_type.h"
#include "iit_boot.h"

//Максимальная длина пакета
#define USART_PACKET_MAX_LENGHT	250

/*Definitions*/
#define State_INITIAL		0
#define State_IDLE			1
#define State_RECEIVING		2
#define State_SENDING		3
#define State_PROCESSING	4
#define State_WAITING		5

//Количество тиков таймера(переполнений) для 1,750мс
#define T35_Ticks (F_CPU / (TIMER0_CLOCK * 255 * 1000000 / 3750) + 1)
//Количество тиков таймера(переполнений) для 0,750мс
#define T15_Ticks (F_CPU / (TIMER0_CLOCK * 255 * 1000000 / 1750) + 1)
//300 ms
#define TimeOut_Ticks (F_CPU / (TIMER0_CLOCK * 255 * 1000000 / 300000) + 1)

#ifdef _AVR_IOMX8_H_
#define UCR0 (*(pUDR0 - 5))
#define USR0 (*(pUDR0 - 6))
#define UBR0 (*(pUDR0 - 2))
#else
#define UCR0 (*(pUDR0 - 2))
#define USR0 (*(pUDR0 - 1))
#define UBR0 (*(pUDR0 - 3))
#endif

byte UartRecivePacket(void);
void SET_DE485_0(void);
void CLR_DE485_0(void);
void InitUART(void);

extern volatile byte timer0;
extern volatile unsigned char  *pUDR0;
extern volatile byte * volatile pBuf0;
extern byte DataBuffer0[ USART_PACKET_MAX_LENGHT ];
extern volatile byte BufferSize0;
extern volatile byte state0;


#endif
