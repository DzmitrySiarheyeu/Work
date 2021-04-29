#include "type.h"
#include "wdt.h"
#include "LPC11xx.h"

extern unsigned int Image$$RW_IRAM1$$ZI$$Base;
extern unsigned int Image$$RW_IRAM1$$ZI$$Length;

extern unsigned int Load$$RW_IRAM1$$Base;
extern unsigned int Image$$RW_IRAM1$$RW$$Base;
extern unsigned int Image$$RW_IRAM1$$RW$$Length;



void Prepare_Zero( unsigned char * dst, unsigned int len )
{
    while(len > 0)
    {
        *dst++ = 0;
        len --;
    }
}

void Prepare_Copy( unsigned char * src, unsigned char * dst, unsigned int len )
{
    if(dst != src)
    {
        while(len > 0)
        {
            *dst++ = *src++;
            len --;
        }
    }
}

void loader(void);
void pre_boot(int flag)
{
	if(flag == 1)
		ConfigurePLL();
#ifdef DEBUG_OFF
	WDTInit();
#endif

	//NVIC_DeInit();
  	//NVIC_SCBDeInit();

	NVIC->ICER[0] = 0xFFFFFFFF;

    // Configure the NVIC Preemption Priority Bits
 	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_5);


	Prepare_Copy((unsigned char *)&Load$$RW_IRAM1$$Base,(unsigned char *)&Image$$RW_IRAM1$$RW$$Base,(unsigned int)&Image$$RW_IRAM1$$RW$$Length);
    Prepare_Zero((unsigned char *)&Image$$RW_IRAM1$$ZI$$Base,(unsigned int)&Image$$RW_IRAM1$$ZI$$Length);

	loader();	
}
