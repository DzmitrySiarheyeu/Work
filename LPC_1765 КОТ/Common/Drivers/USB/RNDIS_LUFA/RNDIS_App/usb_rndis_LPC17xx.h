#ifndef USBCDC_LPC17XX_H
#define USBCDC_LPC17XX_H
#include "Config.h"
#if(_USB_)


#include <stdint.h>
#include "DriversCore.h"
#include "FreeRTOS.h"
#include "semphr.h"

#include "USB.h"
#include "Descriptors.h"
		

#ifdef __cplusplus
extern "C" {
#endif


// Объект USB
typedef struct tagUSB_INFO_LPC17XX
{
	uint8_t buf[USB_BUF_RX_SIZE];
	uint16_t tot_rx_size; 
	uint16_t cur_rx_cnt;
	uint8_t rndis_err;

	#if(_MULTITASKING_)
	  xSemaphoreHandle Sem_dev;
	#else
      uint8_t Sem_rx;
	#endif

}USB_INFO_LPC17XX;

extern DEVICE usb_device;

/* External Variables: */
extern USB_ClassInfo_RNDIS_Device_t Ethernet_RNDIS_Interface_Device;

	/* Function Prototypes: */
void USBDeviceMode_USBTask(void);

void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);

#ifdef __cplusplus
}
#endif

#endif
#endif
