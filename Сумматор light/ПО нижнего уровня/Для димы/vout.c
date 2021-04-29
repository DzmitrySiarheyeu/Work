#include "vout.h"

//----------------- V OUT TIMER1-------------
inline void VOutInit()
{
    cbi(PRR,  PRTIM1);      //On TIMER1

    sbi(OC1B_DDR, OC1B_PIN);    // pin to out

    TCCR1B = VOUT_TIMER_CLOCK_DIVIRDER;

    sbi(TCCR1A, COM1B1);    //  Clear OC1A/OC1B on Compare Match, set
                            //  OC1A/OC1B at BOTTOM (non-inverting mode)

     // Fast PWM     OCR1A   BOTTOM  TOP
    sbi(TCCR1A, WGM10);
    sbi(TCCR1A, WGM11);
    sbi(TCCR1B, WGM12);
    sbi(TCCR1B, WGM13);

    OCR1A = VOUT_TIMER_MAX_TIMER_VALUE;

    sbi(TCCR1B, WGM12);
    sbi(TCCR1B, WGM13);
     
    // OFF PWM
    OCR1B = 0;

	// Interrupt enable
    sbi(TIMSK1, TOIE1);
}

unsigned short old_value = 0;
unsigned short new_value = 0;
inline void VOutWidthSet(unsigned short value)
{
    new_value = value;
	if ((value > (old_value + 100)) || (value < (old_value - 100)))
	{
		old_value = value;
	}
	cli();
	OCR1B = value;
	sei();
	
	old_value = value;
}

inline void VOutProcess()
{
    double k = K_12;
    double U;

	U = Reverse4(RegFile.mb_struct.U_max) * VolumePercent;
	
	VOutWidthSet(VOUT_TIMER_MAX_TIMER_VALUE * (U / (k * VCC)));
}

