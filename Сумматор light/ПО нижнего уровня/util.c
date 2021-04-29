#include "util.h"

void mem_reverce(void *data, unsigned char count)
{
	unsigned char i;
	byte temp;
	for(i = 0; i < count/2; i++)
	{
		temp = ((unsigned char *)data)[ i ];
		((unsigned char *)data)[ i ] = ((unsigned char *)data)[ count - i - 1 ];
		((unsigned char *)data)[ count - i -1 ] = temp;
	}
}

inline double Reverse4(double in)
{
    byte tmp;
    byte * p = (byte *)(&in);
    
    tmp = *p;       *p = *(p + 3);       *(p + 3) = tmp;
    tmp = *(p + 1); *(p + 1) = *(p + 2); *(p + 2) = tmp;

    return in;
}

inline unsigned short Reverse2(unsigned short in)
{
    byte tmp;
    byte * p = (byte *)(&in);
    
    tmp = *p;   *p = *(p + 1);  *(p + 1) = tmp;

    return in;
}
