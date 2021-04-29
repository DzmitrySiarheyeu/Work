//*****************************************************************************
//
// serial.h - Prototypes for the Serial Port Driver
//
// Copyright (c) 2008-2009 Texas Instruments Incorporated. All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 5450 of the RDK-S2E Firmware Package.
//
//*****************************************************************************

#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "hw_types.h"
#include "config.h"
#include "DriversCore.h"

#if(_TELNET_ || _SER2ENET_)


//*****************************************************************************
//
// Values used to indicate various parity modes for SerialSetParity and
// SerialGetParity.
//
//*****************************************************************************
#define SERIAL_PARITY_NONE  ((unsigned char)1)
#define SERIAL_PARITY_ODD   ((unsigned char)2)
#define SERIAL_PARITY_EVEN  ((unsigned char)3)
#define SERIAL_PARITY_MARK  ((unsigned char)4)
#define SERIAL_PARITY_SPACE ((unsigned char)5)

//*****************************************************************************
//
// Values used to indicate various flow control modes for SerialSetFlowControl
// and SerialGetFlowControl.
//
//*****************************************************************************
#define SERIAL_FLOW_CONTROL_NONE ((unsigned char)1)
#define SERIAL_FLOW_CONTROL_HW   ((unsigned char)3)

//*****************************************************************************
//
// Values used to indicate various flow control modes for SerialSetFlowOut
// and SerialGetFlowOut.
//
//*****************************************************************************
#define SERIAL_FLOW_OUT_SET      ((unsigned char)11)
#define SERIAL_FLOW_OUT_CLEAR    ((unsigned char)12)

//*****************************************************************************
//
// Prototypes for the Serial interface functions.
//
//*****************************************************************************
extern tBoolean SerialSendFull(DEVICE_HANDLE ulPortz);
extern void SerialSend(DEVICE_HANDLE ulPort, unsigned char ucChar);
extern long SerialReceive(DEVICE_HANDLE ulPort, uint8_t *buf, uint16_t count);
extern unsigned long SerialReceiveAvailable(DEVICE_HANDLE ulPort);
extern void SerialSetBaudRate(DEVICE_HANDLE ulPort, unsigned long ulBaudRate);
extern unsigned long SerialGetBaudRate(DEVICE_HANDLE ulPort);
extern void SerialSetDataSize(DEVICE_HANDLE ulPort, unsigned char ucDataSize);
extern unsigned char SerialGetDataSize(DEVICE_HANDLE ulPort);
extern void SerialSetParity(DEVICE_HANDLE ulPort, unsigned char ucParity);
extern unsigned char SerialGetParity(DEVICE_HANDLE ulPort);
extern void SerialSetStopBits(DEVICE_HANDLE ulPort, unsigned char ucStopBits);
extern unsigned char SerialGetStopBits(DEVICE_HANDLE ulPort);
extern void SerialSetBreak(DEVICE_HANDLE ulPort, unsigned char ucBreakControlValue);
extern unsigned char SerialGetBreak(DEVICE_HANDLE ulPort);
extern void SerialSetFlowControl(DEVICE_HANDLE ulPort,
                                 unsigned char ucFlowControl);
extern unsigned char SerialGetFlowControl(DEVICE_HANDLE ulPort);
extern void SerialSetFlowOut(DEVICE_HANDLE ulPort, unsigned char ucFlowValue);
extern unsigned char SerialGetFlowOut(DEVICE_HANDLE ulPort);
extern void SerialPurgeData(DEVICE_HANDLE ulPort,
                            unsigned char ucPurgeCommand);
extern void SerialSetDefault(DEVICE_HANDLE ulPort);
extern void SerialSetFactory(DEVICE_HANDLE ulPort);
extern void SerialSetCurrent(DEVICE_HANDLE ulPort);
extern void SerialInit(void);

extern /*inline*/ void RS485Init(void);
extern /*inline*/ void RS485EnableRcv(void);
extern /*inline*/ void RS485EnableSnd(void);

#endif // _TELNET_ || _SER2ENET_

#endif // __SERIAL_H__
