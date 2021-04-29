#ifndef USBCDC_LPC17XX_H
#define USBCDC_LPC17XX_H
#include "Config.h"
#if(_USBCDC_)


#include <stdint.h>
#include "DriversCore.h"
#include "FreeRTOS.h"
#include "semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_USBCDC_DATA_OFFSET 		30

#define USBCDC_CONNECTION_STATUS	100
#define USBCDC_RX_DATA_COUNT        101
#define USBCDC_TX_FREE_SPACE        102

// Буфер приема - передачи
typedef struct buf_st  
{
  uint16_t in;
  uint16_t out;
  uint8_t buf[USBCDC_BUF_SIZE];
}USBCDC_BUF;


typedef struct __attribute__ ((__packed__)) tagUARTData
{
    uint32_t tx_count;
    uint32_t rx_count;
    uint16_t DataPoint;
}USBCDCData;

// Объект USBCDC
typedef struct tagUSBCDC_INFO_LPC17XX
{
    uint8_t USBCDC_ID;
    USBCDC_BUF rx_buf;
    USBCDC_BUF tx_buf;

	uint32_t time_out_value;
	uint8_t time_out;
	uint8_t busy;
	uint8_t tx_full;
    uint8_t tx_empty;
	volatile uint8_t overflow;

	#if(_MULTITASKING_)
      xSemaphoreHandle Sem_rx;
      xSemaphoreHandle Sem_tx;
	#else
      uint8_t Sem_rx;
      uint8_t Sem_tx;
	#endif

}USBCDC_INFO_LPC17XX;

extern DEVICE usbcdc_device;

#ifdef __cplusplus
}
#endif

#endif
#endif
