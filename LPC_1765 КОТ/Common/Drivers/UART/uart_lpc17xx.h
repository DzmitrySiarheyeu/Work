#ifndef UART_LPC17XX_H
#define UART_LPC17XX_H
#include "Config.h"
#if(_UART_)


#include <stdint.h>
#include "DriversCore.h"														   

#ifdef __cplusplus
extern "C" {
#endif

												 
#define MAX_UART_DATA_OFFSET    300

#define UART_STATUS_TIMEOUT					DEVICE_STATUS_TIMEOUT
#define UART_STATUS_CH_TIMEOUT			DEVICE_STATUS_CH_TIMEOUT


/***************************************************************************/

// Буфер приема - передачи
typedef struct  buf_st  {
  uint16_t in;
  uint16_t out;
  uint16_t buf_size;
  uint8_t *buf;
}UART_BUF;


typedef struct __attribute__ ((__packed__)) tagUARTData
{
    uint32_t tx_count;
    uint32_t rx_count;
    uint16_t DataPoint;
}UARTData;

// Объект UART
typedef struct tagUART_INFO_LPC17XX
{
    void  *pUart;
		UART_BUF rx_buf;
    UART_BUF tx_buf;
	  uint32_t def_baudrate_value;
	  //uint32_t def_time_out_value;
	  uint32_t time_out_value;
    volatile uint8_t time_out;
	  uint8_t status;
    volatile uint8_t overflow;
	  #if(_MULTITASKING_)
      xSemaphoreHandle Sem_rx;
      xSemaphoreHandle Sem_tx;
	  #else
      uint8_t Sem_rx;
      uint8_t Sem_tx;
	  #endif
}UART_INFO_LPC17XX;

typedef struct tagConfig_info
{
	uint32_t baud_rate;
	uint8_t  parity;
	uint8_t  databits;
	uint8_t  stopbits;
	uint8_t  fifo_Level;
	uint32_t rx_timeout;
}UART_CONFIG_INFO;

/*--------------------------------------------------------------------------*/
/*-------------Определения для UART_CONFIG_INFO-----------------------------*/
/*--------------------------------------------------------------------------*/

#define		C_UART_STOPBIT_1		0   	/*!< UART 1 Stop Bits Select */
#define		C_UART_STOPBIT_2		1		/*!< UART Two Stop Bits Select */

#define	    C_UART_DATABIT_5		0       /*!< UART 5 bit data mode */
#define		C_UART_DATABIT_6		1     	/*!< UART 6 bit data mode */
#define		C_UART_DATABIT_7		2    	/*!< UART 7 bit data mode */
#define		C_UART_DATABIT_8		3		/*!< UART 8 bit data mode */

#define		C_UART_PARITY_NONE 		0		/*!< No parity */
#define		C_UART_PARITY_ODD	 	1		/*!< Odd parity */
#define		C_UART_PARITY_EVEN 		2		/*!< Even parity */
#define		C_UART_PARITY_SP_1 		3		/*!< Forced "1" stick parity */
#define		C_UART_PARITY_SP_0 		4		/*!< Forced "0" stick parity */

#define		C_UART_FIFO_TRGLEV0	    0		/*!< UART FIFO trigger level 0: 1 character */
#define		C_UART_FIFO_TRGLEV1 	1		/*!< UART FIFO trigger level 1: 4 character */
#define		C_UART_FIFO_TRGLEV2		2		/*!< UART FIFO trigger level 2: 8 character */
#define		C_UART_FIFO_TRGLEV3		3		/*!< UART FIFO trigger level 3: 14 character */

/*--------------------------------------------------------------------------*/
/*---------------------Коды для настройки-----------------------------------*/
/*--------------------------------------------------------------------------*/

//#define SET_BAUDRATE         (0)
#define SET_DATASIZE         (1)
#define SET_PARITY           (2)
#define SET_STOPSIZE         (3)
#define SET_CONTROL          (4)
#define NOTIFY_LINESTATE     (5)
#define NOTIFY_MODEMSTATE    (6)
#define FLOWCONTROL_SUSPEND  (7)
#define FLOWCONTROL_RESUME   (8)
#define SET_LINESTATE_MASK   (9)
#define SET_MODEMSTATE_MASK  (10)
#define PURGE_DATA           (11)

#define GET_BAUDRATE         (0 + 100)
#define GET_DATASIZE         (1 + 100)
#define GET_PARITY           (2 + 100)
#define GET_STOPSIZE         (3 + 100)
#define GET_CONTROL          (4 + 100)
#define RE_INIT				 (5 + 100)
//#define NOTIFY_LINESTATE     (5)
//#define NOTIFY_MODEMSTATE    (6)
//#define FLOWCONTROL_SUSPEND  (7)
//#define FLOWCONTROL_RESUME   (8)
#define GET_LINESTATE_MASK   (9 + 100)
#define GET_MODEMSTATE_MASK  (10 + 100)
//#define PURGE_DATA           (11)

#define GET_DATA_AMOUNT      (0 + 50)
#define DISABLE_RX           (2 + 50)
#define ENABLE_RX            (3 + 50)
#define DISABLE_TX           (4 + 50)
#define ENABLE_TX            (5 + 50)	  

/*---------------------Коды опций для SET_CONTROL--------------------------*/

#define SET_BREAK_ON         (5)
#define SET_BREAK_OFF        (6)

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif
#endif
