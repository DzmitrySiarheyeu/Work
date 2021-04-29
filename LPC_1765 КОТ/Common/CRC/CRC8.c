#include <stdint.h>



uint8_t crc8 (uint8_t byte, uint8_t crc)
{
	uint8_t i = 8;

	do
	{
		if ((byte ^ crc) & 0x01)
			crc = ((crc ^ 0x18) >>1) | 0x80;
			else
				crc >>= 1;
		byte >>= 1;
	} while (--i);
	return crc;
}


uint8_t get_crc8(uint8_t *data, uint16_t len, uint8_t f_crc)
{
	int i = 0;
	uint8_t crc = f_crc;

	for(i = 0; i < len; i++)
	{
		crc = crc8(data[i], crc);
	}
	return crc;
}

