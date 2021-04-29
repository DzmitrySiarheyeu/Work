//*****************************************************************************
//
// telnet.c - Telnet session support routines.
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

#include <stdlib.h>

#include "hw_types.h"
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include "lwip/sys.h"
#include "lwip/def.h"
#include "lwip/sockets.h"
#include "lwip/stats.h"
#include "lwip/ip.h"
#include "telnet.h"
#include "ModemTask.h" 



#define sysGetErrno()       	errno
#define sysSleep(ms)            posTaskSleep(ms)
#define sysMemAlloc(s)          mem_malloc(s)
#define sysMemFree(x)           mem_free(x)
#define sysMemCopy(d,s,l)       memcpy(d,s,l)
#define sysMemSet(d,v,s)        memset(d,v,s)


typedef void (*TL_POOL_FUNC)(TL_CONNECTION *conn);
typedef void (*TL_ACCEPT_FUNC)(TL_CONNECTION *conn);

typedef struct tl_server_s
{
	struct tl_server_s *next;
	lelem_t     		connListRoot;
	uint16_t			port;
	fd_set      		SocketSet;
	int         		serverSocket;
	u32_t       		open_connections;
	uint8_t 			RFC2217Session;
	uint8_t     		conn_limit;
	TL_POOL_FUNC		tl_pool_func;
	TL_ACCEPT_FUNC		tl_accept_func;
}TL_SERVER;

TL_SERVER *firs_tl_server = NULL;

extern struct ip_addr ip_adr;

#define INIT_LIST_HEAD(head) \
  do { (head)->prev = head; (head)->next = head; } while(0)
#define END_OF_LIST(head, elem)  ((void*)(head) == (void*)(elem))
#define NEXT_LIST_ELEM(elem, type)   \
          (type)(void*)((lelem_t*)(void*)(elem))->next
#define PREV_LIST_ELEM(elem, type)   \
          (type)(void*)((lelem_t*)(void*)(elem))->prev
#define FIRST_LIST_ELEM(head, type)  NEXT_LIST_ELEM(head, type)
#define LAST_LIST_ELEM(head, type)   PREV_LIST_ELEM(head, type)


/*---------------------------------------------------------------------------
 *  FUNCTION PROTOTYPES
 *-------------------------------------------------------------------------*/

static int		tl_newServerSocket(TL_SERVER *serv);
static int		tl_fdSanity(TL_SERVER *serv);
static int		tl_processConnections(TL_SERVER *serv, fd_set *clientset, int fdcount);

static void     tl_acceptClient(TL_SERVER *serv);
static void     tl_timeout(TL_SERVER *serv, uint32_t cur_t);
static void    	tl_listAddTail(lelem_t *headelem, lelem_t *elem);
//static void    	mb_listAddHead(lelem_t *headelem, lelem_t *elem);
static void    	tl_listDel(lelem_t *elem);

static void		tl_destroyConnection(TL_CONNECTION *conn);
static int		tl_addSocket(TL_SERVER *serv, const int sock);
static void		tl_delSocket(TL_SERVER *serv, const int sock);
static int		tl_newConnection(TL_SERVER *serv, int socket);





static void add_tl_serv_to_list(TL_SERVER *serv)
{
	if(firs_tl_server == NULL)
	{
		firs_tl_server = serv;
		firs_tl_server->next = NULL;
	}
	else
	{
		serv->next = firs_tl_server;
		firs_tl_server = serv;
	}
}

static TL_SERVER * get_tl_serv(TL_CONNECTION *conn)
{
	TL_SERVER *serv = firs_tl_server;
	while(serv)
	{
		if(FD_ISSET(conn->sock, &serv->SocketSet))
			return serv;
		else serv = serv->next;
	}
	return NULL;
}




#if(_TELNET_)
    #include "shell.h"
	
	static void form_telnet_cmd(TL_CONNECTION *conn, uint8_t c);
	struct telnetd_cmd_in_buf s;

#endif // _TELNET_


#if(_SER2ENET_)

#include "ser2enet.h"
#include "serial.h"
#include "SetParametrs.h"
#include "tcp_redirector.h"

tConfigParameters g_sParameters;

//*****************************************************************************
//
//! Processes a telnet character in IAC SB COM-PORT-OPT mode (RFC2217).
//!
//! \param pState is the telnet state data for this connection.
//!
//! This function will handle the RFC 2217 options.
//!
//! The response (if any) is written into the telnet transmit buffer.
//!
//! \return None.
//
//*****************************************************************************

static void	
TelnetProcessRFC2217Command(TL_CONNECTION *conn)
{
    int iIndex = 0;
    unsigned long ulTemp;
	tTelnetSessionData *pState = &conn->TelnetSession;
    //
    // Set the com port option based on the command sent.
    //
    switch(pState->pRFC2217Session->ucRFC2217Command)
    {
        //
        // Set serial port baud rate.
        //
        case TELNET_C2S_SET_BAUDRATE:
        {
            if(pState->pRFC2217Session->ulRFC2217Value && pState->pRFC2217Session->ucRFC2217Index)
            {
                DEBUG_PRINTF_ONE("RFC2217 SET_BAUDRATE - %lu \n\r", pState->pRFC2217Session->ulRFC2217Value);

                SerialSetBaudRate(pState->pRFC2217Session->hSerialPort,
                                  pState->pRFC2217Session->ulRFC2217Value);
            }
            break;
        }

        //
        // Set serial port data size.
        //
        case TELNET_C2S_SET_DATASIZE:
        {
            if(pState->pRFC2217Session->ulRFC2217Value && pState->pRFC2217Session->ucRFC2217Index)
            {
                DEBUG_PRINTF_ONE("RFC2217 SET_DATASIZE - %lu \n\r", pState->pRFC2217Session->ulRFC2217Value);

                SerialSetDataSize(pState->pRFC2217Session->hSerialPort,
                                 (unsigned char)pState->pRFC2217Session->ulRFC2217Value);
            }
            break;
        }

        //
        // Set serial port parity.
        //
        case TELNET_C2S_SET_PARITY:
        {
            if(pState->pRFC2217Session->ulRFC2217Value && pState->pRFC2217Session->ucRFC2217Index)
            {
                DEBUG_PRINTF_ONE("RFC2217 SET_PARITY - %lu \n\r", pState->pRFC2217Session->ulRFC2217Value);
                SerialSetParity(pState->pRFC2217Session->hSerialPort,
                               (unsigned char)pState->pRFC2217Session->ulRFC2217Value);
            }
            break;
        }

        //
        // Set serial port stop bits.
        //
        case TELNET_C2S_SET_STOPSIZE:
        {
            if(pState->pRFC2217Session->ulRFC2217Value && pState->pRFC2217Session->ucRFC2217Index)
            {
                DEBUG_PRINTF_ONE("RFC2217 SET_STOPSIZE - %lu \n\r", pState->pRFC2217Session->ulRFC2217Value);
                SerialSetStopBits(pState->pRFC2217Session->hSerialPort,
                                 (unsigned char)pState->pRFC2217Session->ulRFC2217Value);
            }
            break;
        }

        //
        // Set serial port flow control.
        //
        case TELNET_C2S_SET_CONTROL:
        {
            DEBUG_PRINTF_ONE("RFC2217 SET_CONTROL - %lu \n\r", pState->pRFC2217Session->ulRFC2217Value);

            switch(pState->pRFC2217Session->ulRFC2217Value)
            {
                case 1:
                case 3:
                {
                    SerialSetFlowControl(pState->pRFC2217Session->hSerialPort,
                            (unsigned char)pState->pRFC2217Session->ulRFC2217Value);
                    break;
                }

                case 5:
                case 6:
                {
                    SerialSetBreak(pState->pRFC2217Session->hSerialPort,
                            (unsigned char)pState->pRFC2217Session->ulRFC2217Value);
                    break;
                }

                case 11:
                case 12:
                {
                    SerialSetFlowOut(pState->pRFC2217Session->hSerialPort,
                            (unsigned char)pState->pRFC2217Session->ulRFC2217Value);
                    break;
                }

                default:
                {
                    break;
                }
            }
            break;
        }

        //
        // Set flow control suspend/resume option.
        //
        case TELNET_C2S_FLOWCONTROL_SUSPEND:
        case TELNET_C2S_FLOWCONTROL_RESUME:
        {
            DEBUG_PRINTF_ONE("RFC2217 FLOWCONTROL - %lu \n\r", pState->pRFC2217Session->ucRFC2217Command);

            pState->pRFC2217Session->ucRFC2217FlowControl =
                (unsigned char)pState->pRFC2217Session->ucRFC2217Command;
            break;
        }

        //
        // Set the line state mask.
        //
        case TELNET_C2S_SET_LINESTATE_MASK:
        {
            DEBUG_PRINTF_ONE("RFC2217 SET_LINESTATE_MASK - %lu \n\r", pState->pRFC2217Session->ulRFC2217Value);
            pState->pRFC2217Session->ucRFC2217LineMask = (unsigned char)pState->pRFC2217Session->ulRFC2217Value;
            break;
        }

        //
        // Set the modem state mask.
        //
        case TELNET_C2S_SET_MODEMSTATE_MASK:
        {
            DEBUG_PRINTF_ONE("RFC2217 SET_MODEMSTATE_MASK - %lu \n\r", pState->pRFC2217Session->ulRFC2217Value);
            pState->pRFC2217Session->ucRFC2217ModemMask = (unsigned char)pState->pRFC2217Session->ulRFC2217Value;
            break;
        }

        //
        // Purge the serial port data.
        //
        case TELNET_C2S_PURGE_DATA:
        {
            DEBUG_PRINTF_ONE("RFC2217 PURGE_DATA - %lu \n\r", pState->pRFC2217Session->ulRFC2217Value);
            SerialPurgeData(pState->pRFC2217Session->hSerialPort,
                            (unsigned char)pState->pRFC2217Session->ulRFC2217Value);
            break;
        }
    }

    //
    // Now, send an acknowledgement response with the current setting.
    //
    pState->pRFC2217Session->pucRFC2217Response[iIndex++] = TELNET_IAC;
    pState->pRFC2217Session->pucRFC2217Response[iIndex++] = TELNET_SB;
    pState->pRFC2217Session->pucRFC2217Response[iIndex++] = TELNET_OPT_RFC2217;

    //
    // Use the "Server to Client" response code.
    //
    pState->pRFC2217Session->pucRFC2217Response[iIndex++] = (pState->pRFC2217Session->ucRFC2217Command + 100);

    //
    // Read the appropriate value from the serial port module.
    //
    switch(pState->pRFC2217Session->ucRFC2217Command)
    {
        case TELNET_C2S_SET_BAUDRATE:
        {
            ulTemp = SerialGetBaudRate(pState->pRFC2217Session->hSerialPort);
            break;
        }

        case TELNET_C2S_SET_DATASIZE:
        {
            ulTemp = SerialGetDataSize(pState->pRFC2217Session->hSerialPort);
            break;
        }

        case TELNET_C2S_SET_PARITY:
        {
            ulTemp = SerialGetParity(pState->pRFC2217Session->hSerialPort);
            break;
        }

        case TELNET_C2S_SET_STOPSIZE:
        {
            ulTemp = SerialGetStopBits(pState->pRFC2217Session->hSerialPort);
            break;
        }

        case TELNET_C2S_SET_CONTROL:
        {
            switch(pState->pRFC2217Session->ulRFC2217Value)
            {
                case 0:
                case 1:
                case 2:
                case 3:
                {
                    ulTemp = SerialGetFlowControl(pState->pRFC2217Session->hSerialPort);
                    break;
                }
                case 4:
                case 5:
                case 6:
                {
                    ulTemp = SerialGetBreak(pState->pRFC2217Session->hSerialPort);
                    break;
                }

                case 10:
                case 11:
                case 12:
                {
                    ulTemp = SerialGetFlowOut(pState->pRFC2217Session->hSerialPort);
                    break;
                }

                default:
                {
                    ulTemp = 0;
                    break;
                }
            }
            break;
        }

        case TELNET_C2S_FLOWCONTROL_SUSPEND:
        case TELNET_C2S_FLOWCONTROL_RESUME:
        case TELNET_C2S_SET_LINESTATE_MASK:
        case TELNET_C2S_SET_MODEMSTATE_MASK:
        case TELNET_C2S_PURGE_DATA:
        {
            ulTemp = pState->pRFC2217Session->ulRFC2217Value;
            break;
        }

        default:
        {
            ulTemp = 0;
            break;
        }
    }

    //
    // Now, set the response value into the output buffer.
    //
    if(pState->pRFC2217Session->ucRFC2217Command == TELNET_C2S_SET_BAUDRATE)
    {
        //
        // 4-byte response value.
        //
        pState->pRFC2217Session->pucRFC2217Response[iIndex++] = ((ulTemp >> 24) & 0xFF);
        if(((ulTemp >> 24) & 0xFF) == TELNET_IAC)
        {
            pState->pRFC2217Session->pucRFC2217Response[iIndex++] = TELNET_IAC;
        }
        pState->pRFC2217Session->pucRFC2217Response[iIndex++] = ((ulTemp >> 16) & 0xFF);
        if(((ulTemp >> 16) & 0xFF) == TELNET_IAC)
        {
            pState->pRFC2217Session->pucRFC2217Response[iIndex++] = TELNET_IAC;
        }
        pState->pRFC2217Session->pucRFC2217Response[iIndex++] = ((ulTemp >> 8) & 0xFF);
        if(((ulTemp >> 8) & 0xFF) == TELNET_IAC)
        {
            pState->pRFC2217Session->pucRFC2217Response[iIndex++] = TELNET_IAC;
        }
        pState->pRFC2217Session->pucRFC2217Response[iIndex++] = ((ulTemp >> 0) & 0xFF);
        if(((ulTemp >> 0) & 0xFF) == TELNET_IAC)
        {
            pState->pRFC2217Session->pucRFC2217Response[iIndex++] = TELNET_IAC;
        }
    }
    else
    {
        //
        // 1-byte resposne value.
        //
        pState->pRFC2217Session->pucRFC2217Response[iIndex++] = ((ulTemp >> 0) & 0xFF);
        if(((ulTemp >> 0) & 0xFF) == TELNET_IAC)
        {
            pState->pRFC2217Session->pucRFC2217Response[iIndex++] = TELNET_IAC;
        }
    }

    //
    // Finish out the packet.
    //
    pState->pRFC2217Session->pucRFC2217Response[iIndex++] = TELNET_IAC;
    pState->pRFC2217Session->pucRFC2217Response[iIndex++] = TELNET_SE;

    //
    // Write the packet to the TCP output buffer.
    //

    if(pState->pRFC2217Session->ucRFC2217Command == TELNET_C2S_SET_CONTROL && pState->pRFC2217Session->ulRFC2217Value == 5)
    {
        pState->pRFC2217Session->pucRFC2217Response[iIndex++] = 0;
        pState->pRFC2217Session->pucRFC2217Response[iIndex++] = 0;
        //tcp_write(pState->pConnectPCB, pState->pRFC2217Session->pucRFC2217Response, iIndex, 1);
		if (send(conn->sock, pState->pRFC2217Session->pucRFC2217Response, iIndex, 0) < 0)
	  	{
	    	if (sysGetErrno() == EPIPE)
	    	{
	      		tl_destroyConnection(conn);
	    	}
	  	}
    }
}

//*****************************************************************************
//
//! Processes a telnet character in IAC SB COM-PORT-OPT mode (RFC2217).
//!
//! \param ucChar is the telnet option in question.
//! \param pState is the telnet state data for this connection.
//!
//! This function will handle the RFC 2217 options.
//!
//! The response (if any) is written into the telnet transmit buffer.
//!
//! \return None.
//
//*****************************************************************************
static void
TelnetProcessRFC2217Character(unsigned char ucChar, TL_CONNECTION *conn)
{
    unsigned char *pucValue;
	tTelnetSessionData *pState = &conn->TelnetSession;

    //
    // Determine the current state of the telnet COM-PORT option parser.
    //
    switch(pState->pRFC2217Session->eRFC2217State)
    {
        //
        // The initial state of the parser.  The IAC SB COM-PORT-OPTION data
        // sequence has been received.  The next character (this one) is the
        // specific option/command that is to be negotiated.
        //
        case STATE_2217_GET_COMMAND:
        {
            //
            // Save the command option.
            //
            pState->pRFC2217Session->ucRFC2217Command = ucChar;

            //
            // Initialize the data value for this command.
            //
            pState->pRFC2217Session->ulRFC2217Value = 0;
            pState->pRFC2217Session->ucRFC2217Index = 0;

            //
            // Set the expected number of data bytes based on the
            // command type.
            //
            switch(ucChar)
            {
                case TELNET_C2S_SIGNATURE:
                case TELNET_C2S_FLOWCONTROL_SUSPEND:
                case TELNET_C2S_FLOWCONTROL_RESUME:
                {
                    //
                    // No data is expected, and will be ignored.
                    //
                    pState->pRFC2217Session->ucRFC2217IndexMax = 0;

                    //
                    // This option has been handled.
                    //
                    break;
                }

                case TELNET_C2S_SET_BAUDRATE:
                {
                    //
                    // For baud rate command, we expect 4 bytes of data.
                    //
                    pState->pRFC2217Session->ucRFC2217IndexMax = 4;

                    //
                    // This option has been handled.
                    //
                    break;
                }

                default:
                {
                    //
                    // For other commands, we expect 1 byte of data.
                    //
                    pState->pRFC2217Session->ucRFC2217IndexMax = 1;

                    //
                    // This option has been handled.
                    //
                    break;
                }
            }

            //
            // Prepare to get command data.
            //
            pState->pRFC2217Session->eRFC2217State = STATE_2217_GET_DATA;

            //
            // This state has been handled.
            //
            break;
        }

        //
        // After getting the command, we need to process the data.
        //
        case STATE_2217_GET_DATA:
        {
            //
            // Check if this is the IAC byte.
            //
            if(ucChar == TELNET_IAC)
            {
                //
                //
                pState->pRFC2217Session->eRFC2217State = STATE_2217_GET_DATA_IAC;

                //
                // This state has been handled.
                //
                break;
            }

            //
            // If we are not expecting any data, ignore this byte.
            //
            if(pState->pRFC2217Session->ucRFC2217IndexMax == 0)
            {
                //
                // This state has been handled.
                //
                break;
            }

            //
            // Setup the pointer to the COM-PORT value.
            //
            pucValue = (unsigned char *)&pState->pRFC2217Session->ulRFC2217Value;

            //
            // Save the data, if we still need it.
            // Note: Data arrives in "network" order, but must be stored
            // in "host" order, so we swap byte order as we store it if it
            // is a 4-octet value (e.g. baud rate)
            //
            if(pState->pRFC2217Session->ucRFC2217Index < pState->pRFC2217Session->ucRFC2217IndexMax)
            {
                if(pState->pRFC2217Session->ucRFC2217IndexMax == 4)
                {
                    pucValue[3 - pState->pRFC2217Session->ucRFC2217Index++] = ucChar;
                }
                else
                {
                    pucValue[pState->pRFC2217Session->ucRFC2217Index++] = ucChar;
                }
            }

            //
            // This state has been handled.
            //
            break;
        }

        //
        // After getting the command, we need to process the data.
        //
        case STATE_2217_GET_DATA_IAC:
        {
            //
            // Check if this is the SE byte to end the SB string.
            //
            if(ucChar == TELNET_SE)
            {
                //
                // Process the RFC2217 command.
                //
                TelnetProcessRFC2217Command(conn);

                //
                // Restore telnet state to normal processing.
                //
                pState->eTelnetState = STATE_NORMAL;

                //
                // This state has been handled.
                //
                break;
            }

            //
            // Make sure we go back to regular data processing for
            // the next byte.
            //
            pState->pRFC2217Session->eRFC2217State = STATE_2217_GET_DATA;

            //
            // If we are not expecting any data, ignore this byte.
            //
            if(pState->pRFC2217Session->ucRFC2217IndexMax == 0)
            {
                //
                // This state has been handled.
                //
                break;
            }

            //
            // Setup the pointer to the COM-PORT value.
            //
            pucValue = (unsigned char *)&pState->pRFC2217Session->ulRFC2217Value;

            //
            // Save the data, if we still need it.
            // Note: Data arrives in "network" order, but must be stored
            // in "host" order, so we swap byte order as we store it if it
            // is a 4-octet value (e.g. baud rate)
            //
            if(pState->pRFC2217Session->ucRFC2217Index < pState->pRFC2217Session->ucRFC2217IndexMax)
            {
                if(pState->pRFC2217Session->ucRFC2217IndexMax == 4)
                {
                    pucValue[3 - pState->pRFC2217Session->ucRFC2217Index++] = ucChar;
                }
                else
                {
                    pucValue[pState->pRFC2217Session->ucRFC2217Index++] = ucChar;
                }
            }

            //
            // This state has been handled.
            //
            break;
        }
    }
}

#endif // _SER2ENET_

//*****************************************************************************
//
//! Processes a telnet WILL request.
//!
//! \param ucOption is the telnet option in question.
//! \param pState is the telnet state data for this connection.
//!
//! This function will handle a WILL request for a telnet option.  If it is an
//! option that is known by the telnet server, a DO response will be generated
//! if the option is not already enabled.  For unknown options, a DONT response
//! will always be generated.
//!
//! The response (if any) is written into the telnet transmit buffer.
//!
//! \return None.
//
//*****************************************************************************
static void
TelnetProcessWill(unsigned char ucOption, TL_CONNECTION *conn)
{	
    unsigned char pucBuf[3];
	tTelnetSessionData *pState = &conn->TelnetSession;

    //
    // Check for supported options.
    //
    switch(ucOption)
    {
        //
        // Suppress Go ahead.
        //
        case TELNET_OPT_SUPPRESS_GA:
        {
            //
            // See if the WILL flag for this option is not yet set.
            //
            if((pState->ucFlags & (1 << OPT_FLAG_WILL_SUPPRESS_GA)) == 0)
            {
                //
                // Set the WILL flag for this option.
                //
                pState->ucFlags |= (1 << OPT_FLAG_WILL_SUPPRESS_GA);

                //
                // Send a DO response to this option.
                //
                pucBuf[0] = TELNET_IAC;
                pucBuf[1] = TELNET_DO;
                pucBuf[2] = ucOption;
                //tcp_write(pState->pConnectPCB, pucBuf, 3, 1);
				if (send(conn->sock, pucBuf, 3, 0) < 0)
			  	{
			    	if (sysGetErrno() == EPIPE)
			    	{
			      		tl_destroyConnection(conn);
			    	}
			  	}
            }
            break;
        }

#if(_SER2ENET_)

        //
        // RFC 2217 Com Port Control.
        //
        case TELNET_OPT_RFC2217:
        {

			if(pState->pRFC2217Session)
			{
	            //
	            // See if the WILL flag for this option is not yet set.
	            //
	            if((pState->ucFlags & (1 << OPT_FLAG_WILL_RFC2217)) == 0)
	            {
	                //
	                // Set the WILL flag for this option.
	                //
	                pState->ucFlags |= (1 << OPT_FLAG_WILL_RFC2217);
	
	                //
	                // Send a DO response to this option.
	                //
	                pucBuf[0] = TELNET_IAC;
	                pucBuf[1] = TELNET_DO;
	                pucBuf[2] = ucOption;
	                //tcp_write(pState->pConnectPCB, pucBuf, 3, 1);
					if (send(conn->sock, pucBuf, 3, 0) < 0)
				  	{
				    	if (sysGetErrno() == EPIPE)
				    	{
				      		tl_destroyConnection(conn);
				    	}
				  	}
	            }
	            break;
			}

        }

#endif 

        //
        // This option is not recognized, so send a DONT response.
        //
        default:
        {
            pucBuf[0] = TELNET_IAC;
            if(ucOption == TELNET_OPT_BINARY)
            {
                pucBuf[1] = TELNET_DO;
            }
            else
            {
                pucBuf[1] = TELNET_DONT;
            }
            pucBuf[2] = ucOption;
            //tcp_write(pState->pConnectPCB, pucBuf, 3, 1);
			if (send(conn->sock, pucBuf, 3, 0) < 0)
		  	{
		    	if (sysGetErrno() == EPIPE)
		    	{
		      		tl_destroyConnection(conn);
		    	}
		  	}
            break;
        }
    }
}

//*****************************************************************************
//
//! Processes a telnet WONT request.
//!
//! \param ucOption is the telnet option in question.
//! \param pState is the telnet state data for this connection.
//!
//! This function will handle a WONT request for a telnet option.  If it is an
//! option that is known by the telnet server, a DONT response will be
//! generated if the option is not already disabled.  For unknown options, a
//! DONT response will always be generated.
//!
//! The response (if any) is written into the telnet transmit buffer.
//!
//! \return None.
//
//*****************************************************************************
static void
TelnetProcessWont(unsigned char ucOption, TL_CONNECTION *conn)
{
    unsigned char pucBuf[3];
	tTelnetSessionData *pState = &conn->TelnetSession;
    //
    // Check for supported options.
    //
    switch(ucOption)
    {
        //
        // Suppress Go ahead.
        //
        case TELNET_OPT_SUPPRESS_GA:
        {
            //
            // See if the WILL flag for this option is not yet cleared.
            //
            if((pState->ucFlags & (1 << OPT_FLAG_WILL_SUPPRESS_GA)) > 0)
            {
                //
                // Clear the WILL flag for this option.
                //
                pState->ucFlags &= ~(1 << OPT_FLAG_WILL_SUPPRESS_GA);

                //
                // Send a DONT response to this option.
                //
                pucBuf[0] = TELNET_IAC;
                pucBuf[1] = TELNET_DONT;
                pucBuf[2] = ucOption;
                //tcp_write(pState->pConnectPCB, pucBuf, 3, 1);
				if (send(conn->sock, pucBuf, 3, 0) < 0)
			  	{
			    	if (sysGetErrno() == EPIPE)
			    	{
			      		tl_destroyConnection(conn);
			    	}
			  	}
            }
            break;
        }

#if(_SER2ENET_)

        //
        // RFC 2217 Com Port Control.
        //
		
        case TELNET_OPT_RFC2217:
        {
			if(pState->pRFC2217Session)
			{
	            //
	            // See if the WILL flag for this option is not yet cleared.
	            //
	            if((pState->ucFlags & (1 << OPT_FLAG_WILL_RFC2217)) > 0)
	            {
	                //
	                // Clear the WILL flag for this option.
	                //
	                pState->ucFlags &= ~(1 << OPT_FLAG_WILL_RFC2217);
	
	                //
	                // Send a DONT response to this option.
	                //
	                pucBuf[0] = TELNET_IAC;
	                pucBuf[1] = TELNET_DONT;
	                pucBuf[2] = ucOption;
	                //tcp_write(pState->pConnectPCB, pucBuf, 3, 1);
					if (send(conn->sock, pucBuf, 3, 0) < 0)
				  	{
				    	if (sysGetErrno() == EPIPE)
				    	{
				      		tl_destroyConnection(conn);
				    	}
				  	}
	            }
	            break;
			}
        }

#endif

        //
        // This option is not recognized, so send a DONT response.
        //
        default:
        {
            pucBuf[0] = TELNET_IAC;
            pucBuf[1] = TELNET_DONT;
            pucBuf[2] = ucOption;
            //tcp_write(pState->pConnectPCB, pucBuf, 3, 1);
			if (send(conn->sock, pucBuf, 3, 0) < 0)
		  	{
		    	if (sysGetErrno() == EPIPE)
		    	{
		      		tl_destroyConnection(conn);
		    	}
		  	}
            break;
        }
    }
}

//*****************************************************************************
//
//! Processes a telnet DO request.
//!
//! \param ucOption is the telnet option in question.
//! \param pState is the telnet state data for this connection.
//!
//! This function will handle a DO request for a telnet optoin.  If it is an
//! option that is known by the telnet server, a WILL response will be
//! generated if the option is not already enabled.  For unknown options, a
//! WONT response will always be generated.
//!
//! The response (if any) is written into the telnet transmit buffer.
//!
//! \return None.
//
//*****************************************************************************
static void
TelnetProcessDo(unsigned char ucOption, TL_CONNECTION *conn)
{
    unsigned char pucBuf[3];
	tTelnetSessionData *pState = &conn->TelnetSession;
    //
    // Check for supported options.
    //
    switch(ucOption)
    {
        //
        // Suppress Go ahead.
        //
        case TELNET_OPT_SUPPRESS_GA:
        {
            //
            // See if the DO flag for this option is not yet set.
            //
            if((pState->ucFlags & (1 << OPT_FLAG_DO_SUPPRESS_GA)) == 0)
            {
                //
                // Set the DO flag for this option.
                //
                pState->ucFlags |= (1 << OPT_FLAG_DO_SUPPRESS_GA);

                //
                // Send a WILL response to this option.
                //
                pucBuf[0] = TELNET_IAC;
                pucBuf[1] = TELNET_WILL;
                pucBuf[2] = ucOption;
                //tcp_write(pState->pConnectPCB, pucBuf, 3, 1);
				if (send(conn->sock, pucBuf, 3, 0) < 0)
			  	{
			    	if (sysGetErrno() == EPIPE)
			    	{
			      		tl_destroyConnection(conn);
			    	}
			  	}
            }
            break;
        }

#if(_SER2ENET_)

        //
        // RFC 2217 Com Port Control.
        //
	
        case TELNET_OPT_RFC2217:
        {
			if(pState->pRFC2217Session)
			{
	            //
	            // See if the DO flag for this option is not yet set.
	            //
	            if((pState->ucFlags & (1 << OPT_FLAG_DO_RFC2217)) == 0)
	            {
	                //
	                // Set the DO flag for this option.
	                //
	                pState->ucFlags |= (1 << OPT_FLAG_DO_RFC2217);
	
	                //
	                // Send a WILL response to this option.
	                //
	                pucBuf[0] = TELNET_IAC;
	                pucBuf[1] = TELNET_WILL;
	                pucBuf[2] = ucOption;
	                //tcp_write(pState->pConnectPCB, pucBuf, 3, 1);
					if (send(conn->sock, pucBuf, 3, 0) < 0)
				  	{
				    	if (sysGetErrno() == EPIPE)
				    	{
				      		tl_destroyConnection(conn);
				    	}
				  	}
	            }
	            break;
			}
        }

#endif

        //
        // This option is not recognized, so send a WONT response.
        //
        default:
        {
            pucBuf[0] = TELNET_IAC;
            if(ucOption == TELNET_OPT_BINARY)
            {
                pucBuf[1] = TELNET_WILL;
            }
            else
            {
                pucBuf[1] = TELNET_WONT;
            }
            pucBuf[2] = ucOption;
            //tcp_write(pState->pConnectPCB, pucBuf, 3, 1);
			if (send(conn->sock, pucBuf, 3, 0) < 0)
		  	{
		    	if (sysGetErrno() == EPIPE)
		    	{
		      		tl_destroyConnection(conn);
		    	}
		  	}
            break;
        }
    }
}

//*****************************************************************************
//
//! Processes a telnet DONT request.
//!
//! \param ucOption is the telnet option in question.
//! \param pState is the telnet state data for this connection.
//!
//! This funciton will handle a DONT request for a telnet option.  If it is an
//! option that is known by the telnet server, a WONT response will be
//! generated if the option is not already disabled.  For unknown options, a
//! WONT resopnse will always be generated.
//!
//! The response (if any) is written into the telnet transmit buffer.
//!
//! \return None.
//
//*****************************************************************************
static void
TelnetProcessDont(unsigned char ucOption, TL_CONNECTION *conn)
{
    unsigned char pucBuf[3];
	tTelnetSessionData *pState = &conn->TelnetSession;
    //
    // Check for supported options.
    //
    switch(ucOption)
    {
        //
        // Suppress Go ahead.
        //
        case TELNET_OPT_SUPPRESS_GA:
        {
            //
            // See if the DO flag for this option is not yet cleared.
            //
            if((pState->ucFlags & (1 << OPT_FLAG_DO_SUPPRESS_GA)) > 0)
            {
                //
                // Clear the DO flag for this option.
                //
                pState->ucFlags &= ~(1 << OPT_FLAG_DO_SUPPRESS_GA);

                //
                // Send a WONT response to this option.
                //
                pucBuf[0] = TELNET_IAC;
                pucBuf[1] = TELNET_WONT;
                pucBuf[2] = ucOption;
                //tcp_write(pState->pConnectPCB, pucBuf, 3, 1);
				if (send(conn->sock, pucBuf, 3, 0) < 0)
			  	{
			    	if (sysGetErrno() == EPIPE)
			    	{
			      		tl_destroyConnection(conn);
			    	}
			  	}
            }
            break;
        }

#if(_SER2ENET_)

        //
        // RFC 2217 Com Port Control.
        //

        case TELNET_OPT_RFC2217:
        {
			if(pState->pRFC2217Session)
			{
	            //
	            // See if the DO flag for this option is not yet cleared.
	            //
	            if((pState->ucFlags & (1 << OPT_FLAG_DO_RFC2217)) > 0)
	            {
	                //
	                // Clear the DO flag for this option.
	                //
	                pState->ucFlags &= ~(1 << OPT_FLAG_DO_RFC2217);
	
	                //
	                // Send a WONT response to this option.
	                //
	                pucBuf[0] = TELNET_IAC;
	                pucBuf[1] = TELNET_WONT;
	                pucBuf[2] = ucOption;
	                //tcp_write(pState->pConnectPCB, pucBuf, 3, 1);
					if (send(conn->sock, pucBuf, 3, 0) < 0)
				  	{
				    	if (sysGetErrno() == EPIPE)
				    	{
				      		tl_destroyConnection(conn);
				    	}
				  	}
	            }
	            break;
			}
        }

#endif

        //
        // This option is not recognized, so send a WONT response.
        //
        default:
        {
            pucBuf[0] = TELNET_IAC;
            pucBuf[1] = TELNET_WONT;
            pucBuf[2] = ucOption;
            //tcp_write(pState->pConnectPCB, pucBuf, 3, 1);
			if (send(conn->sock, pucBuf, 3, 0) < 0)
		  	{
		    	if (sysGetErrno() == EPIPE)
		    	{
		      		tl_destroyConnection(conn);
		    	}
		  	}
            break;
        }
    }
}

//*****************************************************************************
//
//! Processes a character received from the telnet port.
//!
//! \param ucChar is the character in question.
//! \param pState is the telnet state data for this connection.
//!
//! This function processes a character received from the telnet port, handling
//! the interpretation of telnet commands (as indicated by the telnet interpret
//! as command (IAC) byte).
//!
//! \return None.
//
//*****************************************************************************
void
TelnetProcessCharacter(unsigned char ucChar, TL_CONNECTION *conn)
{
    unsigned char pucBuf[9];
	tTelnetSessionData *pState = &conn->TelnetSession;

    //
    // Determine the current state of the telnet command parser.
    //
    switch(pState->eTelnetState)
    {
        //
        // The normal state of the parser, were each character is either sent
        // to the UART or is a telnet IAC character.
        //
        case STATE_NORMAL:
        {
            //
            // See if this character is the IAC character.
            //
            if(ucChar == TELNET_IAC)
            {
                //
                // Skip this character and go to the IAC state.
                //
                pState->eTelnetState = STATE_IAC;
            }
            else
            {
            
#if(_SER2ENET_)			
			    //
                // Write this character to the UART.
                //
				if(pState->pRFC2217Session)
                	SerialSend(pState->pRFC2217Session->hSerialPort, ucChar);
				else
#endif
                	form_telnet_cmd(conn, ucChar);
            }

            //
            // This state has been handled.
            //
            break;
        }

        //
        // The previous character was the IAC character.
        //
        case STATE_IAC:
        {
            //
            // Determine how to interpret this character.
            //
            switch(ucChar)
            {
                //
                // See if this character is also an IAC character.
                //
                case TELNET_IAC:
                {
                    //
                    // Send 0xff to the UART.
                    //

#if(_SER2ENET_)

					if(pState->pRFC2217Session)
	                	SerialSend(pState->pRFC2217Session->hSerialPort, ucChar);
					else
#endif
	                	form_telnet_cmd(conn, ucChar);

                    //
                    // Switch back to normal mode.
                    //
                    pState->eTelnetState = STATE_NORMAL;

                    //
                    // This character has been handled.
                    //
                    break;
                }

                //
                // See if this character is the WILL request.
                //
                case TELNET_WILL:
                {
                    //
                    // Switch to the WILL mode; the next character will have
                    // the option in question.
                    //
                    pState->eTelnetState = STATE_WILL;

                    //
                    // This character has been handled.
                    //
                    break;
                }

                //
                // See if this character is the WONT request.
                //
                case TELNET_WONT:
                {
                    //
                    // Switch to the WONT mode; the next character will have
                    // the option in question.
                    //
                    pState->eTelnetState = STATE_WONT;

                    //
                    // This character has been handled.
                    //
                    break;
                }

                //
                // See if this character is the DO request.
                //
                case TELNET_DO:
                {
                    //
                    // Switch to the DO mode; the next character will have the
                    // option in question.
                    //
                    pState->eTelnetState = STATE_DO;

                    //
                    // This character has been handled.
                    //
                    break;
                }

                //
                // See if this character is the DONT request.
                //
                case TELNET_DONT:
                {
                    //
                    // Switch to the DONT mode; the next character will have
                    // the option in question.
                    //
                    pState->eTelnetState = STATE_DONT;

                    //
                    // This character has been handled.
                    //
                    break;
                }

                //
                // See if this character is the AYT request.
                //
                case TELNET_AYT:
                {
                    //
                    // Send a short string back to the client so that it knows
                    // that we're still alive.
                    //
                    pucBuf[0] = '\r';
                    pucBuf[1] = '\n';
                    pucBuf[2] = '[';
                    pucBuf[3] = 'Y';
                    pucBuf[4] = 'e';
                    pucBuf[5] = 's';
                    pucBuf[6] = ']';
                    pucBuf[7] = '\r';
                    pucBuf[8] = '\n';
                   // tcp_write(pState->pConnectPCB, pucBuf, 9, 1);
					if (send(conn->sock, pucBuf, 9, 0) < 0)
				  	{
				    	if (sysGetErrno() == EPIPE)
				    	{
				      		tl_destroyConnection(conn);
				    	}
				  	}

                    //
                    // Switch back to normal mode.
                    //
                    pState->eTelnetState = STATE_NORMAL;

                    //
                    // This character has been handled.
                    //
                    break;
                }

                //
                // See if this is the SB request.
                //
				
                case TELNET_SB:
                {
					if(pState->pRFC2217Session)
					{
	                    //
	                    // Switch to SB processing mode.
	                    //
	                    pState->eTelnetState = STATE_SB;
	
	                    //
	                    // This character has been handled.
	                    //
	                    break;
					}
                }
                //
                // Explicitly ignore the GA and NOP request, plus provide a
                // catch-all ignore for unrecognized requests.
                //
                case TELNET_GA:
                case TELNET_NOP:
                default:
                {
                    //
                    // Switch back to normal mode.
                    //
                    pState->eTelnetState = STATE_NORMAL;

                    //
                    // This character has been handled.
                    //
                    break;
                }
            }

            //
            // This state has been handled.
            //
            break;
        }

        //
        // The previous character sequence was IAC WILL.
        //
        case STATE_WILL:
        {
            //
            // Process the WILL request on this option.
            //
            //DEBUG_PRINTF_ONE("STATE_WILL\n\r", NULL);

            TelnetProcessWill(ucChar, conn);

            //
            // Switch back to normal mode.
            //
            pState->eTelnetState = STATE_NORMAL;

            //
            // This state has been handled.
            //
            break;
        }

        //
        // The previous character sequence was IAC WONT.
        //
        case STATE_WONT:
        {
            //
            // Process the WONT request on this option.
            //
            //DEBUG_PRINTF_ONE("STATE_WONT\n\r", NULL);

            TelnetProcessWont(ucChar, conn);

            //
            // Switch back to normal mode.
            //
            pState->eTelnetState = STATE_NORMAL;

            //
            // This state has been handled.
            //
            break;
        }

        //
        // The previous character sequence was IAC DO.
        //
        case STATE_DO:
        {
            //
            // Process the DO request on this option.
            //
            TelnetProcessDo(ucChar, conn);

            //DEBUG_PRINTF_ONE("STATE_DO\n\r", NULL);

            //
            // Switch back to normal mode.
            //
            pState->eTelnetState = STATE_NORMAL;

            //
            // This state has been handled.
            //
            break;
        }

        //
        // The previous character sequence was IAC DONT.
        //
        case STATE_DONT:
        {
            //
            // Process the DONT request on this option.
            //
            TelnetProcessDont(ucChar, conn);

            //
            // Switch back to normal mode.
            //
            pState->eTelnetState = STATE_NORMAL;

            //
            // This state has been handled.
            //
            break;
        }

        //
        // The previous character sequence was IAC SB.
        //
        case STATE_SB:
        {

#if(_SER2ENET_)

            //
            // If the SB request is COM_PORT request (in other words, RFC
            // 2217).
            //
            if(pState->pRFC2217Session && ((ucChar == TELNET_OPT_RFC2217) &&
                 ((pState->ucFlags & (1 << OPT_FLAG_WILL_RFC2217)) > 0) &&
                 ((pState->ucFlags & (1 << OPT_FLAG_DO_RFC2217)) > 0)))
            {
                //
                // Initialize the COM PORT option state machine.
                //
                pState->pRFC2217Session->eRFC2217State = STATE_2217_GET_COMMAND;

                //
                // Change state to COM PORT option processing state.
                //
                pState->eTelnetState = STATE_SB_RFC2217;
            }
            else
#endif
            {
                //
                // Ignore this SB option.
                //
                pState->eTelnetState = STATE_SB_IGNORE;
            }

            //
            // This state has been handled.
            //
            break;
        }

        //
        // In the middle of an unsupported IAC SB sequence.
        //
        case STATE_SB_IGNORE:
        {
            //
            // Check for the IAC character.
            //
            if(ucChar == TELNET_IAC)
            {
                //
                // Change state to look for Telnet SE character.
                //
                pState->eTelnetState = STATE_SB_IGNORE_IAC;
            }

            //
            // This state has been handled.
            //
            break;
        }

        //
        // In the middle of a an RFC 2217 sequence.
        //

#if(_SER2ENET_)
		
        case STATE_SB_RFC2217:
        {
			if(pState->pRFC2217Session)
			{
	            //
	            // Allow the 2217 processor to handle this character.
	            //

	            TelnetProcessRFC2217Character(ucChar, conn);

	            //
	            // This state has been handled.
	            //
	            break;
			}
        }

#endif

        //
        // Checking for the terminating IAC SE in unsupported IAC SB sequence.
        //
        case STATE_SB_IGNORE_IAC:
        {
            //
            // Check for the IAC character.
            //
            if(ucChar == TELNET_SE)
            {
                //
                // IAC SB sequence is terminated.  Revert to normal telnet
                // character processing.
                //
                pState->eTelnetState = STATE_NORMAL;
            }
            else
            {
                //
                // Go back to looking for the IAC SE sequence.
                //
                pState->eTelnetState = STATE_SB_IGNORE;
            }

            //
            // This state has been handled.
            //
            break;
        }

        //
        // A catch-all for unknown states.  This should never be reached, but
        // is provided just in case it is ever needed.
        //
        default:
        {
            //
            // Switch back to normal mode.
            //
            pState->eTelnetState = STATE_NORMAL;

            //
            // This state has been handled.
            //
            break;
        }
    }
}



void tl_accept_pool(TL_CONNECTION *conn)
{
	tTelnetSessionData *pState = &conn->TelnetSession;

    pState->eTelnetState = STATE_NORMAL;
    pState->ucFlags |= ((1 << OPT_FLAG_WILL_SUPPRESS_GA) |
                       (1 << OPT_FLAG_SERVER));
#if(_SER2ENET_)

	if(pState->pRFC2217Session)
	{
	    pState->ucFlags |= (1 << OPT_FLAG_WILL_RFC2217);
	    pState->pRFC2217Session->ucRFC2217FlowControl = TELNET_C2S_FLOWCONTROL_RESUME;
	    pState->pRFC2217Session->ucRFC2217ModemMask = 0;
	    pState->pRFC2217Session->ucRFC2217LineMask = 0xff;
	    pState->pRFC2217Session->ucLastModemState = 0;
	    pState->pRFC2217Session->ucModemState = 0;
		pState->pRFC2217Session->hSerialPort = OpenDevice(UART_CONTROL_DRIVER_NAME);
		DeviceIOCtl(pState->pRFC2217Session->hSerialPort, SET_BAUDRATE, (uint8_t *)&g_sParameters.sPort.ulBaudRate);
	}
#endif
	if(!pState->pRFC2217Session)
	{
		clr_telnet_auth();
	    shell_start(conn);
	}
}

void tl_pool(TL_CONNECTION *conn)
{
	int i = 0;

	for(i = 0; i < conn->buflen; i++)
    {
		TelnetProcessCharacter(conn->rxbuffer[i], conn);
	}
	conn->buflen = 0;	

}


/*---------------------------------------------------------------------------
 *  CONNECTION HANDLING
 *-------------------------------------------------------------------------*/


/* Add a socket to the global socket set.
 */
static int tl_addSocket(TL_SERVER *serv, const int sock)
{
  	if (sock >= 0)
  	{
    	FD_CLR(sock, &serv->SocketSet);
    	FD_SET(sock, &serv->SocketSet);
    	return 0;
  	}
  	return -1;
}


/* Remove socket from fdsets and close it.
 */
static void tl_delSocket(TL_SERVER *serv, const int sock)
{
  if (sock >= 0)
  {
    FD_CLR(sock, &serv->SocketSet);
    closesocket(sock);
  }
}


/*-------------------------------------------------------------------------*/


/* Insert an element to the tail of a double linked list.
 */
static void tl_listAddTail(lelem_t *headelem, lelem_t *elem)
{
  elem->prev = headelem->prev;
  elem->next = headelem;
  headelem->prev->next = elem;
  headelem->prev = elem;
}


/* Insert an element to the head of a double linked list.
 */
/*static void mb_listAddHead(lelem_t *headelem, lelem_t *elem)
{
  elem->next = headelem->next;
  elem->prev = headelem;
  headelem->next->prev = elem;
  headelem->next = elem;
}	 */


/* Remove an element from a double linked list
 * and free its memory.
 */
static void tl_listDel(lelem_t *elem)
{
  lelem_t *next = elem->next;
  lelem_t *prev = elem->prev;
  next->prev = prev;
  prev->next = next;
}


/*-------------------------------------------------------------------------*/


/* Create and initialize a new connection structure.
 * On success, the connection is established and a 220 status is sent.
 */
static int tl_newConnection(TL_SERVER *serv, int socket)
{
  unsigned long one = 1;
  TL_CONNECTION *conn;

  if(serv->open_connections == serv->conn_limit)
  	return -1;

  conn = (TL_CONNECTION *)(sysMemAlloc(sizeof(TL_CONNECTION)));
  if (conn == NULL)
    return -1;

  conn->sock        = socket;
  conn->buflen      = 0;
  conn->lastTrans   = (xTaskGetTickCount() / 1000);

  if(serv->RFC2217Session)
  {
  	  conn->TelnetSession.pRFC2217Session = (tRFC2217SessionData *)(sysMemAlloc(sizeof(tRFC2217SessionData)));
  }
  else conn->TelnetSession.pRFC2217Session = 0;

  /* set nonblocking mode */
  ioctlsocket(socket, FIONBIO, (void*)&one);

  /* add socket and connection to list of active connections */
  tl_addSocket(serv, socket);
  tl_listAddTail(&serv->connListRoot, (lelem_t*)(void*)conn);
  serv->open_connections++;
  serv->tl_accept_func(conn);
  return 0;
}




/* Destroys a ctrl connection and
 * cleans up the connection structure.
 */
static void tl_destroyConnection(TL_CONNECTION *conn)
{

  if (conn == NULL)
    return;

  TL_SERVER *serv = get_tl_serv(conn);
  if ((conn->sock >= 0) && (serv->open_connections > 0))
  {
    serv->open_connections--;
  }

  tl_delSocket(serv, conn->sock);
  tl_listDel((lelem_t*)(void*)conn);
  if(conn->TelnetSession.pRFC2217Session)
  {
  	sysMemFree(conn->TelnetSession.pRFC2217Session);
  }
  sysMemFree(conn);
}

/*-------------------------------------------------------------------------*/


/* This function processes all open control connections.
 * If data was received over the connection, it is
 * passed to the command line parser.
 */

static int tl_processConnections(TL_SERVER *serv, fd_set *clientset, int fdcount)
{
  TL_CONNECTION *conn, *next;
  int checked;
  int   bytes;

  next = FIRST_LIST_ELEM(&serv->connListRoot, TL_CONNECTION*);
  checked = 0;

  /* process all connections in the list */
  while (!END_OF_LIST(&serv->connListRoot, next) && (checked < fdcount))
  {
    conn = next;
    next = NEXT_LIST_ELEM(conn, TL_CONNECTION*);

    if (!FD_ISSET(conn->sock, clientset))
    {
      continue;
    }

    checked++;

    bytes = recv(conn->sock, conn->rxbuffer + conn->buflen, (TL_INPBUFSIZE-1) - conn->buflen, 0);

    if (bytes <= 0)
    {
      /* If no bytes are available or -1 is returned,
       * the client must have closed the connection since
       * select told us that there was something with the socket.
       */
      tl_destroyConnection(conn);
      continue;
    }

    /* safety check: buffer overrun (should never happen) */
    if ( (conn->buflen + (u16_t) bytes) >= (TL_INPBUFSIZE-1))
    {
      tl_destroyConnection(conn);
      continue;
    }
    conn->buflen += bytes;
    conn->lastTrans = (xTaskGetTickCount() / 1000);

    serv->tl_pool_func(conn);  /////////////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  }
  return checked;
}




/* Accept and open a new connection to the next client.
 */
static void tl_acceptClient(TL_SERVER *serv)
{
  struct sockaddr_in addr;
  int    s;
  static int errors = 0;
  u32_t  alen;


  alen = sizeof(addr);
  sysMemSet(&addr, 0, sizeof(addr));
  s = accept(serv->serverSocket, (struct sockaddr *)&addr, &alen);

  if (s < 0)
  {

    if ((++errors >= 3))
    {
      /* try to recover server socket on too much errors */
//      mb_delSocket(serverSocket_g);
//      serverSocket_g = mb_newServerSocket();
	  PUT_MES_TO_LOG("MB_TCP: NO RESOURCE FOR INCOMMING CON", 0, 0);
	  PUT_MES_TO_LOG("MB_TCP: DEL ALL CLIENTS", 0, 0);
	  tl_fdSanity(serv);
      errors = 0;
    }
  }
  else
  {
    errors = 0;

    if (tl_newConnection(serv, s) != 0)
    {
      closesocket(s);
    }
  }
}


/*-------------------------------------------------------------------------*/


/* Check for timed out connections,
 * and close them.
 */
static void tl_timeout(TL_SERVER *serv, uint32_t cur_t)
{
  TL_CONNECTION *conn, *next;

  /* run through the list of connections */
  next = FIRST_LIST_ELEM(&serv->connListRoot, TL_CONNECTION*);
  while (!END_OF_LIST(&serv->connListRoot, next))
  {
    conn = next;
    next = NEXT_LIST_ELEM(conn, TL_CONNECTION*);

    if ((cur_t - conn->lastTrans) > TL_TIMEOUT)
    {
      tl_destroyConnection(conn);
    }
  }
}


/*-------------------------------------------------------------------------*/


/* create a new server socket that listens to incomming FTP requests.
 */
static int tl_newServerSocket(TL_SERVER *serv)
{
  unsigned long one = 1;
  struct sockaddr_in addr;
  int s, err;
  
  s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (s < 0)
    return -1;

  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void*) &one, sizeof(one));
  ioctlsocket(s, FIONBIO, (void*) &one);    /* set nonblocking mode */

  sysMemSet(&addr, 0, sizeof(addr));
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = 0;///*0x0200A8C0;*/ip_adr.addr;
  addr.sin_port        = htons(serv->port);	                         //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  err = bind(s, (struct sockaddr *)&addr, sizeof(struct sockaddr));
  if (err < 0)
  {
    if ((sysGetErrno() == ENOMEM) || (sysGetErrno() == EADDRINUSE))
    {
      /* wait some time and try again...*/
      sys_msleep(500);
      err = bind(s, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    }
    if (err < 0)
    {
      closesocket(s);
      //sysPrintErr("Failed to bind server socket!\n");
      return -1;
    }
  }

  listen(s, 20);

  tl_addSocket(serv, s);

  return s;
}


/*-------------------------------------------------------------------------*/


/* This function tries to remove all faulty
 * file handles from the file sets.
 */
static int tl_fdSanity(TL_SERVER *serv)
{
  struct timeval tv = {0,0};
  TL_CONNECTION   *conn, *nextc;
  fd_set         fds;
//  char           buf;

  /* Test if the server socket has failed.
     If so, destroy the socket and create a new one. */
  FD_ZERO(&fds);
  FD_SET(serv->serverSocket, &fds); 
  if (select(serv->serverSocket, &fds, NULL, NULL, &tv) < 0)
  {
    FD_CLR(serv->serverSocket, &serv->SocketSet);
    closesocket(serv->serverSocket);
    serv->serverSocket = tl_newServerSocket(serv);
    if (serv->serverSocket < 0)
      return -1;
  }
 
  /* close all faulty connections */
  nextc = FIRST_LIST_ELEM(&serv->connListRoot, TL_CONNECTION*);
  while (!END_OF_LIST(&serv->connListRoot, nextc))
  {
    conn = nextc;
    nextc = NEXT_LIST_ELEM(conn, TL_CONNECTION*);
//
//    if ((recv(conn->sock, &buf, 0, 0) < 0) &&
//        (sysGetErrno() == EBADF))
//    {
      tl_destroyConnection(conn);
//    }
  }

  return 0;
}


#if(_TELNET_)
xQueueHandle telnet_queue;
/* main function. does all initialization.
 */
void TelnetTask(void *pvParameters)
{
  int          i;
  struct timeval timeout;
  volatile uint32_t  cur_t;
  fd_set         fds;
  TL_SERVER		 tl_serv;

  add_tl_serv_to_list(&tl_serv);
  tl_serv.port = 23;
  FD_ZERO(&tl_serv.SocketSet);
  tl_serv.open_connections = 0;
  tl_serv.RFC2217Session = 0;
  tl_serv.conn_limit = 5;
  tl_serv.tl_pool_func = tl_pool;
  tl_serv.tl_accept_func = tl_accept_pool;

  telnet_queue = xQueueCreate(5, sizeof(MODEM_MESSAGE));

  do
  {
	tl_serv.serverSocket = tl_newServerSocket(&tl_serv);
	delay(1000);
  }while(tl_serv.serverSocket < 0);


  ///* init lists 
  INIT_LIST_HEAD(&tl_serv.connListRoot);					 


  timeout.tv_sec  = 2;
  timeout.tv_usec = 100000;

  for(;;)
  {
  	vTaskDelay(10);
    ///* load fds 
    fds = tl_serv.SocketSet;

    i = select(FD_SETSIZE, &fds, NULL, NULL, &timeout);

    if (i < 0)
    {
      if (sysGetErrno() == EBADF)
      {
       // /* test for insane file descriptors 
        tl_fdSanity(&tl_serv);
      }
      else
      if (sysGetErrno() != EINTR)
      {
       // //sysPrintErr("select() failed\n");
      }
      continue;
    }

	cur_t = (uint32_t)(xTaskGetTickCount() / 1000);
    tl_timeout(&tl_serv, cur_t);

    if (i > 0)
    {

      ///* ... handle all connections
      tl_processConnections(&tl_serv, &fds, i);

     // /* check for incomming connections 
      if (FD_ISSET(tl_serv.serverSocket, &fds))
      {
        tl_acceptClient(&tl_serv);
        if (tl_serv.serverSocket< 0)
          tl_fdSanity(&tl_serv);
        i--;
      }
    }
  }
}



//*****************************************************************************//
//**********************TELNET FUNCTION FOR SHELL******************************//
//*****************************************************************************//

#include "string.h"

// символ новой строки
#define ISO_nl       0x0a
// символ возврата каретки
#define ISO_cr       0x0d
// символ backspace
#define ISO_bs       0x08

void 
shell_input(TL_CONNECTION *conn, char *cmd, uint16_t size);

void
shell_output(TL_CONNECTION *conn, char *str1, char *str2)
{
    //tTelnetSessionData *pState = &g_sTelnetSession;

//    tcp_write(pState->pConnectPCB, str1, strlen(str1), 1);
//	tcp_output(pState->pConnectPCB);
	if (send(conn->sock, str1, strlen(str1), 0) < 0)
  	{
    	if (sysGetErrno() == EPIPE)
    	{
      		tl_destroyConnection(conn);
    	}
  	}
   // tcp_write(pState->pConnectPCB, str2, strlen(str2), 1);
//	tcp_output(pState->pConnectPCB);
	if (send(conn->sock, str2, strlen(str2), 0) < 0)
  	{
    	if (sysGetErrno() == EPIPE)
    	{
      		tl_destroyConnection(conn);
    	}
  	}
}

void
shell_prompt(TL_CONNECTION *conn)
{
   // tTelnetSessionData *pState = &g_sTelnetSession;

    //tcp_write(pState->pConnectPCB, SHELL_PROMPT, strlen(SHELL_PROMPT), 1);
//	tcp_output(pState->pConnectPCB);
	if (send(conn->sock, SHELL_PROMPT, strlen(SHELL_PROMPT), 0) < 0)
  	{
    	if (sysGetErrno() == EPIPE)
    	{
      		tl_destroyConnection(conn);
    	}
  	}
}

static void
form_telnet_cmd(TL_CONNECTION *conn, uint8_t c)
{
  if(c == ISO_cr) 
  {
    return;
  }

  if(c == ISO_bs) 
  {
    if(s.bufptr > 0)
        s.bufptr--;
    return;
  }
  
  s.buf[(int)s.bufptr] = c;
  if(s.buf[(int)s.bufptr] == ISO_nl || s.bufptr == sizeof(s.buf) - 1) 
  {
    if(s.bufptr > 0) 
    {
      s.buf[(int)s.bufptr] = 0;
    }
    shell_input(conn, s.buf, sizeof(s.buf));
    s.bufptr = 0;
  } 
  else 
  {
    ++s.bufptr;
  }
}

#endif  // _TELNET_


#if (_SER2ENET_)

static tConfigParameters g_sParametersFactory =
{
    {
        19200, // The baud rate (ulBaudRate).
        8,	 // The number of data bits.
        SERIAL_PARITY_NONE,	 // The parity (NONE).
        1,	  // The number of stop bits.
        0,	// BREAK state off.
        SERIAL_FLOW_CONTROL_NONE, // The flow control (NONE).
        0, // The telnet session timeout (ulTelnetTimeout).
        24,	// The telnet session listen or local port number
        0,	 // Flags indicating the operating mode for this port.
		0,
    }
};

static void s2e_over_tcp_redir(void);

#define S2E_OUTPUT_BUFFER_SIZE		500

void ResetWDT(void);


void s2e_pool(TL_CONNECTION *conn)
{
	int i = 0;
	tTelnetSessionData *pState = &conn->TelnetSession;
	for(i = 0; i < conn->buflen; i++)
    {
		TelnetProcessCharacter(conn->rxbuffer[i], conn);
	}
	conn->buflen = 0;

	unsigned char *pucTemp = pvPortMalloc(S2E_OUTPUT_BUFFER_SIZE);
	if(pucTemp == 0) return;
	
	int count = SerialReceive(pState->pRFC2217Session->hSerialPort, pucTemp, (uint16_t)S2E_OUTPUT_BUFFER_SIZE);

	if(count == 0){
		vPortFree(pucTemp);
		return;
	}	

	if (send(conn->sock, pucTemp, count, 0) < 0)
  	{
    	if (sysGetErrno() == EPIPE)
    	{
      		tl_destroyConnection(conn);
    	}
  	}
	
	vPortFree(pucTemp);	

}


void S2EHandlerTask(void *pvParameters)
{
  int          i;
  struct timeval timeout;
  volatile uint32_t  cur_t;
  fd_set         fds;
  TL_SERVER		 s2e_serv;

  g_sParameters = g_sParametersFactory;

//  DeviceIOCtl(pState->pRFC2217Session->hSerialPort, SET_BAUDRATE, (uint8_t *)&g_sParameters.sPort.ulBaudRate);

  if(g_sParameters.sPort.ucRFCflag == 0)   //   Если нужно только пересылать двнные
		s2e_over_tcp_redir(); 

  //unsigned char *pucTemp = pvPortMalloc(S2E_OUTPUT_BUFFER_SIZE);

  add_tl_serv_to_list(&s2e_serv);
  s2e_serv.port = g_sParameters.sPort.usTelnetLocalPort;
  FD_ZERO(&s2e_serv.SocketSet);
  s2e_serv.open_connections = 0;
  s2e_serv.RFC2217Session = 1;
  s2e_serv.conn_limit = 1;
  s2e_serv.tl_pool_func = s2e_pool;
  s2e_serv.tl_accept_func = tl_accept_pool;


  do
  {
	s2e_serv.serverSocket = tl_newServerSocket(&s2e_serv);
	delay(1000);
  }while(s2e_serv.serverSocket < 0);


  ///* init lists 
  INIT_LIST_HEAD(&s2e_serv.connListRoot);					 


  timeout.tv_sec  = 2;
  timeout.tv_usec = 100000;

  for(;;)
  {
  	vTaskDelay(10);
    ///* load fds 
    fds = s2e_serv.SocketSet;

    i = select(FD_SETSIZE, &fds, NULL, NULL, &timeout);

    if (i < 0)
    {
      if (sysGetErrno() == EBADF)
      {
       // /* test for insane file descriptors 
        tl_fdSanity(&s2e_serv);
      }
      else
      if (sysGetErrno() != EINTR)
      {
       // //sysPrintErr("select() failed\n");
      }
      continue;
    }

	cur_t = (uint32_t)(xTaskGetTickCount() / 1000);
    tl_timeout(&s2e_serv, cur_t);

    if (i > 0)
    {

      ///* ... handle all connections
      tl_processConnections(&s2e_serv, &fds, i);

     // /* check for incomming connections 
      if (FD_ISSET(s2e_serv.serverSocket, &fds))
      {
        tl_acceptClient(&s2e_serv);
        if (s2e_serv.serverSocket< 0)
          tl_fdSanity(&s2e_serv);
        i--;
      }
    }
  }
}

void S2E_Init(void)
{
	uint8_t *p_data;
	if(GetSettingsFileStatus())
    {
		p_data = (uint8_t *)GetPointToElement("USER_UART", "Port");
		if(p_data == 0 || strlen((char *)p_data) > 5 || strlen((char *)p_data) < 1)
        {
             g_sParametersFactory.sPort.usTelnetLocalPort = 24;//GetNumericPort("505");
        }
		else g_sParametersFactory.sPort.usTelnetLocalPort = atoi((char *)p_data);


		p_data = (uint8_t *)GetPointToElement("USER_UART", "BaudRate");
		if(p_data == 0 || strlen((char *)p_data) > 6 || strlen((char *)p_data) < 1)
        {
             g_sParametersFactory.sPort.ulBaudRate = 19200;
        }
		else g_sParametersFactory.sPort.ulBaudRate = atoi((char *)p_data);

		p_data = (uint8_t *)GetPointToElement("USER_UART", "RFC");
		if(p_data == 0 || strlen((char *)p_data) > 2 || strlen((char *)p_data) < 1)
        {
             g_sParametersFactory.sPort.ucRFCflag = 0;
        }
		else g_sParametersFactory.sPort.ucRFCflag = atoi((char *)p_data);
	}	
}

int ccc = 0;
static void s2e_over_tcp_redir(void)
{
	return;
//	uint8_t flag = 0;
//	int count = 0;
//	uint32_t delay_timeout = 10;
//	DEVICE_HANDLE hTCP = OpenDevice("TCP_RD_2");
//	DEVICE_HANDLE hUART = OpenDevice(_SER2ENET_DRIVER_);
//	DeviceIOCtl(hTCP, TCP_REDIR_SET_PORT, (uint8_t *)&g_sParameters.sPort.usTelnetLocalPort);
//	DeviceIOCtl(hTCP, TCP_REDIR_LISTEN, 0);
//	DeviceIOCtl(hUART, SET_BAUDRATE, (uint8_t *)&g_sParameters.sPort.ulBaudRate);
//	DeviceIOCtl(hUART, SET_RX_TIMEOUT, (uint8_t *)&delay_timeout);
//	unsigned char *pucTemp = pvPortMalloc(S2E_OUTPUT_BUFFER_SIZE);

//	for(;;)
//	{
//		DeviceIOCtl(hTCP, DEVICE_VALID_CHECK, (void *)&flag);
//		if(flag)
//		{
//			count = 0;
//			count = ReadDevice(hTCP, pucTemp, S2E_OUTPUT_BUFFER_SIZE);
//			if(count > 0)
//			{
//				WriteDevice(hUART, pucTemp, count);	
//			}
//			count = 0;
//			count = ReadDevice(hUART, pucTemp, S2E_OUTPUT_BUFFER_SIZE);
//			if(count > 0)
//			{
//				WriteDevice(hTCP, pucTemp, count);
//				DeviceIOCtl(hTCP, POOL, 0);
//				ccc++;	
//			}
//			delay(5);	 	
//		}
//		else
//			delay(100);
//	}
} 

#endif

//*****************************************************************************//
//*****************************************************************************//
//*****************************************************************************//

#endif // _TELNET_ || _SER2ENET_
