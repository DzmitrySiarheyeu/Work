//=============================================================================
// Copyright (C) 2003 Atmel Corporation
//
// File:			loader.c
// Compiler:		IAR Atmel AVR C/EC++ Compiler
// Output Size:
// Based on work by:ШE, VU
// Created:			4-Feb-2003	JP (Atmel Finland)
// Modified:
//
// Support Mail:	avr@atmel.com
//
// Description:		Loader receives encrypted data frames (generated using
//					'create' tool and sent by 'update' tool), decrypts them and
//					executes the commands described in frames.
//
//					For more information, refer to the Application Note
//					Documentation.
//
// Other Info:		
//=============================================================================


#include <avr/io.h>
#include <avr/boot.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "util.h"
#include "des.h"
#include "loader.h"
#include "bootldr.h"
#include "mbfanction0.h"


//=============================================================================
// Error Codes

#define ERROR_OK 0x11
#define ERROR_CRC 0x22


//=============================================================================
// Frame types

#define TYPE_EOF 0
#define TYPE_ERASE 1
#define TYPE_PREPARE 2
#define TYPE_DATA 3
#define TYPE_PROGRAM 4
#define TYPE_EEPROM 5
#define TYPE_LOCKBITS 6
#define TYPE_RESET 7

__attribute__ ((section (".bootloader"))) __attribute__ ((__noinline__)) unsigned int CRC(unsigned char byte, unsigned int oldCRC)
{
	uint16_t __pattern;
	uint16_t __ret;
	uint8_t __i;

   	asm volatile(
			"ldi		%1, 0x08" "\n\t"
			"ldi		%A2, 0x05" "\n\t"
			"ldi		%B2, 0x80" "\n\t"

		"CRC_Loop%=:"
			//Rotate left. If MSb is 1 -> divide with Generator-polynome.
			"lsl		%3" "\n\t"
			"rol		%A4" "\n\t"
			"rol		%B4" "\n\t"
			"brcc	CRC_SkipEor%=" "\n\t"

			"eor		%A4, %A2" "\n\t"
			"eor		%B4, %B2" "\n\t"

		"CRC_SkipEor%=:"   
			"dec		%1" "\n\t"
			"brne	CRC_Loop%=" "\n\t"
			:"=d" (__ret), "=&d" (__i), "=&d" (__pattern)
			: "r" (byte), "0" (oldCRC));
			return __ret;
}

//=============================================================================
// Prepare bus (in this case, wait for the first byte to arrive).

__attribute__ ((section (".bootloader")))  static inline void busPrepare(void)
{
	while (!(UCSR0A & (1 << RXC0))) wdt_reset();;
}


//=============================================================================
// Receive byte

__attribute__ ((section (".bootloader")))  static inline unsigned char busReceiveByte(void)
{
	busPrepare();
	
	return UDR0;
}


//=============================================================================
// Set bus to a busy state. Requires no action in USART.

__attribute__ ((section (".bootloader")))  static inline void busBusy(void)
{
	return;
}


//=============================================================================
// Reply byte

__attribute__ ((section (".bootloader")))  static inline void busReplyByte(unsigned char data)
{
	UCSR0B &= ~(1 << RXEN0);
//	PORT_DE |= 1 << PIN_DE;
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
	while(!(UCSR0A & (1 << TXC0)));
//	PORT_DE &= ~(1 << PIN_DE);
	UCSR0B |= 1 << RXEN0;
	UCSR0A |= (1 << TXC0);
}

//=============================================================================
// Initialize bus interface

__attribute__ ((section (".bootloader"))) static inline void busInit(void)
{
	UBRR0 = (uint16_t)(F_CPU/16/9600 - 1);	
		
	// настройка порта 
//	PORT_DE &= ~(1 << PIN_DE);
//	DDR_DE |= (1 << PIN_DE);

	// Enable Rx and Tx.
	UCSR0B = (1 << RXEN0) + (1 << TXEN0);
}

/*
__attribute__ ((section (".bootloader"))) static inline void eeprom_write_byte_boot(uint8_t addr, uint8_t data)
{
	asm volatile (

	"spmEEWriteByte%=:"
		"rcall	spmEEWriteByteComplete%=" "\n\t"

		"out		%2, %0" "\n\t"				// EEARL
		"out		%3, __zero_reg__" "\n\t"	// EEARH
		"out		%4, %1" "\n\t"				// EEDR

		"out		%5, %6" "\n\t"				// EEMWE
		"sbi		%5, %7" "\n\t"				// EEWE
		
	"spmEEWriteByteComplete%=:"
		"sbic	%5, %7" "\n\t"					// EEWE
		"rjmp	spmEEWriteByteComplete%=" "\n\t"
		"ret"
		: : "r" (addr), "r" (data), "I" (_SFR_IO_ADDR(EEARL)), "I" (_SFR_IO_ADDR(EEARH)), "I" (_SFR_IO_ADDR(EEDR)), "I" (_SFR_IO_ADDR(EECR)), "I" (1 << EEMPE), "I" (EEPE) );
}
*/

__attribute__ ((section (".bootloader"))) void boot_program_page_core (uint32_t page, uint8_t *buf)	
{
      	uint8_t i;
		uint16_t *p = (uint16_t *)buf;
        	
        boot_page_erase (page);
        boot_spm_busy_wait ();      // Wait until the memory is erased.

        for (i=0; i<SPM_PAGESIZE; i+=2)
        {
		    boot_page_fill (page + i, *p++);
		}
        boot_page_write (page);     // Store buffer in flash page.
        boot_spm_busy_wait();       // Wait until the memory is written.

        // Reenable RWW-section again. We need this if we want to jump back
        // to the application after bootloading.

        boot_rww_enable ();
}

//=============================================================================
// Loader routine. For more information about the format of frames, please
// refer to the Application Note Documentation.

BOOTLOADER_SECTION __attribute__ ((__noreturn__)) void loader(void)
{
	unsigned char rxBuffer[BUFFER_SIZE];	// Receive buffer
	unsigned char pageBuffer[PAGE_SIZE];	// Page is assembled here before

	cli();
	busInit();

	//=============================================================================
	// Initial vector for cipher block unchaining (INITIALVECTOR_HI and LO are
	// defined in 'bootldr.h' which is generated by 'create' tool)

												// getting programmed to flash mem
	#if KEY_COUNT > 0
		unsigned long chainCipherBlock[2];		// Buffer for Cipher Block
												// Unchaining
		chainCipherBlock[0] = INITIALVECTOR_HI;
		chainCipherBlock[1] = INITIALVECTOR_LO;
	#endif

	wdt_enable(WDTO_1S);
	// Prepare bus for incoming frames...
	busPrepare();

	// Loop forever (the loop is broken by Watchdog when RESET record is
	// received)
	for (;;)
	{
		unsigned char frameSize;
		unsigned int crc;
		
		// Get the frame size
		frameSize =	((unsigned char)busReceiveByte() << 8) |	busReceiveByte();

		// Receive a frame of data from communication interface and calculate
		// its CRC
		{
			unsigned char *p = rxBuffer;
			unsigned char n = frameSize;

			crc = 0;
			do
			{
				unsigned char ch;

				ch = busReceiveByte();

				*p++ = ch;
				crc = CRC(ch, crc);
				
				wdt_reset();
			}
			while (--n);
			
			busBusy();
		}

		// CRC is OK?
		if (crc == 0x0000)
		{
			unsigned char *p = rxBuffer;			

			// Decrypt 8 bytes, CRC-bytes are ignored.
			#if KEY_COUNT > 0
			{
				unsigned char *p = rxBuffer;

				frameSize -= 2;
				do
				{
			    	//wdt_reset();
					desDecrypt(p, (uint8_t *)chainCipherBlock);
					p += 8;
				}
				while (frameSize -= 8);
			}
			#endif // KEY_COUNT > 0

			// Check that the signature is correct
			uint32_t *tt = (uint32_t *)p;
			if(*tt++ == SIGNATURE)
			{
				uint16_t address = 0;
				unsigned char size;
				unsigned char type;
				p = (unsigned char *)tt;
				// Continue parsing the frames until the 'End Of Frame' is
				// received
				while ((type = *p++) != TYPE_EOF)
				{
			    	wdt_reset();

					// Receive Lock Bits (used in TYPE_LOCKBITS)
					//bits = *p++;
					p++; // биты у нас не поддерживаются

					address = ((unsigned int)*p++ << 8);
					address = ((unsigned int)address + *p++);

					p++; // size = ((unsigned int)*p++ << 8); // наш размер всегда меньше байта
					size = *p++;

					switch (type)
					{
						// Erase page
						case TYPE_ERASE:
						if((unsigned int)address < MEM_SIZE)
							boot_page_erase (address);
							boot_spm_busy_wait();       // Wait until the memory is written.
							// Fall-through!

						// Prepare for incoming data chunks by copying the page
						// contents into the page buffer
						case TYPE_PREPARE:
						{
							unsigned char *q = pageBuffer;
							uint16_t r = (uint16_t)address;

							do
							{
								*q++ = pgm_read_byte_near(r++);;
							}
							while (--size);
							break;
						}

						// Chunk of data is written into the page buffer
						case TYPE_DATA:
						{
							unsigned char *r =
								&pageBuffer[address];

							do
							{
								*r++ = *p++;
							}
							while (--size);
							break;
						}

						// Program page buffer into flash page
						case TYPE_PROGRAM:
						{
							if((unsigned int)address < MEM_SIZE)
								boot_program_page_core((unsigned int)address, pageBuffer);
							break;
						}

						// Write a chunk of data into EEPROM
					/*	case TYPE_EEPROM:
							do
							{
						    	wdt_reset();
								eeprom_write_byte_boot(address++, *p++);
							}
							while (--size);
							break;*/

						// Reset (let the Watchdog Timer expire)!
						case TYPE_RESET:
							busReplyByte(ERROR_OK);
							for(;;);
							
						// Nonsense frames, one byte long
						default:
							p -= 5;
							break;
					}
				}
			}

			busReplyByte(ERROR_OK);
		}
    	else
    		busReplyByte(ERROR_CRC);   		
  	}
}


__attribute__ ((section (".bootloader"))) __attribute__ ((__naked__)) void init_boot(void)
{
	asm volatile("clr __zero_reg__");
#ifdef CRC_CHECK
		// Check that the Application Section contents is undamaged
		// by calculating the CRC of the whole memory.
		{
			unsigned int crc = 0;
			uint16_t p = 0x000000;
			unsigned int n = MEM_SIZE;
	
			do
			{
				crc = CRC(pgm_read_byte_near(p++), crc);
				wdt_reset();
			}
			while (--n);
			// Application Section damaged
			//   -> do not jump to Reset Vector of the Application Section
			if (crc)
				loader();
		}
#endif
	asm volatile ("jmp 0x0000");
}

__attribute__ ((section (".initboot"))) __attribute__ ((__naked__)) void start_boot(void)   
{
	asm volatile ("rjmp init_boot");
	asm volatile ("rjmp boot_program_page_core");
	asm volatile ("rjmp loader");
}



