#ifndef USER_MODBUS_H
#define USER_MODBUS_H

#include <stdint.h>

//Регистр MB
typedef uint16_t MB_REGISTR;

#ifndef offsetof
  #define offsetof(T, member)     ((size_t)((&((T *)0)->member)))
#endif /* offsetof */

void InitModBusReg(void);
void RegModBusPool(void);
unsigned short Revshort(unsigned short in);
void MBCMethodConfig(void);
int GetMBCMethod(void);
void MBCPortConfig(void);

#endif

