#pragma once

// WinAVR  20071221  
#define inline inline __attribute__ ((gnu_inline))

#ifndef __ATTR_EEPROM__
	#define __ATTR_EEPROM__ __attribute__((section(".eeprom")))
#endif

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h> 
#include <avr/wdt.h> 
#include <string.h>

#include "interface.h"

#include "pins.h"
#include "base_type.h"
#include "settings.h"

#define VCC 5.0

#define DEFAULT_BAUDRATE 19200

#ifndef sbi
	#define sbi(reg, bit) (reg |= (1<<bit))
#endif

#ifndef cbi
	#define cbi(reg, bit) (reg &= (~(1<<bit)))
#endif


void mem_reverce(void *data, unsigned char count);

double Reverse4(double in);

unsigned short Reverse2(unsigned short in);

