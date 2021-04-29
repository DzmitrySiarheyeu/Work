#ifndef USBCDC_STM32F10X_H
#define USBCDC_STM32F10X_H
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

// ����� ������ - ��������
typedef struct buf_tx_st  
{
  uint16_t in;
  uint16_t out;
  uint8_t buf[USBCDC_BUF_TX_SIZE];
}USBCDC_TX_BUF;

typedef struct buf_rx_st  
{
  uint16_t in;
  uint16_t out;
  uint8_t buf[USBCDC_BUF_RX_SIZE];
}USBCDC_RX_BUF;


typedef struct __attribute__ ((__packed__)) tagUARTData
{
    uint32_t tx_count;
    uint32_t rx_count;
    uint16_t DataPoint;
}USBCDCData;

// ������ USBCDC
typedef struct tagUSBCDC_INFO_STM32F1
{
    uint8_t USBCDC_ID;
    USBCDC_RX_BUF rx_buf;
    USBCDC_TX_BUF tx_buf;

	uint32_t time_out_value;
	uint8_t time_out;
	uint8_t busy;
	uint8_t tx_full;
    uint8_t tx_empty;
	volatile uint8_t overflow;
	volatile uint8_t waitRxBufferFree;

	#if(_MULTITASKING_)
      xSemaphoreHandle Sem_rx;
      xSemaphoreHandle Sem_tx;
	#else
      uint8_t Sem_rx;
      uint8_t Sem_tx;
	#endif

}USBCDC_INFO_STM32F1;

extern DEVICE usbcdc_device;

#ifdef __cplusplus
}
#endif

#endif
#endif
