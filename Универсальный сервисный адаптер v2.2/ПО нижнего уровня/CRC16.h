#ifndef __CRC_16_H__
#define __CRC_16_H__

#include <stdint.h>


unsigned int CRC16(uint8_t * puchMsg, uint8_t usDataLen, unsigned int prevCRC);
uint16_t CRC16_P (prog_uint8_t *puchMsg, uint8_t usDataLen, unsigned int prevCRC);


#endif // __CRC_16_H__
