#pragma once

#include "errors_led.h"
#include "dot.h"

#define TIMER2_CLOCK_DIVIRDER_1     1
#define TIMER2_CLOCK_DIVIRDER_8     2
#define TIMER2_CLOCK_DIVIRDER_64    3
#define TIMER2_CLOCK_DIVIRDER_256   4
#define TIMER2_CLOCK_DIVIRDER_1024  5

//-------------SET--------------------

#define FOUT_TIMER_CLOCK_DIVIRDER   TIMER2_CLOCK_DIVIRDER_8

//#define	INIT_F_OUT_PIN()	 {GPIOSetDir(1, 1, 1); LPC_IOCON->R_PIO1_1 = 0x01UL;}
//#define SET_F_OUT_PIN(s)     (GPIOSetValue(1, 1, s > 0 ? 0 : 1))

#define	INIT_F_OUT_PIN()	 {GPIOSetDir(1, 1, 1); LPC_IOCON->R_PIO1_1 = 0x03UL;}
#define SET_F_OUT_PIN()        LPC_TMR32B1->EMR ^= (3 << 4);



//Для кварцевого генератора 8.0 МГц
#define OUT_FRTBL_TO_FR (TIMER_FREQ)		// коэффициент используемый для перевода значений из таблицы outFrequency в выходную частоты
											// Вых. частота = OUT_FRTBL_TO_FR/(outFrequency[i] + 1)

#define SET_1kHz	((OUT_FRTBL_TO_FR/1000.0) - 1)
#define SET_750Hz	((OUT_FRTBL_TO_FR/750.0) - 1)

void FOutInit(void);
void set_Freq(float freq);
void f_out_proc(void);

