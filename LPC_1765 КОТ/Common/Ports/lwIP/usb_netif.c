/* This source file is part of the ATMEL AVR32-SoftwareFramework-AT32UC3A-1.2.2ES Release */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
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
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include "Config.h"
#include "DriversCore.h"

#include "opt.h"
#include "def.h"
#include "mem.h"
#include "pbuf.h"
#include "sys.h"
#include "tcpip.h"
#include "stats.h"


#include "etharp.h"
#include "lwip/timers.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"

#define netifMTU              (600)
#define netifGUARD_BLOCK_TIME       ( 250 )
#define IFNAME0 'e'
#define IFNAME1 'm'


static xTaskHandle USBEthernetHandle = NULL;
static xSemaphoreHandle xCCCSemaphore = NULL;

struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
};


/* Forward declarations. */
void  ethernetif_input(void * );
static err_t ethernetif_output(struct netif *netif, struct pbuf *p,
             struct ip_addr *ipaddr);
static struct netif *xNetIf = NULL;

int RndisRxReady(void);
int RndisRead(uint8_t *buf, int count);
int RndisWrite(uint8_t *buf, int count);


static void
low_level_init(struct netif *netif)
{

  /* maximum transfer unit */
  netif->mtu = netifMTU;
  
  /* broadcast capability */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
 
  /* Do whatever else is needed to initialize interface. */  
  xNetIf = netif;

  /* Initialise the MACB.  This routine contains code that polls status bits.
  If the Ethernet cable is not plugged in then this can take a considerable
  time.  To prevent this starving lower priority tasks of processing time we
  lower our priority prior to the call, then raise it back again once the
  initialisation is complete. */

  /* Create the task that handles the MACB. */
  if(USBEthernetHandle == NULL)
   	xTaskCreate( ethernetif_input, ( signed portCHAR * ) "ETH_INT", 128, NULL, 1, &USBEthernetHandle );
}

/*
 * low_level_output():
 *
 * Should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 */

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
	struct pbuf *q;
	err_t xReturn = ERR_OK;

	DEVICE_HANDLE	hUSB = OpenDevice("USB");

  /* Parameter not used. */
  ( void ) netif;

  #if ETH_PAD_SIZE
    pbuf_header( p, -ETH_PAD_SIZE );    /* drop the padding word */
  #endif

  /* Access to the MACB is guarded using a semaphore. */

    for( q = p; q != NULL; q = q->next )
    {
      /* Send the data from the pbuf to the interface, one pbuf at a
      time. The size of the data in each pbuf is kept in the ->len
      variable.  if q->next == NULL then this is the last pbuf in the
      chain. */
		WriteDevice(hUSB, q->payload, q->len);  
    }

  #if ETH_PAD_SIZE
    pbuf_header( p, ETH_PAD_SIZE );     /* reclaim the padding word */
  #endif

  #if LINK_STATS
    lwip_stats.link.xmit++;
  #endif /* LINK_STATS */
	CloseDevice(hUSB);
    return xReturn;
}

/*
 * low_level_input():
 *
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 */

static struct pbuf *
low_level_input(struct netif *netif) {
	struct pbuf         *p = NULL;
	struct pbuf         *q;
	u16_t               len = 0;


  /* Parameter not used. */
  ( void ) netif;

  DEVICE_HANDLE	hUSB = OpenDevice("USB");


  /* Access to the emac is guarded using a semaphore. */

	DeviceIOCtl(hUSB, RX_DATA_COUNT, (uint8_t *)&len);

    if( len )
    {
      #if ETH_PAD_SIZE
        len += ETH_PAD_SIZE;    /* allow room for Ethernet padding */
      #endif
  
      /* We allocate a pbuf chain of pbufs from the pool. */
      p = pbuf_alloc( PBUF_RAW, len, PBUF_POOL );
  
      if( p != NULL )
      {
        #if ETH_PAD_SIZE
          pbuf_header( p, -ETH_PAD_SIZE );    /* drop the padding word */
        #endif
  
        /* Let the driver know we are going to read a new packet. */
  
        /* We iterate over the pbuf chain until we have read the entire
        packet into the pbuf. */
        for( q = p; q != NULL; q = q->next )
        {
          /* Read enough bytes to fill this pbuf in the chain. The
          available data in the pbuf is given by the q->len variable. */

			ReadDevice(hUSB, q->payload, q->len);
        }
  
        #if ETH_PAD_SIZE
          pbuf_header( p, ETH_PAD_SIZE );     /* reclaim the padding word */
        #endif
        #if LINK_STATS
          lwip_stats.link.recv++;
        #endif /* LINK_STATS */
      }
      else
      {
        #if LINK_STATS
          lwip_stats.link.memerr++;
          lwip_stats.link.drop++;
        #endif /* LINK_STATS */
      }
    }

	CloseDevice(hUSB);

  return p;
}

/*
 * ethernetif_output():
 *
 * This function is called by the TCP/IP stack when an IP packet
 * should be sent. It calls the function called low_level_output() to
 * do the actual transmission of the packet.
 *
 */

static err_t
ethernetif_output(struct netif *netif, struct pbuf *p,
      struct ip_addr *ipaddr)
{
  
 /* resolve hardware address, then send (or queue) packet */
  return etharp_output(netif, p, ipaddr);
 
}

/*
 * ethernetif_input():
 *
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface.
 *
 */

void ethernetif_input( void * pvParameters )
{

struct pbuf         *p;

  ( void ) pvParameters;
  DEVICE_HANDLE	hUSB = NULL;

  for( ;; )  {
   
    do
    {
	  hUSB = OpenDevice("USB");
	  DeviceIOCtl(hUSB, POOL, NULL);
	  CloseDevice(hUSB);
	  hUSB = NULL;
      /* move received packet into a new pbuf */
      p = low_level_input( xNetIf );

      if( p == NULL )
      {
        /* No packet could be read.  Wait a for an interrupt to tell us
        there is more data available. */
        vTaskDelay(1);
      }

    } while( p == NULL );

	//ethernet_input(p, xNetIf);
	xNetIf->input( p, xNetIf );
 }

//    /* points to packet payload, which starts with an Ethernet header */
//    ethhdr = p->payload;
//  
//    #if LINK_STATS
//      lwip_stats.link.recv++;
//    #endif /* LINK_STATS */
//  
//    ethhdr = p->payload;
//  
//    switch( htons( ethhdr->type ) )
//    {
//      /* IP packet? */
//      case ETHTYPE_IP:
//        /* update ARP table */
//      //  etharp_ip_input( xNetIf, p );
//  
//        /* skip Ethernet header */
//        pbuf_header( p, (s16_t)-sizeof(struct eth_hdr) );
//  
//        /* pass to network layer */
//        xNetIf->input( p, xNetIf );
//        break;
//  
//      case ETHTYPE_ARP:
//        /* pass p to ARP module */
////        etharp_arp_input( xNetIf, ethernetif->ethaddr, p );
//        break;
//  
//      default:
//        pbuf_free( p );
//        p = NULL;
//        break;
//    }
//  }
}

static void
arp_timer(void *arg)
{
  etharp_tmr();
  sys_timeout(ARP_TMR_INTERVAL, arp_timer, NULL);
}

/*
 * ethernetif_init():
 *
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 */

err_t ethernetif_init(struct netif *netif)
{
  struct ethernetif *ethernetif;
  //int i;

  vSemaphoreCreateBinary( xCCCSemaphore );
    
  ethernetif = (struct ethernetif *)mem_malloc(sizeof(struct ethernetif));
  
  if (ethernetif == NULL)
  {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
    return ERR_MEM;
  }
  
  netif->state = ethernetif;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  netif->output = ethernetif_output;
  netif->linkoutput = low_level_output;
  
  //for(i = 0; i < 6; i++) netif->hwaddr[i] = MACB_if.hwaddr[i];
  ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);
  
  low_level_init(netif);

  etharp_init();

  sys_timeout(ARP_TMR_INTERVAL, arp_timer, NULL);

  return ERR_OK;
}

struct netif usb_netif;

/* The IP address being used. */
#define emacIPADDR0 192
#define emacIPADDR1 168
#define emacIPADDR2 11
#define emacIPADDR3 111


/* The IP address being used. */
#define emacMACADDR0 0x26
#define emacMACADDR1 0x25
#define emacMACADDR2 0x24
#define emacMACADDR3 0x23
#define emacMACADDR4 0x22
#define emacMACADDR5 0x21

/* The gateway address being used. */
#define emacGATEWAY_ADDR0 192
#define emacGATEWAY_ADDR1 168
#define emacGATEWAY_ADDR2 11
#define emacGATEWAY_ADDR3 110

/* The network mask being used. */
#define emacNET_MASK0 255
#define emacNET_MASK1 255
#define emacNET_MASK2 255
#define emacNET_MASK3 0



void USBConfigureInterface(void * param)
{
struct ip_addr xIpAddr, xNetMask, xGateway;
//	int i = 0;

	
	IP4_ADDR(&xIpAddr,emacIPADDR0,emacIPADDR1,emacIPADDR2,emacIPADDR3);
	IP4_ADDR(&xNetMask,emacNET_MASK0,emacNET_MASK1,emacNET_MASK2,emacNET_MASK3);
	IP4_ADDR(&xGateway,emacGATEWAY_ADDR0,emacGATEWAY_ADDR1,emacGATEWAY_ADDR2,emacGATEWAY_ADDR3);

	//for(i = 0; i < 6; i++) usb_netif.hwaddr[i] = AppConfig.MyMACAddr.v[i];
	usb_netif.hwaddr[0] = emacMACADDR0;
	usb_netif.hwaddr[1] = emacMACADDR1;
	usb_netif.hwaddr[2] = emacMACADDR2;
	usb_netif.hwaddr[3] = emacMACADDR3;
	usb_netif.hwaddr[4] = emacMACADDR4;
	usb_netif.hwaddr[5] = emacMACADDR5;
	usb_netif.hwaddr_len = 6;


  /* add data to netif */
  netif_add(&usb_netif, &xIpAddr, &xNetMask, &xGateway, NULL, ethernetif_init, tcpip_input);
  /* make it the default interface */
  //netif_set_default(&usb_netif);
  /* bring it up */
  netif_set_up(&usb_netif);

  //delay(10000);

  //dhcp_start(&usb_netif);
}

void USBInterfaceDown(void)
{
	if(USBEthernetHandle)
	{
		vTaskDelete(&USBEthernetHandle);
		USBEthernetHandle = NULL;
	}
	netif_set_down(&usb_netif);
	netif_remove(&usb_netif);
}

