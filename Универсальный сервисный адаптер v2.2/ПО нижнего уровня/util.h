#ifndef _UTIL_H_ 
#define _UTIL_H_ 

#include <stdint.h>
#include <avr/pgmspace.h>

void GetMemBuf(uint8_t *dst, PGM_VOID_P src, int len);
void SetMemBuf(PGM_VOID_P dst, uint8_t *src, int len);

#endif

