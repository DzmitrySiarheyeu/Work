//=============================================================================
// Copyright (C) 2003 Atmel Corporation
//
// File:			bus.h
// Compiler:		IAR Atmel AVR C/EC++ Compiler
// Output Size:
// Created:			10-Feb-2003	JP (Atmel Finland)
// Modified:
//
// Support Mail:	avr@atmel.com
//
// Description:		General software interface for the bus used for data
//                  transmission of encrypted frames.
//
// Other Info:		
//=============================================================================


#ifndef BUS_H
#define BUS_H

// Initialize bus
extern void busInit(void);

// Prepare bus for self-programming purposes
extern void busPrepare(void);

// Receive one byte
extern unsigned char busReceiveByte(void);

// Set the bus to indicate that the device is busy processing the
// previous received frame
extern void busBusy(void);

// Reply one byte as a response to the received frame
extern void busReplyByte(unsigned char data);

#endif // BUS_H
