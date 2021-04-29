//*****************************************************************************
//
// telnet.h - Definitions for the telnet command interface.
//
// Copyright (c) 2007-2009 Texas Instruments Incorporated. All rights reserved.
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

#ifndef __TELNET_H__
#define __TELNET_H__

#include "config.h"

#if(_TELNET_ || _SER2ENET_)

#include "lwip/opt.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/tcp_impl.h"
#include "ppp/ppp.h"
#include "lwip/tcp.h"

#include "hw_types.h"
#include "stdint.h"
#include "DriversCore.h"

//*****************************************************************************
//
//! The bit in the flag that is set when the remote client has sent a WILL
//! request for SUPPRESS_GA and the server has accepted it.
//
//*****************************************************************************
#define OPT_FLAG_WILL_SUPPRESS_GA 1

//*****************************************************************************
//
//! The bit in the flag that is set when the remote client has sent a DO
//! request for SUPPRESS_GA and the server has accepted it.
//
//*****************************************************************************
#define OPT_FLAG_DO_SUPPRESS_GA 1

//*****************************************************************************
//
//! The bit in the flag that is set when the remote client has sent a WILL
//! request for COM_PORT and the server has accepted it.
//
//*****************************************************************************
#define OPT_FLAG_WILL_RFC2217   2

//*****************************************************************************
//
//! The bit in the flag that is set when the remote client has sent a DO
//! request for COM_PORT and the server has accepted it.
//
//*****************************************************************************
#define OPT_FLAG_DO_RFC2217     3

//*****************************************************************************
//
//! The bit in the flag that is set when a connection is operating as a telnet
//! server.  If clear, this implies that this connection is a telnet client.
//
//*****************************************************************************
#define OPT_FLAG_SERVER         4


////*****************************************************************************
////
////! The possible states of the TCP session.
////
////*****************************************************************************
//typedef enum
//{
//    //
//    //! The TCP session is idle.  No connection has been attempted, nor has it
//    //! been configured to listen on any port.
//    //
//    STATE_TCP_IDLE,
//
//    //
//    //! The TCP session is listening (server mode).
//    //
//    STATE_TCP_LISTEN,
//
//    //
//    //! The TCP session is connecting (client mode).
//    //
//    STATE_TCP_CONNECTING,
//
//    //
//    //! The TCP session is connected.
//    //
//    STATE_TCP_CONNECTED,
//}
//tTCPState;

//*****************************************************************************
//
//! The possible states of the telnet option parser.
//
//*****************************************************************************
typedef enum
{
    //
    //! The telnet option parser is in its normal mode.  Characters are passed
    //! as is until an IAC byte is received.
    //
    STATE_NORMAL,

    //
    //! The previous character received by the telnet option parser was an IAC
    //! byte.
    //
    STATE_IAC,

    //
    //! The previous character sequence received by the telnet option parser
    //! was IAC WILL.
    //
    STATE_WILL,

    //
    //! The previous character sequence received by the telnet option parser
    //! was IAC WONT.
    //
    STATE_WONT,

    //
    //! The previous character sequence received by the telnet option parser
    //! was IAC DO.
    //
    STATE_DO,

    //
    //! The previous character sequence received by the telnet option parser
    //! was IAC DONT.
    //
    STATE_DONT,

    //
    //! The previous character sequence received by the telnet option parser
    //! was IAC SB.
    //
    STATE_SB,

    //
    //! The previous character sequence received by the telnet option parser
    //! was IAC SB n, where n is an unsupported option.
    //
    STATE_SB_IGNORE,

    //
    //! The previous character sequence received by the telnet option parser
    //! was IAC SB n, where n is an unsupported option.
    //
    STATE_SB_IGNORE_IAC,

    //
    //! The previous character sequence received by the telnet option parser
    //! was IAC SB COM-PORT-OPTION (in other words, RFC 2217).
    //
    STATE_SB_RFC2217,
}
tTelnetState;

//*****************************************************************************
//
//! The possible states of the telnet COM-PORT option parser.
//
//*****************************************************************************
typedef enum
{
    //
    //! The telnet COM-PORT option parser is ready to process the first
    //! byte of data, which is the sub-option to be processed.
    //
    STATE_2217_GET_COMMAND,

    //
    //! The telnet COM-PORT option parser is processing data bytes for the
    //! specified command/sub-option.
    //
    STATE_2217_GET_DATA,

    //
    //! The telnet COM-PORT option parser has received an IAC in the data
    //! stream.
    //
    STATE_2217_GET_DATA_IAC,

}
tRFC2217State;

//*****************************************************************************
//
//! Структура содержит часть сервера, котороя относится к rfc2217
//
//*****************************************************************************
//
typedef struct
{
	//
    //! This value holds the UART Port Number for this telnet session.
    //
    DEVICE_HANDLE hSerialPort;

    //
    //! The current state of the telnet option parser.
    //
    tRFC2217State eRFC2217State;

    //
    //! The COM-PORT Command being processed.
    //
    unsigned char ucRFC2217Command;

    //
    //! The COM-PORT value received (associed with the COM-PORT Command).
    //
    unsigned long ulRFC2217Value;

    //
    //! The index into the COM-PORT value received (for multi-byte values).
    //
    unsigned char ucRFC2217Index;

    //
    //! The maximum number of bytes expected (0 means ignore data).
    //
    unsigned char ucRFC2217IndexMax;

    //
    //! The response buffer for RFC2217 commands.
    //
    unsigned char pucRFC2217Response[16];

    //
    //! The RFC 2217 flow control value.
    //
    unsigned char ucRFC2217FlowControl;

    //
    //! The modem state mask.
    //
    unsigned char ucRFC2217ModemMask;

    //
    //! The line state mask.
    //
    unsigned char ucRFC2217LineMask;

    //
    //! The reported modem state.
    //
    unsigned char ucModemState;

    //
    //! The last reported modem state.
    //
    unsigned char ucLastModemState;
} __attribute__ ((__packed__)) tRFC2217SessionData;

//*****************************************************************************
//
//! This structure is used holding the state of a given telnet session.
//
//*****************************************************************************
typedef struct
{
//    //
//    //! This value holds the pointer to the TCP PCB associated with this
//    //! connected telnet session.
//    //
//    struct tcp_pcb *pConnectPCB;
//
//    //
//    //! This value holds the pointer to the TCP PCB associated with this
//    //! listening telnet session.
//    //
//    struct tcp_pcb *pListenPCB;
//
//    //
//    //! The current state of the TCP session.
//    //
//    tTCPState eTCPState;

    //
    //! The current state of the telnet option parser.
    //
    tTelnetState eTelnetState;

    //
    //! The listen port for the telnet server or the local port for the telnet
    //! client.
    //

    //
    //! Flags for various options associated with the telnet session.
    //
    volatile unsigned char ucFlags;

//    //
//    //! A counter for the TCP connection timeout.
//    //
//    unsigned long ulConnectionTimeout;

//    //
//    //! The max time for TCP connection timeout counter.
//    //
//    unsigned long ulMaxTimeout;

//    //
//    //! This value holds an array of pbufs.
//    //
//    struct pbuf *pBufQ[PBUF_POOL_SIZE];

//    //
//    //! This value holds the read index for the pbuf queue.
//    //
//    int iBufQRead;

//    //
//    //! This value holds the write index for the pbuf queue.
//    //
//    int iBufQWrite;

//    //
//    //! This value holds the head of the pbuf that is currently being
//    //! processed (that has been popped from the queue).
//    //
//    struct pbuf *pBufHead;

//    //
//    //! This value holds the actual pbuf that is being processed within the
//    //! pbuf chain pointed to by the pbuf head.
//    //
//    struct pbuf *pBufCurrent;

//    //
//    //! This value holds the offset into the payload section of the current
//    //! pbuf.
//    //
//    unsigned long ulBufIndex;

	//
    //! Структура для управления rfc2217
    //
	tRFC2217SessionData *pRFC2217Session;

    //
    //! The indication that link layer has been lost.
    //
    tBoolean bLinkLost;

    //
//    //! Debug and diagnostic counters.
//    //
//    unsigned char ucErrorCount;
//    unsigned char ucReconnectCount;
//    unsigned char ucConnectCount;

    //
    //! The last error reported by lwIP while attempting to make a connection.
    //
    err_t eLastErr;
} __attribute__ ((__packed__)) tTelnetSessionData;

#define TL_TIMEOUT     300
#define TL_INPBUFSIZE  512

typedef struct lelem_s  lelem_t;
struct lelem_s
{
  lelem_t   *prev;
  lelem_t   *next;
};


typedef struct connect_s
{
	lelem_t             list;
	uint32_t			lastTrans;
	tTelnetSessionData  TelnetSession;	
	int                 sock;
	u16_t				buflen;
	char				rxbuffer[TL_INPBUFSIZE];
}TL_CONNECTION;


//*****************************************************************************
//
// Telnet commands, as defined by RFC854.
//
//*****************************************************************************
#define TELNET_IAC              255
#define TELNET_WILL             251
#define TELNET_WONT             252
#define TELNET_DO               253
#define TELNET_DONT             254
#define TELNET_SE               240
#define TELNET_NOP              241
#define TELNET_DATA_MARK        242
#define TELNET_BREAK            243
#define TELNET_IP               244
#define TELNET_AO               245
#define TELNET_AYT              246
#define TELNET_EC               247
#define TELNET_EL               248
#define TELNET_GA               249
#define TELNET_SB               250

//*****************************************************************************
//
// Telnet options, as defined by RFC856-RFC861.
//
//*****************************************************************************
#define TELNET_OPT_BINARY       0
#define TELNET_OPT_ECHO         1
#define TELNET_OPT_SUPPRESS_GA  3
#define TELNET_OPT_STATUS       5
#define TELNET_OPT_TIMING_MARK  6
#define TELNET_OPT_EXOPL        255

//*****************************************************************************
//
// Telnet Com Port Control options, as defined by RFC2217.
//
//*****************************************************************************
#define TELNET_OPT_RFC2217              44

//
// Client to Server Option Defintions
//
#define TELNET_C2S_SIGNATURE            0
#define TELNET_C2S_SET_BAUDRATE         1
#define TELNET_C2S_SET_DATASIZE         2
#define TELNET_C2S_SET_PARITY           3
#define TELNET_C2S_SET_STOPSIZE         4
#define TELNET_C2S_SET_CONTROL          5
#define TELNET_C2S_NOTIFY_LINESTATE     6
#define TELNET_C2S_NOTIFY_MODEMSTATE    7
#define TELNET_C2S_FLOWCONTROL_SUSPEND  8
#define TELNET_C2S_FLOWCONTROL_RESUME   9
#define TELNET_C2S_SET_LINESTATE_MASK   10
#define TELNET_C2S_SET_MODEMSTATE_MASK  11
#define TELNET_C2S_PURGE_DATA           12

//
// Server to Client Option Definitions
//
#define TELNET_S2C_SIGNATURE            (0 + 100)
#define TELNET_S2C_SET_BAUDRATE         (1 + 100)
#define TELNET_S2C_SET_DATASIZE         (2 + 100)
#define TELNET_S2C_SET_PARITY           (3 + 100)
#define TELNET_S2C_SET_STOPSIZE         (4 + 100)
#define TELNET_S2C_SET_CONTROL          (5 + 100)
#define TELNET_S2C_NOTIFY_LINESTATE     (6 + 100)
#define TELNET_S2C_NOTIFY_MODEMSTATE    (7 + 100)
#define TELNET_S2C_FLOWCONTROL_SUSPEND  (8 + 100)
#define TELNET_S2C_FLOWCONTROL_RESUME   (9 + 100)
#define TELNET_S2C_SET_LINESTATE_MASK   (10 + 100)
#define TELNET_S2C_SET_MODEMSTATE_MASK  (11 + 100)
#define TELNET_S2C_PURGE_DATA           (12 + 100)



void TelnetTask(void *pvParameters);


//*****************************************************************************//
//**********************TELNET FUNCTION FOR SHELL******************************//
//*****************************************************************************//

void
shell_output(TL_CONNECTION *conn, char *str1, char *str2);

#define TELNET_CMD_MAX_LEN   (128)
#define TELNET_MAX_LEN       (TELNET_CMD_MAX_LEN)

// для обработки команды телнет
struct telnetd_cmd_in_buf {
  // максимальная длина команды - TELNET_CMD_MAX_LEN символа (включая символ новой строки)
  char buf[TELNET_CMD_MAX_LEN];
  uint8_t bufptr;
};


//*****************************************************************************//
//*****************************************************************************//
//*****************************************************************************//


#endif // _TELNET_ || _SER2ENET_

#endif // __TELNET_H__
