#ifndef TCP_REDIRECTOR_H
#define TCP_REDIRECTOR_H

#include "Config.h"
#if(_TCP_REDIRECTOR_)

#include <stdint.h>
#include "DriversCore.h"														   
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/sockets.h"


#define TCP_REDIR_WRITER_BUF		1024
#define	ERR_MAX_COUNT				5

typedef struct __attribute__ ((__packed__)) tagMemChainSeg
{
	int 					size;
	struct tagMemChainSeg  *pNext;
	void					*data;
}MEM_CHAIN_SEG;

typedef struct tagMemChain
{
	MEM_CHAIN_SEG 		*mem_chain;
	xSemaphoreHandle 	sem;
}MEM_CHAIN;

typedef struct tagTCPRedirinfo
{
	int lis_soket;
	int work_soket;
	
	uint32_t 	addr;
	uint16_t 	port;

	uint8_t		sock_type;

	int		    timeout;

	MEM_CHAIN	mStart_1;
	MEM_CHAIN	mStart_2;
	MEM_CHAIN	*pMainMemChain;
	xQueueHandle tcp_r_queue;
	fd_set		SocketSet;

//	xSemaphoreHandle sem;
//	xSemaphoreHandle tx_sem;

//	uint8_t buf[TCP_REDIR_WRITER_BUF];
	uint8_t f_busy;
	uint16_t in;
	uint16_t out;
		
}TCP_REDIRECTOR_INFO;


//typedef struct tagWriterInfo
//{
//	uint8_t buf[TCP_REDIR_WRITER_BUF];
//	uint16_t in;
//	uint16_t out;
//}TCP_WRITER_INFO;

typedef struct tagTcpRedir
{
	uint16_t f_sem;
}TCP_R_MESSAGE;



#define		TCP_REDIR_SET_PORT		0
#define		TCP_REDIR_SET_IP		1
#define		TCP_REDIR_DISCON		2
#define		TCP_REDIR_CONNECT		3
#define		TCP_REDIR_LISTEN		4
#define		TCP_REDIR_SET_TIMEOUT	5


#define		TCP_REDIR_SOCK_CON		0
#define		TCP_REDIR_SOCK_LISTEN	1

#endif
#endif
