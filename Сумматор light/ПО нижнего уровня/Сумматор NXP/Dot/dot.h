#pragma once

#include "settings.h"
#include "errors_led.h"

#ifndef ERROR_OK
    #define ERROR_OK                0
#endif

#define RS_DOT_STATE_STOP			0
#define RS_DOT_STATE_READY			1
#define RS_DOT_STATE_WAITING		2
#define RS_DOT_STATE_TIMEOUT  		3
#define RS_DOT_STATE_BAD_RESPONSE   4

// Get Fuel percent for all tanks
float GetFuelPercent(void);
unsigned char  DotsProcess(void);

extern float VolumePercent;      // Volume from all tanks
extern float VolumePercent_addr_99;
