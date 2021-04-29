#pragma once

#include "util.h"
#include "settings.h"
#include "rs_dot.h"
#include "freq_dot.h"
#include "analog_dot.h"
#include "errors_led.h"

// Get Fuel percent for all tanks
double GetFuelPercent(void);
unsigned char  DotsProcess();

extern double VolumePercent;      // Volume from all tanks
