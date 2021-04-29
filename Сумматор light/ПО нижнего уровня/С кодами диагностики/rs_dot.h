#pragma once

#include "mbfanction0.h"
#include "mbclient.h"
#include "settings.h"

#ifndef ERROR_OK_LED
    #define ERROR_OK_LED                0
#endif

#define RS_DOT_STATE_STOP			0
#define RS_DOT_STATE_READY			1
#define RS_DOT_STATE_WAITING		2
#define RS_DOT_STATE_TIMEOUT  		3
#define RS_DOT_STATE_BAD_RESPONSE   4

unsigned char GetRsDotLevelPercent(unsigned char dotAddress, double * value);
void MBMasterIncTimer0(void);
void StartModbusMasterTimer0(void);

extern volatile unsigned char CurrentRsDot;
extern volatile unsigned char CurrentRsDotState;
