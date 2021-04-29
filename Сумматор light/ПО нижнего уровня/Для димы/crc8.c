#include "crc8.h"

unsigned char crc8 (unsigned char byte, unsigned char crc)
{
	unsigned char i = 8;

	do
	{
		if ((byte ^ crc) & 0x01)
			crc = ((crc ^ 0x18) >>1) | 0x80;
			else
				crc >>= 1;
		byte >>= 1;
	} while (--i);
	return(crc);
}
