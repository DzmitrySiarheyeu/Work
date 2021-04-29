// Реализация подсчета 16 битной контрольной суммы
#include <stdint.h>

static inline uint16_t AddCRC_RX (unsigned char c, uint16_t prevCRC)
{
  unsigned char j;
  uint16_t lsb;
  prevCRC ^= c;
  for (j = 0; j < 8; j++)
  {
    lsb = prevCRC & 0x0001;
    prevCRC = prevCRC >> 1;
    if (lsb) prevCRC ^= 0xA001;
  }
  return prevCRC;
}

uint16_t CRC16 (uint8_t * puchMsg, uint16_t usDataLen, uint16_t prevCRC) /* The function returns the CRC as a unsigned short type */
{
	while (usDataLen--) /* pass through message buffer */
		prevCRC = AddCRC_RX(*puchMsg++, prevCRC);
	return prevCRC;
}
