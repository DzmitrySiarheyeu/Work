#ifndef CONFIG_H
#define CONFIG_H

#include "LPC11xx.h"
#include "loader.h"

//=============================================================================
// Error Codes

	#define PO_VERSION      (uint8_t)(100)  // Версия 1,00 * 100

	#define ERROR_OK 0x11
	#define ERROR_CRC 0x22

/************************UART*************************/

#define UART0 			(0)
#define UART1 			(1)
#define UART2 			(2)
#define UART3 			(3)

#define UART_DEVICE 	(UART1)
#define UART_PORT	    (2)		       	
#define UART_RXD_PIN	(0)		
#define UART_TXD_PIN	(1)	

#define USE_RS485_CONTROL		(1)

/************************WDT**************************/	

/*****************EXTERNAL BOOT REQUEST***************/	


/**********************USER PROGRAM*******************/	

// Количество секторов FLASH на данном микроконтроллере.
// Зависит от объема памяти:
// 32  kB - 7
// 64  kB - 15
// 128 kB - 17
// 256 kB - 21
// 512 kB - 29

#define MAX_USER_SECTOR (6)

extern char rxBuffer[BUFFER_SIZE];

#endif
