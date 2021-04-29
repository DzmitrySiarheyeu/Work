#pragma once

#include "util.h"
#include "errors_led.h"
#include "dot.h"

#define TIMER2_CLOCK_DIVIRDER_1     1
#define TIMER2_CLOCK_DIVIRDER_8     2
#define TIMER2_CLOCK_DIVIRDER_64    3
#define TIMER2_CLOCK_DIVIRDER_256   4
#define TIMER2_CLOCK_DIVIRDER_1024  5

//-------------SET--------------------

#define FOUT_TIMER_CLOCK_DIVIRDER   TIMER2_CLOCK_DIVIRDER_8

//Для кварцевого генератора 8.0 МГц
#define OUT_FRTBL_TO_FR (F_CPU/16.0)		// коэффициент используемый для перевода значений из таблицы outFrequency в выходную частоты
											// Вых. частота = OUT_FRTBL_TO_FR/(outFrequency[i] + 1)

#define SET_1kHz	((OUT_FRTBL_TO_FR/1000.0) - 1)
#define SET_750Hz	((OUT_FRTBL_TO_FR/750.0) - 1)

void FOutInit(void);
void set_Freq(double freq);
