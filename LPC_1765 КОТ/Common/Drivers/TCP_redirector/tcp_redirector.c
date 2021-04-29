#include "Config.h"
#if(_TCP_REDIRECTOR_)
#include "lwip/sockets.h"
#include "lwip/tcp.h"
#include "PPPOverDevTask.h"
#include "tcp_redirector.h"
#include <string.h>
#include <stdint.h>

static int CreateTCP_1(void);
static int CreateTCP_2(void);
static DEVICE_HANDLE OpenTCP_1(void);
static DEVICE_HANDLE OpenTCP_2(void);
static void CloseTCP(DEVICE_HANDLE handle);
static int ReadTCP(DEVICE_HANDLE handle, void * dst, int count);
static int WriteTCP(DEVICE_HANDLE handle, const void * dst, int count);
static int SeekTCP(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
static int WriteTCPText(DEVICE_HANDLE handle, void * pSrc, int count);
static int ReadTCPText(DEVICE_HANDLE handle, void * pDst, int count);
static int SeekTCPText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
static int TCPIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData);

//static void write_tcp_task( void *pvParameters );
static void tcp_redir_pool(DEVICE_HANDLE handle);
static void give_mem_chain(MEM_CHAIN *pChain);
static MEM_CHAIN * take_mem_chain(DEVICE_HANDLE handle, int flag);


static uint8_t tcp_redir_valid(DEVICE_HANDLE handle);
static void tcp_redir_listen(DEVICE_HANDLE handle);
static int tcp_redir_con(DEVICE_HANDLE handle);

TCP_REDIRECTOR_INFO tcp_redirector_1_info = {.lis_soket = -1, .work_soket = -1, .sock_type = 0, .timeout = TCP_READ_DEF_TIMEOUT, .f_busy = 0};
TCP_REDIRECTOR_INFO tcp_redirector_2_info = {.lis_soket = -1, .work_soket = -1, .sock_type = 0, .timeout = TCP_READ_DEF_TIMEOUT, .f_busy = 0};



const DEVICE_IO tcp_device_1_io = {
    .DeviceID = TCP_DEV,
	.CreateDevice =	CreateTCP_1,
	.CloseDevice = CloseTCP,
    .OpenDevice = OpenTCP_1,
    .ReadDevice = ReadTCP,
    .WriteDevice = WriteTCP,
    .SeekDevice = SeekTCP,
    .ReadDeviceText = ReadTCPText,
    .WriteDeviceText = WriteTCPText,
    .SeekDeviceText = SeekTCPText,
    .DeviceIOCtl = TCPIOCtl,
    .DeviceName = "TCP_RD_1",
    };

DEVICE tcp_device_1 = {
    .device_io = (DEVICE_IO *)&tcp_device_1_io,
    .flDeviceOpen = 0,
    .pDriverData = &tcp_redirector_1_info,
    .pData = 0	
    };

const DEVICE_IO tcp_device_2_io = {
    .DeviceID = TCP_DEV,
	.CreateDevice =	CreateTCP_2,
	.CloseDevice = CloseTCP,
    .OpenDevice = OpenTCP_2,
    .ReadDevice = ReadTCP,
    .WriteDevice = WriteTCP,
    .SeekDevice = SeekTCP,
    .ReadDeviceText = ReadTCPText,
    .WriteDeviceText = WriteTCPText,
    .SeekDeviceText = SeekTCPText,
    .DeviceIOCtl = TCPIOCtl,
    .DeviceName = "TCP_RD_2",
    };

DEVICE tcp_device_2 = {
    .device_io = (DEVICE_IO *)&tcp_device_2_io,
    .flDeviceOpen = 0,
    .pDriverData = &tcp_redirector_2_info,
    .pData = 0	
    };

static int CreateTCP_1(void)
{
	tcp_redirector_1_info.tcp_r_queue = xQueueCreate(2, sizeof(TCP_R_MESSAGE));
	tcp_redirector_1_info.pMainMemChain = &tcp_redirector_1_info.mStart_1;
	tcp_redirector_1_info.mStart_1.mem_chain = NULL;
	tcp_redirector_1_info.mStart_2.mem_chain = NULL;
	create_sem(tcp_redirector_1_info.mStart_1.sem);
	create_sem(tcp_redirector_1_info.mStart_2.sem);	
	
	return DEVICE_OK;
}

static int CreateTCP_2(void)
{
	tcp_redirector_2_info.tcp_r_queue = xQueueCreate(2, sizeof(TCP_R_MESSAGE));
	tcp_redirector_2_info.pMainMemChain = &tcp_redirector_2_info.mStart_1;
	tcp_redirector_2_info.mStart_1.mem_chain = NULL;
	tcp_redirector_2_info.mStart_2.mem_chain = NULL;
	create_sem(tcp_redirector_2_info.mStart_1.sem);
	create_sem(tcp_redirector_2_info.mStart_2.sem);	 
	
	return DEVICE_OK;
}

static DEVICE_HANDLE OpenTCP_1(void)
{
	return &tcp_device_1;	
}

static DEVICE_HANDLE OpenTCP_2(void)
{
	return &tcp_device_2;	
}

static int WriteTCPText(DEVICE_HANDLE handle, void * pSrc, int count)
{
    return -1;
}

static int ReadTCPText(DEVICE_HANDLE handle, void * pDst, int count)
{
#if(_TEXT_)
    return 0;
#else
	return -1;
#endif
}

static int SeekTCPText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
#if(_TEXT_)
	return 0;
#else
	return -1;
#endif
	
}

static int SeekTCP(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
    return -1;
}

static void CloseTCP (DEVICE_HANDLE handle)
{
/*	TCP_REDIRECTOR_INFO *tcp_info = (TCP_REDIRECTOR_INFO *)(((DEVICE *)handle)->pDriverData);
	if(tcp_info->work_soket >= 0)
	{
		closesocket(tcp_info->work_soket);
		tcp_info->work_soket = -1;
	}  */
	//handle = NULL;	
}


static int WriteTCP (DEVICE_HANDLE handle, const void * pSrc, int count)
{
	TCP_R_MESSAGE mes;
	TCP_REDIRECTOR_INFO *tcp_info = (TCP_REDIRECTOR_INFO *)(((DEVICE *)handle)->pDriverData);
	MEM_CHAIN_SEG *pMem = NULL, *pChain = NULL;
	MEM_CHAIN     *Chain = NULL;
	if(tcp_info->work_soket < 0)
		return 0;

	Chain = take_mem_chain(handle, 1);
	pChain = Chain->mem_chain;
//	xSemaphoreTake(tcp_info->tx_sem, 0xFFFFFFFF);

	pMem = (MEM_CHAIN_SEG *)pvPortMalloc((size_t)(sizeof(MEM_CHAIN_SEG) + count));
	if(pMem == NULL)
	{
		return 0;
	}
	pMem->size = count;
	pMem->data = ((uint8_t *)pMem + sizeof(MEM_CHAIN_SEG));
	pMem->pNext = NULL;

	memcpy((char *)pMem->data, (char *)pSrc, count);

	if(pChain != NULL)
	{
		while(pChain->pNext != NULL)
		{
			pChain = pChain->pNext;
		}
		pChain->pNext = pMem;
	}
	else Chain->mem_chain = pMem;


	xQueueSendToBack(tcp_info->tcp_r_queue, ( void * )&mes, 0x0);

	give_mem_chain(tcp_info->pMainMemChain);

	return count; 
	
}

static int ReadTCP(DEVICE_HANDLE handle, void * pDst, int count) 
{
	TCP_REDIRECTOR_INFO *tcp_info = (TCP_REDIRECTOR_INFO *)(((DEVICE *)handle)->pDriverData);
	int len = 0;

	if(tcp_info->work_soket < 0)
		return -1;


	len = recv(tcp_info->work_soket, (uint8_t *)pDst, count, 0);

	
	if(len < 0)
	{
		closesocket(tcp_info->work_soket);
		tcp_info->work_soket = -1;
	}
	
	return len;	
				 
}


static int TCPIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData)
{
	TCP_REDIRECTOR_INFO *tcp_info = (TCP_REDIRECTOR_INFO *)(((DEVICE *)handle)->pDriverData);

	switch(code)
	{
		case TCP_REDIR_SET_PORT:						  //  Настройка номера порта
			tcp_info->port =  *(uint16_t *)pData;
			break;
		case TCP_REDIR_SET_IP:						  //  Настройка ip адреса
			tcp_info->addr =  *(uint32_t *)pData;
			break;
		case TCP_REDIR_SET_TIMEOUT:
			tcp_info->timeout =  *(int *)pData;
			break;
		case DEVICE_PRE_CLOSE:
			if(tcp_info->work_soket >= 0)
			{
				closesocket(tcp_info->work_soket);
				tcp_info->work_soket = -1;
			}
			break;
		case TCP_REDIR_CONNECT:
			return tcp_redir_con(handle);
		//	break;
		case DEVICE_VALID_CHECK:
			*pData = tcp_redir_valid(handle);
			break;
		case TCP_REDIR_LISTEN:
			tcp_redir_listen(handle);
			break;
		case POOL:
			tcp_redir_pool(handle);
			break;


		//case GET_ERRNO	  : return netconn_err(tcp_info->newconn);  
	}

	return -1;				
}


static int tcp_redir_con(DEVICE_HANDLE handle)
{
	TCP_REDIRECTOR_INFO *tcp_info = (TCP_REDIRECTOR_INFO *)(((DEVICE *)handle)->pDriverData);
  	struct sockaddr_in addr;
  	int s, err;

	if(tcp_info->work_soket >= 0)
		closesocket(tcp_info->work_soket);
	tcp_info->work_soket = -1;
		

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (s < 0)
    	return -1;

//	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void*) &one, sizeof(one));	 //  Не блакирует ip адрес для других сокетов
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tcp_info->timeout, sizeof(tcp_info->timeout));
  //	ioctlsocket(s, FIONBIO, (void*) &one);    							//  	Работа без блокировки на функциях

	memset(&addr, 0, sizeof(addr));
	addr.sin_len = sizeof(addr);
  	addr.sin_family      = AF_INET;
  	addr.sin_addr.s_addr = tcp_info->addr;
  	addr.sin_port        = tcp_info->port;

	err = connect(s, (struct sockaddr *)&addr, sizeof(addr));
	if(err < 0)
	{
		closesocket(s);
      	return -1;
	}

	SetSocketTCPPrio(s, 100);

	//ioctlsocket(s, FIONBIO, (void*) &one); 
	tcp_info->work_soket = s;
	tcp_info->sock_type = TCP_REDIR_SOCK_CON;

	DEBUG_PUTS("TCP_R CONNECTED !!!");
	return 0;
}


static void tcp_redir_listen(DEVICE_HANDLE handle)
{
	TCP_REDIRECTOR_INFO *tcp_info = (TCP_REDIRECTOR_INFO *)(((DEVICE *)handle)->pDriverData);
	unsigned long one = 1;
  	struct sockaddr_in addr;
  	int s = -1, err;

	if(tcp_info->work_soket >= 0)
		closesocket(tcp_info->work_soket);
	tcp_info->work_soket = -1;

	if(tcp_info->lis_soket >= 0)
		closesocket(tcp_info->lis_soket);
	tcp_info->lis_soket = -1;

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (s < 0)
    	return;

	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void*) &one, sizeof(one));	 //  Не блакирует ip адрес для других сокетов
	//setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tcp_redirector_info.timeout, sizeof(tcp_redirector_info.timeout));
 	ioctlsocket(s, FIONBIO, (void*) &one);    							//  	Работа без блокировки на функциях


	memset(&addr, 0, sizeof(addr));
	addr.sin_len = sizeof(addr);
  	addr.sin_family      = AF_INET;
  	addr.sin_addr.s_addr = 0;
  	addr.sin_port        = htons(tcp_info->port);

  	err = bind(s, (struct sockaddr *)&addr, sizeof(struct sockaddr));
  	if (err < 0)
  	{
     	closesocket(s);
      	return;
  	}

  	err = listen(s, 20);
	if(err < 0) return;

	tcp_info->lis_soket = s;
	FD_ZERO(&tcp_info->SocketSet);
	FD_SET(tcp_info->lis_soket, &tcp_info->SocketSet);

	tcp_info->sock_type = TCP_REDIR_SOCK_LISTEN;
}

static uint8_t tcp_redir_valid(DEVICE_HANDLE handle)
{
	TCP_REDIRECTOR_INFO *tcp_info = (TCP_REDIRECTOR_INFO *)(((DEVICE *)handle)->pDriverData);
	int i, s;
//	unsigned long one = 1;
	struct timeval timeout;
	struct sockaddr_in addr;
	u32_t  alen = sizeof(addr);
	fd_set         fds;

	if(tcp_info->sock_type == TCP_REDIR_SOCK_LISTEN)
	{
		if(tcp_info->lis_soket == -1)
		{
			tcp_redir_listen(handle);
			return 0;
		} 
		//if(tcp_redirector_info.work_soket != -1) return 1;
		timeout.tv_sec  = 0;
		timeout.tv_usec = 10000;
		fds = tcp_info->SocketSet;
		i = select(FD_SETSIZE, &fds, NULL, NULL, &timeout);
		if(i > 0)
		{
			if (FD_ISSET(tcp_info->lis_soket, &fds))
      		{
				s = accept(tcp_info->lis_soket, (struct sockaddr *)&addr, &alen);
				setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tcp_info->timeout, sizeof(tcp_info->timeout));
			//	ioctlsocket(s, FIONBIO, (void*) &one); 
				if(tcp_info->work_soket >= 0)
					closesocket(tcp_info->work_soket);
				tcp_info->work_soket = s;
			}
		}
	}

	if(tcp_info->work_soket < 0) return 0;
	else return 1;
}


//static uint8_t re_buf[512];
//static void write_tcp_task( void *pvParameters )
static void tcp_redir_pool(DEVICE_HANDLE handle)
{
	TCP_REDIRECTOR_INFO *tcp_info = (TCP_REDIRECTOR_INFO *)(((DEVICE *)handle)->pDriverData);
	TCP_R_MESSAGE mes;
	int len = 0/*, i = 0*/, len_w = 0;

	MEM_CHAIN_SEG *pMem = NULL, *pMemPrev = NULL;	
	MEM_CHAIN     *Chain = NULL;



	if(xQueueReceive(tcp_info->tcp_r_queue, (void *)&mes, 0))
	{
		Chain = take_mem_chain(handle, 0);
		pMem = Chain->mem_chain;
		while(pMem)
		{
			len_w = 0;
			while(pMem->size) 
			{
				len = pMem->size;
				if(len > 350) len = 350;
				len = send(tcp_info->work_soket, (void *)&((pMem->data)[len_w]), len, NULL);
				if(len < 0) break;
				pMem->size -= len;
				len_w += len;
			}
			pMemPrev = pMem;
			pMem = pMem->pNext;
			vPortFree(pMemPrev);			
		}
		Chain->mem_chain = NULL;
		give_mem_chain(Chain);

		if(len < 0)
		{
			closesocket(tcp_info->work_soket);
			tcp_info->work_soket = -1;
		}

	}		
}

static void chaing_main_chain(DEVICE_HANDLE handle)
{
	TCP_REDIRECTOR_INFO *tcp_info = (TCP_REDIRECTOR_INFO *)(((DEVICE *)handle)->pDriverData);
	if(tcp_info->pMainMemChain == &tcp_info->mStart_1)
		tcp_info->pMainMemChain = &tcp_info->mStart_2;
	else if(tcp_info->pMainMemChain == &tcp_info->mStart_2)
		tcp_info->pMainMemChain = &tcp_info->mStart_1;
}


static MEM_CHAIN * take_mem_chain(DEVICE_HANDLE handle, int flag)
{
	TCP_REDIRECTOR_INFO *tcp_info = (TCP_REDIRECTOR_INFO *)(((DEVICE *)handle)->pDriverData);
	if(flag == 1)
	{
		if(take_sem(tcp_info->pMainMemChain->sem, 0x0) != pdTRUE)
			chaing_main_chain(handle);
		take_sem(tcp_info->pMainMemChain->sem, 0x0);
	}
	else
	{
		take_sem(tcp_info->pMainMemChain->sem, 0xFFFFFFFF);
	}
	return tcp_info->pMainMemChain;
}

static void give_mem_chain(MEM_CHAIN *pChain)
{
	give_sem(pChain->sem);
}

#endif
