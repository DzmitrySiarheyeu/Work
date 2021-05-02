#include <string.h>
#include <avr/interrupt.h>
#include <avr/boot.h>
#include "util.h"
#include "crc16.h"

static uint8_t write_buf[SPM_PAGESIZE];

static void boot_program_page(uint32_t page, uint8_t *buf)
{
	uint8_t sreg;
	// Disable interrupts.

    sreg = SREG;
    cli();
	eeprom_busy_wait ();

	((void (*)(uint32_t, uint8_t *))0x1c01)(page, buf);

    SREG = sreg;
}

void GetMemBuf(uint8_t *dst, PGM_VOID_P src, int len)
{
	memcpy_P(dst, src, len);	
}

void SetMemBuf(PGM_VOID_P dst, uint8_t *src, int len)
{
	// пишем не более SPM_PAGESIZE за раз
	if(len > SPM_PAGESIZE)
		return;
	
	// Вычисляем адрес начала страницы содержащий наш адрес
	uint16_t page_addr = ((unsigned int)dst / SPM_PAGESIZE) * SPM_PAGESIZE;

	// Копируем содержимое страницы в буфер
	memcpy_P(write_buf, (PGM_VOID_P)page_addr, SPM_PAGESIZE);
	// вычисляем смещение относительно начала страницы
	dst = (PGM_VOID_P)(dst - (PGM_VOID_P)page_addr);

	memcpy(write_buf + (unsigned int)dst, src, len);

	boot_program_page(page_addr, write_buf);	
}


