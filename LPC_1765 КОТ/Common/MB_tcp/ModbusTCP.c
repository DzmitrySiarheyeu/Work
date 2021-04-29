#include "Config.h"
#if(_MB_TCP_)
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include "lwip/sys.h"
#include "lwip/def.h"
#include "lwip/sockets.h"
#include "lwip/stats.h"
#include "lwip/ip.h"
#include "MB_client.h"
#include "ModbusTCP.h"

/*---------------------------------------------------------------------------
 *  DEFINES
 *-------------------------------------------------------------------------*/

/* ----------------------- MBAP Header --------------------------------------*/
/*
 *
 * <------------------------ MODBUS TCP/IP ADU(1) ------------------------->
 *              <----------- MODBUS PDU (1') ---------------->
 *  +-----------+---------------+------------------------------------------+
 *  | TID | PID | Length | UID  |Code | Data                               |
 *  +-----------+---------------+------------------------------------------+
 *  |     |     |        |      |                                           
 * (2)   (3)   (4)      (5)    (6)                                          
 *
 * (2)  ... MB_TCP_TID          = 0 (Transaction Identifier - 2 Byte) 
 * (3)  ... MB_TCP_PID          = 2 (Protocol Identifier - 2 Byte)
 * (4)  ... MB_TCP_LEN          = 4 (Number of bytes - 2 Byte)
 * (5)  ... MB_TCP_UID          = 6 (Unit Identifier - 1 Byte)
 * (6)  ... MB_TCP_FUNC         = 7 (Modbus Function Code)
 *
 * (1)  ... Modbus TCP/IP Application Data Unit
 * (1') ... Modbus Protocol Data Unit
 */

#define MB_TCP_TID          0
#define MB_TCP_PID          2
#define MB_TCP_LEN          4
#define MB_TCP_UID          6
#define MB_TCP_FUNC         7

#define MB_TCP_PROTOCOL_ID  0   /* 0 = Modbus Protocol */

#define MB_TIMEOUT     900
#define MB_INPBUFSIZE  512

#define MB_FUNC_NONE                          (  0 )
#define MB_FUNC_READ_COILS                    (  1 )
#define MB_FUNC_READ_DISCRETE_INPUTS          (  2 )
#define MB_FUNC_WRITE_SINGLE_COIL             (  5 )
#define MB_FUNC_WRITE_FILE			          ( 15 )
#define MB_FUNC_READ_FILE			          ( 14 )
#define MB_FUNC_READ_HOLDING_REGISTER         (  3 )
#define MB_FUNC_READ_INPUT_REGISTER           (  4 )
#define MB_FUNC_WRITE_REGISTER                (  6 )
#define MB_FUNC_WRITE_MULTIPLE_REGISTERS      ( 16 )
#define MB_FUNC_READWRITE_MULTIPLE_REGISTERS  ( 23 )
#define MB_FUNC_DIAG_READ_EXCEPTION           (  7 )
#define MB_FUNC_DIAG_DIAGNOSTIC               (  8 )
#define MB_FUNC_DIAG_GET_COM_EVENT_CNT        ( 11 )
#define MB_FUNC_DIAG_GET_COM_EVENT_LOG        ( 12 )
#define MB_FUNC_OTHER_REPORT_SLAVEID          ( 17 )
#define MB_FUNC_ERROR                         ( 128 )

#define		MB_ADDRESS_BROADCAST		0
#define 	MB_PDU_FUNC_OFF     		0   /*!< Offset of function code in PDU. */
#define 	MB_PDU_ADDR_OFF     		1   /*!< Offset for response data in PDU. */
#define 	MB_PDU_REG_NUM_OFF     		2   /*!< Offset for response data in PDU. */
#define 	MB_PDU_REG_NUM_OFF     		2   /*!< Offset for response data in PDU. */

#define sysGetErrno()       	errno
#define sysSleep(ms)            posTaskSleep(ms)
#define sysMemAlloc(s)          mem_malloc(s)
#define sysMemFree(x)           mem_free(x)
#define sysMemCopy(d,s,l)       memcpy(d,s,l)
#define sysMemSet(d,v,s)        memset(d,v,s)


/*---------------------------------------------------------------------------
 *  TYPEDEFS
 *-------------------------------------------------------------------------*/

typedef enum
{
    MB_ENOERR = 1,                  /*!< no error. */
    MB_ENOREG,                  /*!< illegal register address. */
    MB_EINVAL,                  /*!< illegal argument. */
    MB_EPORTERR,                /*!< porting layer error. */
    MB_ENORES,                  /*!< insufficient resources. */
    MB_EIO,                     /*!< I/O error. */
    MB_EILLSTATE,               /*!< protocol stack in illegal state. */
    MB_ETIMEDOUT                /*!< timeout error occurred. */
} MBErrorCode;

typedef enum
{
    MB_EX_NONE = 0x00,
    MB_EX_ILLEGAL_FUNCTION = 0x01,
    MB_EX_ILLEGAL_DATA_ADDRESS = 0x02,
    MB_EX_ILLEGAL_DATA_VALUE = 0x03,
    MB_EX_SLAVE_DEVICE_FAILURE = 0x04,
    MB_EX_ACKNOWLEDGE = 0x05,
    MB_EX_SLAVE_BUSY = 0x06,
    MB_EX_MEMORY_PARITY_ERROR = 0x08,
    MB_EX_GATEWAY_PATH_FAILED = 0x0A,
    MB_EX_GATEWAY_TGT_FAILED = 0x0B
} MBException;

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
	int                 sock;
	u16_t				buflen;
	char				rxbuffer[MB_INPBUFSIZE];
}MB_CONNECTION;


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

static int		mb_newServerSocket(void);
static int		mb_fdSanity(void);
static int		mb_processConnections(fd_set *clientset, int fdcount);

static void     mb_acceptClient(void);
static void     mb_timeout(uint32_t cur_t);
static void    	mb_listAddTail(lelem_t *headelem, lelem_t *elem);
//static void    	mb_listAddHead(lelem_t *headelem, lelem_t *elem);
static void    	mb_listDel(lelem_t *elem);

static void		mb_destroyConnection(MB_CONNECTION *conn);
static int		mb_addSocket(const int sock);
static void		mb_delSocket(const int sock);
static int		mb_newConnection(int socket);

static void 	mb_frame_send(MB_CONNECTION *conn);
static void 	mb_pool(MB_CONNECTION *conn);


/*---------------------------------------------------------------------------
 *  GLOBAL VARIABLES
 *-------------------------------------------------------------------------*/

static lelem_t      connListRoot_g;
static uint16_t 	port_g;

static fd_set       globalSocketSet;
static int          serverSocket_g;
static u32_t        open_connections_g;


/*---------------------------------------------------------------------------
 *  MODBUS DATA HANDLING 
 *-------------------------------------------------------------------------*/

static void mb_pool(MB_CONNECTION *conn)
{
	uint16_t          PID;
	uint16_t		  Length;
	uint8_t           *pMBTCPFrame;
	MBException 	  Exception;
	uint8_t           FunctionCode = 0;

	PID = conn->rxbuffer[MB_TCP_PID] << 8U;
    PID |= conn->rxbuffer[MB_TCP_PID + 1];

	Length = conn->rxbuffer[MB_TCP_LEN] << 8U;
    Length |= conn->rxbuffer[MB_TCP_LEN + 1];

	if( conn->buflen < ( MB_TCP_UID + Length ) )
    {
		return;
    }
    else if( conn->buflen > ( MB_TCP_UID + Length ) )
    {
		mb_destroyConnection(conn);
		return;
	}


	if(PID != MB_TCP_PROTOCOL_ID) goto mb_pool_exit;

	pMBTCPFrame = (uint8_t *)&conn->rxbuffer[MB_TCP_FUNC];

	FunctionCode = pMBTCPFrame[MB_PDU_FUNC_OFF];
	switch(FunctionCode)
	{
#if _MB_RD_IN_REG_FUNC_
		case MB_FUNC_READ_INPUT_REGISTER:
			Exception = (MBException)MBCReadInputRegisters((READ_INPUT_REGS *)pMBTCPFrame, (uint8_t *)&Length);	
			break;
#endif
#if _MB_WR_S_REG_FUNC_
		case MB_FUNC_WRITE_REGISTER:
			Exception = (MBException)MBCWriteSingleRegister((WRITE_SINGLE_REGISTER*)pMBTCPFrame, (uint8_t *)&Length);
			break;
#endif
#if	_MB_WR_FILE_FUNC_
		case MB_FUNC_WRITE_FILE :
			Exception = (MBException)MBCWriteFile((WRITE_FILE *)pMBTCPFrame, (uint8_t *)&Length);
			break;
#endif
#if	_MB_RD_FILE_FUNC_
		case MB_FUNC_READ_FILE :
			Exception = (MBException)MBCReadFile((READ_FILE *)pMBTCPFrame, (uint8_t *)&Length);
			break;
#endif
		default:
			Exception = MB_EX_ILLEGAL_FUNCTION;
            break;
	}

	if( Exception != MB_EX_NONE )
    {
    	/* An exception occured. Build an error frame. */
        Length = 0;
        pMBTCPFrame[Length++] = ( uint8_t )( FunctionCode | MB_FUNC_ERROR );
        pMBTCPFrame[Length++] = Exception;
    } 
	conn->buflen = Length;              
    mb_frame_send(conn);

mb_pool_exit:
	conn->buflen = 0;	
}


void mb_frame_send(MB_CONNECTION *conn)
{
	conn->rxbuffer[MB_TCP_LEN] = ( conn->buflen + 1 ) >> 8U;
    conn->rxbuffer[MB_TCP_LEN + 1] = ( conn->buflen + 1 ) & 0xFF;

	conn->buflen += MB_TCP_FUNC;
	if (send(conn->sock, conn->rxbuffer, conn->buflen, 0) < 0)
  	{
    	if (sysGetErrno() == EPIPE)
    	{
      		mb_destroyConnection(conn);
    	}
  	}
}


/*---------------------------------------------------------------------------
 *  CONNECTION HANDLING
 *-------------------------------------------------------------------------*/



/* Add a socket to the global socket set.
 */
static int mb_addSocket(const int sock)
{
  	if (sock >= 0)
  	{
    	FD_CLR(sock, &globalSocketSet);
    	FD_SET(sock, &globalSocketSet);
    	return 0;
  	}
  	return -1;
}


/* Remove socket from fdsets and close it.
 */
static void mb_delSocket(const int sock)
{
  if (sock >= 0)
  {
    FD_CLR(sock, &globalSocketSet);
    closesocket(sock);
  }
}


/*-------------------------------------------------------------------------*/


/* Insert an element to the tail of a double linked list.
 */
static void mb_listAddTail(lelem_t *headelem, lelem_t *elem)
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
static void mb_listDel(lelem_t *elem)
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
static int mb_newConnection(int socket)
{
  unsigned long one = 1;
  MB_CONNECTION *conn;

  conn = (MB_CONNECTION *)(sysMemAlloc(sizeof(MB_CONNECTION)));
  if (conn == NULL)
    return -1;

  conn->sock        = socket;
  conn->buflen      = 0;
  conn->lastTrans   = (xTaskGetTickCount() / 1000);

  /* set nonblocking mode */
  ioctlsocket(socket, FIONBIO, (void*)&one);

  /* add socket and connection to list of active connections */
  mb_addSocket(socket);
  mb_listAddTail(&connListRoot_g, (lelem_t*)(void*)conn);
  open_connections_g++;
  return 0;
}




/* Destroys a ctrl connection and
 * cleans up the connection structure.
 */
static void mb_destroyConnection(MB_CONNECTION *conn)
{

  if (conn == NULL)
    return;

  if ((conn->sock >= 0) && (open_connections_g > 0))
  {
    open_connections_g--;
  }

  mb_delSocket(conn->sock);
  mb_listDel((lelem_t*)(void*)conn);
  sysMemFree(conn);
}

/*-------------------------------------------------------------------------*/


/* This function processes all open control connections.
 * If data was received over the connection, it is
 * passed to the command line parser.
 */

static int mb_processConnections(fd_set *clientset, int fdcount)
{
  MB_CONNECTION *conn, *next;
  int checked;
  int   bytes;

  next = FIRST_LIST_ELEM(&connListRoot_g, MB_CONNECTION*);
  checked = 0;

  /* process all connections in the list */
  while (!END_OF_LIST(&connListRoot_g, next) && (checked < fdcount))
  {
    conn = next;
    next = NEXT_LIST_ELEM(conn, MB_CONNECTION*);

    if (!FD_ISSET(conn->sock, clientset))
    {
      continue;
    }

    checked++;

    bytes = recv(conn->sock, conn->rxbuffer + conn->buflen, (MB_INPBUFSIZE-1) - conn->buflen, 0);

    if (bytes <= 0)
    {
      /* If no bytes are available or -1 is returned,
       * the client must have closed the connection since
       * select told us that there was something with the socket.
       */
      mb_destroyConnection(conn);
      continue;
    }

    /* safety check: buffer overrun (should never happen) */
    if ( (conn->buflen + (u16_t) bytes) >= (MB_INPBUFSIZE-1))
    {
      mb_destroyConnection(conn);
      continue;
    }
    conn->buflen += bytes;
    conn->lastTrans = (xTaskGetTickCount() / 1000);

    mb_pool(conn);
  }
  return checked;
}




/* Accept and open a new connection to the next client.
 */
static void mb_acceptClient(void)
{
  struct sockaddr_in addr;
  int    s;
  static int errors = 0;
  u32_t  alen;


  alen = sizeof(addr);
  sysMemSet(&addr, 0, sizeof(addr));
  s = accept(serverSocket_g, (struct sockaddr *)&addr, &alen);

  if (s < 0)
  {

    if ((++errors >= 3))
    {
      /* try to recover server socket on too much errors */
//      mb_delSocket(serverSocket_g);
//      serverSocket_g = mb_newServerSocket();
	  PUT_MES_TO_LOG("MB_TCP: NO RESOURCE FOR INCOMMING CON", 0, 0);
	  PUT_MES_TO_LOG("MB_TCP: DEL ALL CLIENTS", 0, 0);
	  mb_fdSanity();
      errors = 0;
    }
  }
  else
  {
    errors = 0;

    if (mb_newConnection(s) != 0)
    {
      closesocket(s);
    }
  }
}


/*-------------------------------------------------------------------------*/


/* Check for timed out connections,
 * and close them.
 */
static void mb_timeout(uint32_t cur_t)
{
  MB_CONNECTION *conn, *next;

  /* run through the list of connections */
  next = FIRST_LIST_ELEM(&connListRoot_g, MB_CONNECTION*);
  while (!END_OF_LIST(&connListRoot_g, next))
  {
    conn = next;
    next = NEXT_LIST_ELEM(conn, MB_CONNECTION*);

    if ((cur_t - conn->lastTrans) > MB_TIMEOUT)
    {
      mb_destroyConnection(conn);
    }
  }
}


/*-------------------------------------------------------------------------*/


/* create a new server socket that listens to incomming FTP requests.
 */
static int mb_newServerSocket(void)
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
  addr.sin_port        = port_g;	                         //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

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

  mb_addSocket(s);

  return s;
}


/*-------------------------------------------------------------------------*/


/* This function tries to remove all faulty
 * file handles from the file sets.
 */
static int mb_fdSanity(void)
{
  struct timeval tv = {0,0};
  MB_CONNECTION   *conn, *nextc;
  fd_set         fds;
//  char           buf;

  /* Test if the server socket has failed.
     If so, destroy the socket and create a new one. */
  FD_ZERO(&fds);
  FD_SET(serverSocket_g, &fds); 
  if (select(serverSocket_g, &fds, NULL, NULL, &tv) < 0)
  {
    FD_CLR(serverSocket_g, &globalSocketSet);
    closesocket(serverSocket_g);
    serverSocket_g = mb_newServerSocket();
    if (serverSocket_g < 0)
      return -1;
  }
 
  /* close all faulty connections */
  nextc = FIRST_LIST_ELEM(&connListRoot_g, MB_CONNECTION*);
  while (!END_OF_LIST(&connListRoot_g, nextc))
  {
    conn = nextc;
    nextc = NEXT_LIST_ELEM(conn, MB_CONNECTION*);
//
//    if ((recv(conn->sock, &buf, 0, 0) < 0) &&
//        (sysGetErrno() == EBADF))
//    {
      mb_destroyConnection(conn);
//    }
  }

  return 0;
}




/* main function. does all initialization.
 */
void ModbusTCPTask(void *pvParameters)
{
  int          i;
  struct timeval timeout;
  volatile uint32_t  cur_t;
  fd_set         fds;

  FD_ZERO(&globalSocketSet);
  open_connections_g = 0;

  port_g = *(uint16_t *)pvParameters;

  do
  {
	serverSocket_g = mb_newServerSocket();
	delay(1000);
  }while(serverSocket_g < 0);


  ///* init lists 
  INIT_LIST_HEAD(&connListRoot_g);					 


  timeout.tv_sec  = 2;
  timeout.tv_usec = 100000;

  for(;;)
  {
  	vTaskDelay(10);
    ///* load fds 
    fds = globalSocketSet;

    i = select(FD_SETSIZE, &fds, NULL, NULL, &timeout);

    if (i < 0)
    {
      if (sysGetErrno() == EBADF)
      {
       // /* test for insane file descriptors 
        mb_fdSanity();
      }
      else
      if (sysGetErrno() != EINTR)
      {
       // //sysPrintErr("select() failed\n");
      }
      continue;
    }

	cur_t = (uint32_t)(xTaskGetTickCount() / 1000);
    mb_timeout(cur_t);

    if (i > 0)
    {

      ///* ... handle all connections
      mb_processConnections(&fds, i);

     // /* check for incomming connections 
      if (FD_ISSET(serverSocket_g, &fds))
      {
        mb_acceptClient();
        if (serverSocket_g < 0)
          mb_fdSanity();
        i--;
      }
    }
  }
}


#endif
