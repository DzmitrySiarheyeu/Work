/**
 * @file
 * Ping sender module
 *
 */

/*
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 */

/** 
 * This is an example of a "ping" sender (with raw API and socket API).
 * It can be used as a start point to maintain opened a network connection, or
 * like a network "watchdog" for your device.
 *
 */
#include "Config.h"
#if(_PING_)

#include <string.h>

#include "lwip/opt.h"
#include "lwip/stats.h"
#if LWIP_RAW && LWIP_ICMP /* don't build if not configured for use in lwipopts.h */

#include "lwip/mem.h"
#include "lwip/api.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/inet_chksum.h"
#include "ping.h"

#include "SetParametrs.h"
#include "main.h"
#include "log.h"


/**
 * PING_DEBUG: Enable debugging for PING.
 */


#ifndef PING_DEBUG
#define PING_DEBUG     LWIP_DBG_ON
#endif

/** ping target - should be a "struct ip_addr" */
#ifndef PING_TARGET
#define PING_TARGET   (netif_default?netif_default->gw:ip_addr_any)
#endif



/** ping receive timeout - in milliseconds */
#ifndef PING_RCV_TIMEO
#define PING_RCV_TIMEO 3000
#endif

/** ping delay - in milliseconds */
#ifndef PING_DELAY
#define PING_DELAY     20000
#endif

/** ping identifier - must fit on a u16_t */
#ifndef PING_ID
#define PING_ID        0xAFAF
#endif

/** ping additional data size to include in the packet */
#ifndef PING_DATA_SIZE
#define PING_DATA_SIZE 32
#endif

/** ping result action - no default action */
#ifndef PING_RESULT
#define PING_RESULT(ping_ok)
#endif

/* ping variables */
static u16_t ping_seq_num;
//static u32_t ping_time;

#if NO_SYS
/* port-defined functions used for timer execution */
void sys_msleep(u32_t ms);
u32_t sys_now();
#endif /* NO_SYS */


/** Prepare a echo ICMP request */
static void
ping_prepare_echo( struct icmp_echo_hdr *iecho, u16_t len)
{
  int i;

  ICMPH_TYPE_SET(iecho,ICMP_ECHO);
  ICMPH_CODE_SET(iecho, 0);
  iecho->chksum = 0;
  iecho->id     = PING_ID;
  iecho->seqno  = htons(++ping_seq_num);

  /* fill the additional data buffer with some data */
  for(i = 0; i < PING_DATA_SIZE; i++) {
    ((char*)iecho)[sizeof(struct icmp_echo_hdr) + i] = i+30;
  }
  iecho->chksum = inet_chksum(iecho, len);
}

#if LWIP_SOCKET

/* Ping using the socket ip */
static err_t
ping_send(int s, struct ip_addr *addr)
{
  int err;
  struct icmp_echo_hdr *iecho;
  struct sockaddr_in to;
  size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;
  iecho = mem_malloc(ping_size);
  if (!iecho) {
    return ERR_MEM;
  }

  ping_prepare_echo(iecho, ping_size);

  to.sin_len = sizeof(to);
  to.sin_family = AF_INET;
  to.sin_addr.s_addr = addr->addr;

  err = lwip_sendto(s, iecho, ping_size, 0, (struct sockaddr*)&to, sizeof(to));

  mem_free(iecho);

  return (err ? ERR_OK : ERR_VAL);
}

int 
ping_recv(int s)
{
  char buf[64];
  int fromlen, len;
  struct sockaddr_in from;
  struct ip_hdr *iphdr;
  struct icmp_echo_hdr *iecho;

  while((len = lwip_recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*)&from, (socklen_t*)&fromlen)) > 0) {
    if (len >= (sizeof(struct ip_hdr)+sizeof(struct icmp_echo_hdr))) {
      LWIP_DEBUGF( PING_DEBUG, ("ping: recv "));
      ip_addr_debug_print(PING_DEBUG, (struct ip_addr *)&(from.sin_addr));
      //LWIP_DEBUGF( PING_DEBUG, (" %lu ms\n", (sys_now()-ping_time)));
      LWIP_DEBUGF( PING_DEBUG, ("PING_RECIVE   OK!!!!!!!!!!!!!!!\n"));
      iphdr = (struct ip_hdr *)buf;
      iecho = (struct icmp_echo_hdr *)(buf+(IPH_HL(iphdr) * 4));
      if ((iecho->id == PING_ID) && (iecho->seqno == htons(ping_seq_num))) {
        /* do some ping result processing */
        LWIP_DEBUGF( PING_DEBUG, ("ping: OK!!!!!!!!!!!!!!!\n"));
       // stats_display();
        PING_RESULT((ICMPH_TYPE(iecho) == ICMP_ER));
        return len;
      } else {
        LWIP_DEBUGF( PING_DEBUG, ("ping: drop\n"));
      }
    }
  }

  if (len == 0) {
   // LWIP_DEBUGF( PING_DEBUG, ("ping: recv - %lu ms - timeout\n", (sys_now()-ping_time)));
   LWIP_DEBUGF( PING_DEBUG, ("PING = 0!!!!!!!!!!!!!\n"));
   return 0;
  }

  /* do some ping result processing */
  PING_RESULT(0);
  return 0;
}
/*
static void
ping_thread(void *arg)
{
  int s;
  int count = 0;
  int timeout = PING_RCV_TIMEO;
  struct ip_addr ping_target;

  LWIP_UNUSED_ARG(arg);

  if ((s = lwip_socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP)) < 0) {
    return;
  }

  lwip_setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

  ping_target.addr = 0x41857cb2;//PING_TARGET;

  while (1) {
  	//ping_target = PING_TARGET;
    if (ping_send(s, &ping_target) == ERR_OK) {
      LWIP_DEBUGF( PING_DEBUG, ("ping: send "));
      ip_addr_debug_print(PING_DEBUG, &ping_target);
      LWIP_DEBUGF( PING_DEBUG, ("\n"));

      if(ping_recv(s) == 0) count++;
	  else
	  {
	  	  ping_ok = 1;
		  count = 0;
	  }
    } else {
      LWIP_DEBUGF( PING_DEBUG, ("ping: send "));
      ip_addr_debug_print(PING_DEBUG, &ping_target);
      LWIP_DEBUGF( PING_DEBUG, (" - error\n"));
	  count++;
    }
    sys_msleep(PING_DELAY);

	if(count > 5) ping_ok = 0;
  }
}
*/

typedef struct PingInfo
{
	int s;
  	int count;
  	int timeout;
  	struct ip_addr ping_target;
	uint8_t ping_ok;
}PING_INFO;

PING_INFO p_info = {.s = -1, .count = 0};

void PingPool(void)
{
	if(p_info.s < 0) return;

	if(ping_recv(p_info.s) == 0) p_info.count++;
	else
	{
		p_info.ping_ok = 1;
		AddLogMessage("PING OK!!!", 0, 0);
		p_info.count = 0;
	}
	
	if (ping_send(p_info.s, &p_info.ping_target) == ERR_OK)
	{
      	LWIP_DEBUGF( PING_DEBUG, ("ping: send "));
      	ip_addr_debug_print(PING_DEBUG, &p_info.ping_target);
      	LWIP_DEBUGF( PING_DEBUG, ("\n"));
	}
	
	if(p_info.count > 5)
	{
		p_info.ping_ok = 0;	
		AddLogMessage("PING FAIL!!!", 0, 0);
	}
}


void ResetPing(void)
{
	p_info.ping_ok = 1;
	p_info.count = 0;
}

uint8_t IsPingOk(void)
{
	return 	p_info.ping_ok;
}


void PingInit(void)
{
	char *p_data;

	p_info.count = 0;
	p_info.timeout = PING_RCV_TIMEO;
	p_info.ping_ok = 1;

	if(p_info.s < 0)
	{
		if ((p_info.s = lwip_socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP)) < 0) 
    		return;

  		lwip_setsockopt(p_info.s, SOL_SOCKET, SO_RCVTIMEO, &p_info.timeout, sizeof(p_info.timeout));
	}

	if(GetSettingsFileStatus())
    {
		p_data = GetPointToElement("PING", "IP");
        if(p_data == 0  || strlen((char *)p_data) > 15)
        {
             lwip_close(p_info.s);//AddError(ERR_AG_FILE);
        }
        else p_info.ping_target.addr = GetNumericIP(p_data);
	}
}


//void
//ping_init(void)
//{
//#if LWIP_SOCKET
//  sys_thread_new("ping_thread", ping_thread, NULL, (DEFAULT_THREAD_STACKSIZE - 128), DEFAULT_THREAD_PRIO);
//#else /* LWIP_SOCKET */
//  ping_raw_init();
//#endif /* LWIP_SOCKET */
//}  

#endif /* LWIP_RAW && LWIP_ICMP */
#endif
#else
	void PingInit(void){}
	void ResetPing(void){}

	uint8_t IsPingOk(void){
		return 	1;
	}
#endif
