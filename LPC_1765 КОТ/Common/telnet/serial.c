//*****************************************************************************
//
// serial.c - Serial port driver for S2E Module.
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

#include "config.h"

#if(_SER2ENET_)

#include <stdlib.h>

#include "hw_types.h"
#include "ser2enet.h"
#include "serial.h"
#include "telnet.h"

#include "uart_lpc17xx.h"

#include "lpc17xx_gpio.h"

//*****************************************************************************
//
//! \addtogroup serial_api
//! @{
//
//*****************************************************************************


//*****************************************************************************
//
//! The current setting of the serial port
//
//*****************************************************************************
extern tConfigParameters g_sParameters;

//*****************************************************************************
//
//! The current baud rate setting of the serial port
//
//*****************************************************************************
static unsigned long g_ulCurrentBaudRate;
unsigned long g_ulCurrentFlowOut;

//*****************************************************************************
//
//! Checks the availability of the serial port output buffer.
//!
//! \return Returns the number of bytes available in the serial transmit buffer
//
//*****************************************************************************
tBoolean
SerialSendFull(DEVICE_HANDLE ulPort)
{
    //
    // Return the number of bytes available in the tx ring buffer.
    //
	uint16_t count = 0;
	DeviceIOCtl(ulPort, FREE_TX_SPACE, (uint8_t *)&count);
    return count;
}

//*****************************************************************************
//
//! Sends a character to the UART.
//!
//! \param ulPort is the UART port number to be accessed.
//! \param ucChar is the character to be sent.
//!
//! This function sends a character to the UART.  The character will either be
//! directly written into the UART FIFO or into the UART transmit buffer, as
//! appropriate.
//!
//! \return None.
//
//*****************************************************************************
void
SerialSend(DEVICE_HANDLE ulPort, unsigned char ucChar)
{
	WriteDevice(ulPort, &ucChar, 1);
}

//*****************************************************************************
//
//! Receives a character from the UART.
//!
//! \param ulPort is the UART port number to be accessed.
//!
//! This function receives a character from the relevant port's UART buffer.
//!
//! \return Returns -1 if no data is available or the oldest character held in
//! the receive ring buffer.
//
//*****************************************************************************
long
SerialReceive(DEVICE_HANDLE ulPort, uint8_t *buf, uint16_t count)
{
    int iCount;


    iCount = ReadDevice(ulPort, buf, count);

    return iCount;
}   

//*****************************************************************************
//
//! Returns number of characters available in the serial ring buffer.
//!
//! \param ulPort is the UART port number to be accessed.
//!
//! This function will return the number of characters available in the
//! serial ring buffer.
//!
//! \return The number of characters available in the ring buffer..
//
//*****************************************************************************
unsigned long
SerialReceiveAvailable(DEVICE_HANDLE ulPort)
{
    //
    // Return the value.
    //
    uint16_t count = 0;
	DeviceIOCtl(ulPort, RX_DATA_COUNT, (uint8_t *)&count);
	return count;
}

//*****************************************************************************
//
//! Configures the serial port baud rate.
//!
//! \param ulPort is the serial port number to be accessed.
//! \param ulBaudRate is the new baud rate for the serial port.
//!
//! This function configures the serial port's baud rate.  The current
//! configuration for the serial port will be read.  The baud rate will be
//! modified, and the port will be reconfigured.
//!
//! \return None.
//
//*****************************************************************************
void
SerialSetBaudRate(DEVICE_HANDLE ulPort, unsigned long ulBaudRate)
{
    g_ulCurrentBaudRate = ulBaudRate;

    DeviceIOCtl(ulPort, SET_BAUDRATE, (uint8_t *)&ulBaudRate);
}

//*****************************************************************************
//
//! Queries the serial port baud rate.
//!
//! \param ulPort is the serial port number to be accessed.
//!
//! This function will read the UART configuration and return the currently
//! configured baud rate for the selected port.
//!
//! \return The current baud rate of the serial port.
//
//*****************************************************************************
unsigned long
SerialGetBaudRate(DEVICE_HANDLE ulPort)
{
    //
    // Return the current serial port baud rate.
    //
    return(g_ulCurrentBaudRate);
}

//*****************************************************************************
//
//! Configures the serial port data size.
//!
//! \param ulPort is the serial port number to be accessed.
//! \param ucDataSize is the new data size for the serial port.
//!
//! This function configures the serial port's data size.  The current
//! configuration for the serial port will be read.  The data size will be
//! modified, and the port will be reconfigured.
//!
//! \return None.
//
//*****************************************************************************
void
SerialSetDataSize(DEVICE_HANDLE ulPort, unsigned char ucDataSize)
{
    g_sParameters.sPort.ucDataSize = ucDataSize;

    DeviceIOCtl(ulPort, SET_DATASIZE, &ucDataSize);
}

//*****************************************************************************
//
//! Queries the serial port data size.
//!
//! \param ulPort is the serial port number to be accessed.
//!
//! This function will read the UART configuration and return the currently
//! configured data size for the selected port.
//!
//! \return None.
//
//*****************************************************************************
unsigned char
SerialGetDataSize(DEVICE_HANDLE ulPort)
{
    //
    // Return the current data size.
    //
    return(g_sParameters.sPort.ucDataSize);
}

//*****************************************************************************
//
//! Configures the serial port parity.
//!
//! \param ulPort is the serial port number to be accessed.
//! \param ucParity is the new parity for the serial port.
//!
//! This function configures the serial port's parity.  The current
//! configuration for the serial port will be read.  The parity will be
//! modified, and the port will be reconfigured.
//!
//! \return None.
//
//*****************************************************************************
void
SerialSetParity(DEVICE_HANDLE ulPort, unsigned char ucParity)
{
    //
    // Update the configuration with a new parity.
    //
    g_sParameters.sPort.ucParity = ucParity;

    DeviceIOCtl(ulPort, SET_PARITY, &ucParity);
}

//*****************************************************************************
//
//! Queries the serial port parity.
//!
//! \param ulPort is the serial port number to be accessed.
//!
//! This function will read the UART configuration and return the currently
//! configured parity for the selected port.
//!
//! \return Returns the current parity setting for the port.  This will be one
//! of \b SERIAL_PARITY_NONE, \b SERIAL_PARITY_ODD, \b SERIAL_PARITY_EVEN,
//! \b SERIAL_PARITY_MARK, or \b SERIAL_PARITY_SPACE.
//
//*****************************************************************************
unsigned char
SerialGetParity(DEVICE_HANDLE ulPort)
{
    //
    // Return the current data size.
    //
    return(g_sParameters.sPort.ucParity);
}

//*****************************************************************************
//
//! Configures the serial port stop bits.
//!
//! \param ulPort is the serial port number to be accessed.
//! \param ucStopBits is the new stop bits for the serial port.
//!
//! This function configures the serial port's stop bits.  The current
//! configuration for the serial port will be read.  The stop bits will be
//! modified, and the port will be reconfigured.
//!
//! \return None.
//
//*****************************************************************************
void
SerialSetStopBits(DEVICE_HANDLE ulPort, unsigned char ucStopBits)
{

    //
    // Update the configuration with a new stop bits.
    //

    g_sParameters.sPort.ucStopBits = ucStopBits;

    DeviceIOCtl(ulPort, SET_STOPSIZE, &ucStopBits);
}

//*****************************************************************************
//
//! Queries the serial port stop bits.
//!
//! \param ulPort is the serial port number to be accessed.
//!
//! This function will read the UART configuration and return the currently
//! configured stop bits for the selected port.
//!
//! \return None.
//
//*****************************************************************************
unsigned char
SerialGetStopBits(DEVICE_HANDLE ulPort)
{
    //
    // Return the current data size.
    //
    return(g_sParameters.sPort.ucStopBits);
}

//*****************************************************************************
//
//! Sets the serial port BREAK ON/OFF.
//!
//! \param ulPort is the UART port number to be accessed.
//! \param ucBreakControlValue is the value to determine BREAK ON/OFF state.
//! \value 5 for BREAK ON state and value 6 for BREAK OFF state.
//!
//! \return None.
//
//*****************************************************************************
void
SerialSetBreak(DEVICE_HANDLE ulPort, unsigned char ucBreakControlValue)
{
    if(ucBreakControlValue == 5)
	{
        g_sParameters.sPort.ucBreakState = 1;
	}
    else if(ucBreakControlValue == 6) 
	{
        g_sParameters.sPort.ucBreakState = 0;
	}  
  
    DeviceIOCtl(ulPort, SET_CONTROL, &ucBreakControlValue);
}

//*****************************************************************************
//
//! Gets the serial port BREAK ON/OFF state.
//!
//! \return None.
//
//*****************************************************************************
unsigned char
SerialGetBreak(DEVICE_HANDLE ulPort)
{
    return g_sParameters.sPort.ucBreakState;
}

//*****************************************************************************
//
//! Sets the serial port flow control output signal.
//!
//! \param ulPort is the UART port number to be accessed.
//! \param ucFlowValue is the value to program to the flow control pin.  Valid
//! values are \b SERIAL_FLOW_OUT_SET and \b SERIAL_FLOW_OUT_CLEAR.
//!
//! This function will set the flow control output pin to a specified value.
//!
//! \return None.
//
//*****************************************************************************
void
SerialSetFlowOut(DEVICE_HANDLE ulPort, unsigned char ucFlowValue)
{
	if(ucFlowValue == 11)			   // ON
	{
		g_ulCurrentFlowOut = 1;
	}
	else if(ucFlowValue == 12)  // OFF
	{
		g_ulCurrentFlowOut = 0;
	}
}

//*****************************************************************************
//
//! Gets the serial port flow control output signal.
//!
//! \param ulPort is the UART port number to be accessed.
//!
//! This function will set the flow control output pin to a specified value.
//!
//! \return Returns \b SERIAL_FLOW_OUT_SET or \b SERIAL_FLOW_OUT_CLEAR.
//
//*****************************************************************************
unsigned char
SerialGetFlowOut(DEVICE_HANDLE ulPort)
{
    return g_ulCurrentFlowOut;
}

//*****************************************************************************
//
//! Configures the serial port flow control option.
//!
//! \param ulPort is the UART port number to be accessed.
//! \param ucFlowControl is the new flow control setting for the serial port.
//!
//! This function configures the serial port's flow control.  This function
//! will enable/disable the flow control interrupt and the UART transmitter
//! based on the value of the flow control setting and/or the flow control
//! input signal.
//!
//! \return None.
//
//*****************************************************************************
void
SerialSetFlowControl(DEVICE_HANDLE ulPort, unsigned char ucFlowControl)
{
		g_sParameters.sPort.ucFlowControl = ucFlowControl;

}

//*****************************************************************************
//
//! Queries the serial port flow control.
//!
//! \param ulPort is the serial port number to be accessed.
//!
//! This function will return the currently configured flow control for
//! the selected port.
//!
//! \return None.
//
//*****************************************************************************
unsigned char
SerialGetFlowControl(DEVICE_HANDLE ulPort)
{
    //
    // Return the current flow control.
    //
    return(g_sParameters.sPort.ucFlowControl);
}

//*****************************************************************************
//
//! Purges the serial port data queue(s).
//!
//! \param ulPort is the serial port number to be accessed.
//! \param ucPurgeCommand is the command indicating which queue's to purge.
//!
//! This function will purge data from the tx, rx, or both serial port queues.
//!
//! \return None.
//
//*****************************************************************************
void
SerialPurgeData(DEVICE_HANDLE ulPort, unsigned char ucPurgeCommand)
{
    DeviceIOCtl(ulPort, PURGE_DATA, &ucPurgeCommand);
}

//*****************************************************************************
//
//! Configures the serial port to a default setup.
//!
//! \param ulPort is the UART port number to be accessed.
//!
//! This function resets the serial port to a default configuration.
//!
//! \return None.
//
//*****************************************************************************
void
SerialSetDefault(DEVICE_HANDLE ulPort)
{
}

//*****************************************************************************
//
//! Configures the serial port according to the current working parameter
//! values.
//!
//! \param ulPort is the UART port number to be accessed.  Valid values are 0
//! and 1.
//!
//! This function configures the serial port according to the current working
//! parameters in g_sParameters.sPort for the specified port.  The actual
//! parameter set is then read back and g_sParameters.sPort updated to ensure
//! that the structure is correctly synchronized with the hardware.
//!
//! \return None.
//
//*****************************************************************************
void
SerialSetCurrent(DEVICE_HANDLE ulPort)
{
}

//*****************************************************************************
//
//! Configures the serial port to the factory default setup.
//!
//! \param ulPort is the UART port number to be accessed.
//!
//! This function resets the serial port to a default configuration.
//!
//! \return None.
//
//*****************************************************************************
void
SerialSetFactory(DEVICE_HANDLE ulPort)
{
}

//*****************************************************************************
//
//! Initializes the serial port driver.
//!
//! This function initializes and configures the serial port driver.
//!
//! \return None.
//
//*****************************************************************************
void
SerialInit(void)
{
    //dSerialPort = OpenDevice("UART1");
}

//*****************************************************************************
//
//! Initializes the outpun pin for RS485 driver's RE DE functions.
//!
//! \return None.
//
//*****************************************************************************
void RS485Init(void)
{
    //
    //Configure pin as output
    //
    GPIO_SetDir(2, 5, 1);

    //
    //Clear the pin
    //
    GPIO_ClearValue(2, 1 << 5);
}

//*****************************************************************************
//
//! Sets the RS485 RE/DE pins (i.e. enable recieve function).
//!
//! \return None.
//
//*****************************************************************************
void RS485EnableRcv(void)
{
    //
    //Clear the pin
    //
    GPIO_ClearValue(2, 1 << 5);
}

//*****************************************************************************
//
//! Clears the RS485 RE/DE pins (i.e. enable send function).
//!
//! \return None.
//
//*****************************************************************************
void RS485EnableSnd(void)
{
    //
    //Set the pin
    //
    GPIO_SetValue(2, 1 << 5);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************


#endif // _TELNET_ || _SER2ENET_
