#pragma once

#include "util.h"

#define WRITE_DATA_0_COMMAND  0x11
#define NOP_COMMAND           0x00
#define SHUTDOWN_0_COMMAND    0x21

//-------------SET--------------------

// Max & Min values in Ohm
#define POT_MAX_R   49000
#define POT_MIN_R   9000

#define POT_STEP        0.05
//-------------SET--------------------

#define POT_VALUE(R) ((255 - 255UL * (R - 125) / (50000 - 125)))

#define POT_MAX_VALUE   POT_VALUE(POT_MIN_R)     
#define POT_MIN_VALUE   POT_VALUE(POT_MAX_R) 



void PotInit(void);
void PotSet(unsigned char b);

extern double PotValue; 
