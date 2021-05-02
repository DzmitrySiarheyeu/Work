// Базовые типы
// Project 	: GTAA
// Company 	: VIST
// Master  	: Алексей Папковский.
// Start Date	: 09-10-2002
// Last Edition	: 14-10-2002

#ifndef __BASE_TYPE_H__
#define __BASE_TYPE_H__

//#define __AVR_MICROCONTROLLER__
//#define __x51_MICROCONTROLLER__

#ifndef true
#define true 1
#endif 

#ifndef false
#define false 0
#endif

typedef unsigned char byte;

#ifndef null
#define null 0
#endif

#ifndef __ATTR_EEPROM__
#define __ATTR_EEPROM__ __attribute__((section(".eeprom")))
#endif

#ifdef __x51_MICROCONTROLLER__

#include <INTRINS.H>

typedef bit fl_type;
typedef char xdata str_const;
typedef char code prog_char;
typedef int code prog_int;

#define inp(port_name) port_name
#define outb(a, b) a = b

#define SIG_INTERRUPT0 void __vector1(void) interrupt 1
#define SIG_UART_RECV  void __vector4_r(void) interrupt 4 using 2
#define SIG_UART_TRANS void __vector4_t(void) interrupt 4 using 2

#define SIGNAL(signame)	signame

#else

#include <avr\pgmspace.h>
typedef unsigned char fl_type;
typedef prog_char str_const;
#define PRG_RDW(x)({ unsigned int __addr16 = (unsigned int)x;	\
				unsigned int __result;							\
				asm volatile ("movw r30, %1" "\n\t"	 			\
							 "lpm %A0,z+" "\n\t"				\
							 "lpm %B0,z" "\n\t"					\
							 : "=r" (__result)					\
							 : "r" (__addr16)					\
							 : "r30", "r31");					\
							__result; })
//#define	STR_RDB(addr) PRG_RDB(addr)

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#define outb(sfr, val) (_SFR_BYTE(sfr) = (val))
#define inb(sfr) _SFR_BYTE(sfr)
#define _NOP() __asm__ __volatile__ ("nop")


#endif

#endif
