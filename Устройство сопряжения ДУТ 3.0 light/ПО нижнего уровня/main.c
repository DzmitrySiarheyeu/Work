#include "util.h"
#include "settings.h"
#include "vout.h"
#include "rout.h"
#include "fout.h"
#include "potentiometer.h"
#include "freq_dot.h"
#include "rs_dot.h"
#include "errors_led.h"

#include "dot.h"

#include "base_type.h"
#include "mbclient.h"
#include "mbfanction0.h"
#include "omnicomm.h"

inline void Init()
{
    eeprom_read_block(&RegFile, &EepromRegFile, sizeof(REGISTER_FILE));
	RegFile.mb_struct.U_type = VERSION_PO;
	Read_parameters();; // Omnicomm
//	RESET_FUSE_INIT; // Не хватает ножек на разъеме
    LED_INIT;
    RESERV_INIT;
	LedTimerInit();
    ROutInit();
	FOutInit();
#ifdef FREQUENCY_INPUT
    FreqDotInit();
#endif
	VOutInit();
    InitUART();
	
	sei(); 
}


inline void ReservProcess()
{
    double RezervPoint = Reverse4(RegFile.mb_struct.Reserv);
	
	if (VolumePercent < RezervPoint)
	{
		RESERV_ON;
	}
	if (VolumePercent > RezervPoint + 0.01)	
	{
		RESERV_OFF;
	}
	
}

int main(void)
{
	wdt_enable(WDTO_1S);
	unsigned char rout_error = 0; 
	unsigned char dot_error = 0;

   	Init();
	while (1)
	{
		wdt_reset();
	    ModbusProcess();
        dot_error  = DotsProcess();
		set_Freq(VolumePercent * 1000.0 + 500.0);
#ifdef	R_OUT_ENABLE
		rout_error = ROutProcess();
#endif
		VOutProcess();
        ReservProcess();
		ErrorProcess(dot_error, rout_error);
	}
}
