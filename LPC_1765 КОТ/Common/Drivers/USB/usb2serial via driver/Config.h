#ifndef CONFIG_H
#define CONFIG_H

#define DEVICE_ON   1
#define DEVICE_OFF  0

#define OPTION_ON	1
#define OPTION_OFF	0

/************************UART*************************/

#define _UART1_ DEVICE_ON

#if(_UART1_ == DEVICE_ON)

	#define _UART1_PORT_	  2      			    
	#define _UART1_RXD_PIN_   0			
	#define _UART1_TXD_PIN_	  1		
	#define _UART1_BAUDRATE_  9600
	
	#define RX_TIME_OUT_VALUE_1	0       //  âðåìÿ â ìèëëèñåêóíäàõ		

#endif // _UART1_

#define UART_BUF_SIZE      		512

#define Uart_1_ISR		UART1_IRQHandler 
 
#define _UART_ 		(_UART1_ || _UART1_ || _UART2_ || _UART3_)

/************************_USBCDC_********************/

#define _USBCDC_ 		OPTION_ON

#if(_USBCDC_ == OPTION_ON)

		#define		USBCDC_BUF_SIZE					512
		#define		USBCDC_RX_TIME_OUT_VALUE		0x0

#endif //  _USBCDC_

/****************************************************/

#define DEBUG_PUTS(a)
#define PUT_MES_TO_LOG(a,b,c)
#define vConfigureTimerForRunTimeStats(a)

#define RX_DATA_COUNT 99
#define FREE_TX_SPACE 98

#define SRAM1  __attribute__ ((section (".new_bss")))


/*************ÏÎÄÄÅÐÆÊÀ ÌÍÎÃÎÇÀÄÀ×ÍÎÑÒÈ***************/

#define _MULTITASKING_  OPTION_ON

#if(_MULTITASKING_ == OPTION_ON)

#include "FreeRTOS.h"
#include "semphr.h"

	#define create_sem(sem)								 vSemaphoreCreateBinary(sem)		
	#define take_sem(sem, block_time) 					 xSemaphoreTake(sem, block_time)
	#define give_sem(sem) 								 xSemaphoreGive(sem)
	#define give_sem_from_isr(sem, higher_pr_task_woken) xSemaphoreGiveFromISR(sem, higher_pr_task_woken)
	#define delay(block_time)                            vTaskDelay(block_time)

#else
	
	#define create_sem(sem)								 sem=1		
	#define take_sem(sem, block_time) 					 1
	#define give_sem(sem) 								 
	#define give_sem_from_isr(sem, higher_pr_task_woken)
	#define delay(block_time) 
	
#endif // _MULTITASKING_			

//#endif

#endif
