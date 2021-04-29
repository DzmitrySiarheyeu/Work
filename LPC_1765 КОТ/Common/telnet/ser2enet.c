//*****************************************************************************
//
// ser2enet.c - Serial to Ethernet converter.
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

#if (_SER2ENET_)

#include <string.h>
#include "main.h"
#include "ser2enet.h"
#include "serial.h"
#include "telnet.h"
#include "SetParametrs.h"
#include "tcp_redirector.h"

#include "FreeRTOS.h"
#include "task.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Serial To Ethernet Module (ser2enet)</h1>
//!
//! The Serial to Ethernet Converter provides a means of accessing the UART on
//! a Stellaris device via a network connection.  The UART can be connected to
//! the UART on a non-networked device, providing the ability to access the
//! device via a network.  This can be useful to overcome the cable length
//! limitations of a UART connection (in fact, the cable can become thousands
//! of miles long) and to provide networking capability to existing devices
//! without modifying the device's operation.
//!
//! The converter can be configured to use a static IP configuration or to use
//! DHCP to obtain its IP configuration.  Since the converter is providing a
//! telnet server, the effective use of DHCP requires a reservation in the DHCP
//! server so that the converter gets the same IP address each time it is
//! connected to the network.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup main_api
//! @{
//
//*****************************************************************************

////*****************************************************************************
////
////! This structure instance contains the factory-default set of configuration
////! parameters for S2E module.
////
////*****************************************************************************
static tConfigParameters g_sParametersFactory =
{
    {

        // The baud rate (ulBaudRate).
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
        24,

        //
        // Flags indicating the operating mode for this port.
        //
        0,

		0,
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
static void s2e_over_tcp_redir(void);

#define S2E_OUTPUT_BUFFER_SIZE		500

void ResetWDT(void);


void s2e_pool(TL_CONNECTION *conn)
{
	int i = 0;

	for(i = 0; i < conn->buflen; i++)
    {
		TelnetProcessCharacter(conn->rxbuffer[i], conn);
	}
	conn->buflen = 0;

	unsigned char *pucTemp = pvPortMalloc(S2E_OUTPUT_BUFFER_SIZE);
	if(pucTemp == 0) return;
	
	int count = SerialReceive(conn->TelnetSession->pRFC2217Session->hSerialPort, pucTemp, (uint16_t)S2E_OUTPUT_BUFFER_SIZE);
	
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

  tConfigParameters g_sParameters = g_sParametersFactory;

  DeviceIOCtl(pState->pRFC2217Session->hSerialPort, SET_BAUDRATE, (uint8_t *)&g_sParameters.sPort.ulBaudRate);

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
  s2e_serv.tl_accept_func = s2e_accept_pool;


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
        if (tl_serv.serverSocket< 0)
          tl_fdSanity(&s2e_serv);
        i--;
      }
    }
  }
}


void
S2EHandlerTask(void *pvParameters)
{
	sys_prot_t lev;
	long lCount, lIndex, cur_count;
    unsigned char *pucData;
//	int err = 0;

    tTelnetSessionData *pState = &g_sS2EtSession;
	pState->pRFC2217Session = &g_tRFC2217Session;
	g_sParameters = g_sParametersFactory;

	// Установка скорости порта
	DeviceIOCtl(pState->pRFC2217Session->hSerialPort, SET_BAUDRATE, (uint8_t *)&g_sParameters.sPort.ulBaudRate);

	if(g_sParameters.sPort.ucRFCflag == 0)   //   Если нужно только пересылать двнные
		s2e_over_tcp_redir(); 

	unsigned char *pucTemp = pvPortMalloc(S2E_OUTPUT_BUFFER_SIZE);
	if(pucTemp == 0)
		goto S2E_fail;

	//
    // Initialize the telnet module.
    //
    TelnetInit(pState);

    //
    // start listening on the required port.
    //
    TelnetListen(pState, g_sParameters.sPort.usTelnetLocalPort);

    for(;;)
    {
        //
        // If the telnet session is not connected, skip this port.
        //
        if(pState->eTCPState != STATE_TCP_CONNECTED)
        {
			vTaskDelay(100);
            continue;
        }

        //
        // While space is available in the serial output queue, process the
        // pbufs received on the telnet interface.
        //
       while(1)
       {
            //
            // Pop a pbuf off of the rx queue, if one is available, and we are
            // not already processing a pbuf.
            //
            if(pState->pBufHead == NULL)
            {
                if(pState->iBufQRead != pState->iBufQWrite)
                {
                    SYS_ARCH_PROTECT(lev);
                    pState->pBufHead = pState->pBufQ[pState->iBufQRead];
                    pState->iBufQRead =
                        ((pState->iBufQRead + 1) % PBUF_POOL_SIZE);
                    pState->pBufCurrent = pState->pBufHead;
                    pState->ulBufIndex = 0;
                    SYS_ARCH_UNPROTECT(lev);
                }
            }

            //
            // If there is no packet to be processed, break out of the loop.
            //
            if(pState->pBufHead == NULL)
            {
                break;
            }

            //
            // Setup the data pointer for the current buffer.
            //
            pucData = pState->pBufCurrent->payload;

            //
            // Process the next character in the buffer.
            //
            TelnetProcessCharacter(pucData[pState->ulBufIndex], pState);

            //
            // Increment to next data byte.
            //
            pState->ulBufIndex++;

            //
            // Check to see if we are at the end of the current buffer.  If so,
            // get the next pbuf in the chain.
            //
            if(pState->ulBufIndex >= pState->pBufCurrent->len)
            {
                pState->pBufCurrent = pState->pBufCurrent->next;
                pState->ulBufIndex = 0;
            }

            //
            // Check to see if we are at the end of the chain.  If so,
            // acknowledge this data as being consumed to open up the TCP
            // window.
            //
            if((pState->pBufCurrent == NULL) && (pState->ulBufIndex == 0))
            {
                tcp_recved(pState->pConnectPCB, pState->pBufHead->tot_len);
                pbuf_free(pState->pBufHead);
                pState->pBufHead = NULL;
                pState->pBufCurrent = NULL;
                pState->ulBufIndex = 0;
            }
			ResetWDT();
        }

        //
        // Flush the TCP output buffer, in the event that data was
        // queued up by processing the incoming packet.
        //
       // tcp_output(pState->pConnectPCB);

        //
        // Process the RX ring buffer data if space is available in the
        // TCP output buffer.
        //
        if(SerialReceiveAvailable(pState->pRFC2217Session->hSerialPort) && tcp_sndbuf(pState->pConnectPCB) && (pState->pConnectPCB->snd_queuelen < TCP_SND_QUEUELEN))
        {
            //
            // Here, we have data, and we have space.  Set the total amount
            // of data we will process to the lesser of data available or
            // space available.
            //
            lCount = (long)SerialReceiveAvailable(pState->pRFC2217Session->hSerialPort);
            if(tcp_sndbuf(pState->pConnectPCB) < lCount)
            {
                lCount = tcp_sndbuf(pState->pConnectPCB);
            }

            //
            // While we have data remaining to process, continue in this
            // loop.
            //


            while((lCount > 0) &&
                  (tcp_sndqueuelen(pState->pConnectPCB) < TCP_SND_QUEUELEN))
            {
                //
                // First, reset the index into the local buffer.
                //
                lIndex = 0;

                //
                // Fill the local buffer with data while there is data
                // and/or space remaining.
                //
				if(lCount > S2E_OUTPUT_BUFFER_SIZE)
					 cur_count = S2E_OUTPUT_BUFFER_SIZE;
				else cur_count = lCount;

				lIndex = SerialReceive(pState->pRFC2217Session->hSerialPort, pucTemp, (uint16_t)cur_count);
				lCount -= lIndex;
//                while(lCount && (lIndex < S2E_OUTPUT_BUFFER_SIZE))
//                {
//                    pucTemp[lIndex] = SerialReceive(pState->pRFC2217Session->hSerialPort);
//                    lIndex++;
//                    lCount--;
//                }

                tcp_write(pState->pConnectPCB, pucTemp, lIndex, 3);
				vTaskDelay(50);
			//	tcp_output(pState->pConnectPCB);

            }

            //
            // Flush the data that has been written into the TCP output
            // buffer.
            //
            //tcp_output(pState->pConnectPCB);
        }
	//	while(1);
    	vTaskDelay(50);
    }

S2E_fail:

	while(1)
	{
		vTaskDelay(1000);
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
//	while(1)
//	{
//		delay(1000);
//	}
	uint8_t flag = 0;
	int count = 0;
	uint32_t delay_timeout = 10;
	DEVICE_HANDLE hTCP = OpenDevice("TCP_RD_2");
	DEVICE_HANDLE hUART = OpenDevice(_SER2ENET_DRIVER_);
	DeviceIOCtl(hTCP, TCP_REDIR_SET_PORT, (uint8_t *)&g_sParameters.sPort.usTelnetLocalPort);
	DeviceIOCtl(hTCP, TCP_REDIR_LISTEN, 0);
	DeviceIOCtl(hUART, SET_BAUDRATE, (uint8_t *)&g_sParameters.sPort.ulBaudRate);
	DeviceIOCtl(hUART, SET_RX_TIMEOUT, (uint8_t *)&delay_timeout);
	unsigned char *pucTemp = pvPortMalloc(S2E_OUTPUT_BUFFER_SIZE);

	for(;;)
	{
		DeviceIOCtl(hTCP, DEVICE_VALID_CHECK, (void *)&flag);
		if(flag)
		{
			count = 0;
			count = ReadDevice(hTCP, pucTemp, S2E_OUTPUT_BUFFER_SIZE);
			if(count > 0)
			{
				WriteDevice(hUART, pucTemp, count);	
			}
			count = 0;
			count = ReadDevice(hUART, pucTemp, S2E_OUTPUT_BUFFER_SIZE);
			if(count > 0)
			{
				WriteDevice(hTCP, pucTemp, count);
				DeviceIOCtl(hTCP, POOL, 0);
				ccc++;	
			}
			delay(5);	 	
		}
		else
			delay(100);
	}
} 

#endif // _SER2ENET_
