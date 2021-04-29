#include <avr/pgmspace.h>
#include "settings.h"

__ATTR_EEPROM__ REGISTER_FILE EepromRegFile = {0};
REGISTER_FILE RegFile;


DOT_TABLE ProgramDotTable[ MAX_DOT_COUNT ] __attribute__ ((section (".also_var")));

#ifdef FREQUENCY_INPUT
uint16_t CRC_FLASH	__attribute__ ((section (".crccode"))) = 0xe9bc;
#else
uint16_t CRC_FLASH	__attribute__ ((section (".crccode"))) = 0xe9272;
#endif

__ATTR_EEPROM__ byte  EEP_ID0 = 99;	
__ATTR_EEPROM__ unsigned long   EEP_Boudrate0 = DEFAULT_BAUDRATE;

double get_double_P(PGM_VOID_P p)
{
	double temp;
	memcpy_P(&temp, p, sizeof(temp));
	return temp;
}
