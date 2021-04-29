//*****************************************************************************
//
// config.c - Configuration of the serial to Ethernet converter.
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

#if(_TELNET_ || _SER2ENET_)

#include <string.h>
#include "hw_types.h"
#include "ustdlib.h"
#include "config_s2e.h"
#include "serial.h"
#include "telnet.h"

//*****************************************************************************
//
//! \addtogroup config_api
//! @{
//
//*****************************************************************************

////*****************************************************************************
////
////! This structure instance contains the factory-default set of configuration
////! parameters for S2E module.
////
////*****************************************************************************
static const tConfigParameters g_sParametersFactory =
{
    //
    // The sequence number (ucSequenceNum); this value is not important for
    // the copy in SRAM.
    //
    0,

    //
    // The CRC (ucCRC); this value is not important for the copy in SRAM.
    //
    0,

    //
    // The parameter block version number (ucVersion).
    //
    0,

    //
    // Flags (ucFlags)
    //
    0,

    //
    // The TCP port number for UPnP discovery/response (usLocationURLPort).
    //
    6432,

    //
    // Reserved (ucReserved1).
    //
    {
        0, 0
    },

    //
    // Port Parameter Array.
    //
    {
        //
        // The baud rate (ulBaudRate).
        //
        19200,

        //
        // The number of data bits.
        //
        8,

        //
        // The parity (NONE).
        //
        SERIAL_PARITY_NONE,

        //
        // The number of stop bits.
        //
        1,

        //
        // BREAK state off.
        //
        0,

        //
        // The flow control (NONE).
        //
        SERIAL_FLOW_CONTROL_NONE,

        //
        // The telnet session timeout (ulTelnetTimeout).
        //
        0,

        //
        // The telnet session listen or local port number
        // (usTelnetLocalPort).
        //
        23,

        //
        // The telnet session remote port number (when in client mode).
        //
        23,

        //
        // The IP address to which a connection will be made when in telnet
        // client mode.  This defaults to an invalid address since it's not
        // sensible to have a default value here.
        //
        0x00000000,

        //
        // Flags indicating the operating mode for this port.
        //
        PORT_TELNET_SERVER,

        //
        // Reserved (ucReserved0).
        //
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        }
    },

    //
    // Module Name (ucModName).
    //
    {
        'T','I',' ','S','t','e','l','l','a','r','i','s',' ','S','e','r',
        'i','a','l','2','E','t','h','e','r','n','e','t',' ','M','o','d',
        'u','l','e', 0,  0,  0,  0,  0,
    },

    //
    // Static IP address (used only if indicated in ucFlags).
    //
    0x00000000,

    //
    // Default gateway IP address (used only if static IP is in use).
    //
    0x00000000,

    //
    // Subnet mask (used only if static IP is in use).
    //
    0xFFFFFF00,

    //
    // ucReserved2 (compiler will pad to the full length)
    //
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    }
};
//
////*****************************************************************************
////
////! This structure instance contains the run-time set of configuration
////! parameters for S2E module.  This is the active parameter set and may
////! contain changes that are not to be committed to flash.
////
////*****************************************************************************
tConfigParameters g_sParameters;
//
//
////*****************************************************************************
////
////! Loads the S2E parameter block from factory-default table.
////!
////! This function is called to load the factory default parameter block.
////!
////! \return None.
////
////*****************************************************************************
void
ConfigLoadFactory(void)
{
    //
    // Copy the factory default parameter set to the active and working
    // parameter blocks.
    //
    g_sParameters = g_sParametersFactory;
}
//
////*****************************************************************************
////
////! Initializes the configuration parameter block.
////!
////! This function initializes the configuration parameter block.  If the
////! version number of the parameter block stored in flash is older than
////! the current revision, new parameters will be set to default values as
////! needed.
////!
////! \return None.
////
////*****************************************************************************
void
ConfigInit(void)
{
    //
    // load the factory default values.
    //
    ConfigLoadFactory();
}
//
//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

#endif // _TELNET_ || _SER2ENET_
