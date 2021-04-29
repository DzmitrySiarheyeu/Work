#include "Config.h"
#if(_USB_)

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "usb_rndis_lpc17xx.h"
#include "debug_system.h"


static int CreateUSB(void);
static DEVICE_HANDLE OpenUSB(void);
static void CloseUSB(DEVICE_HANDLE handle);
static int ReadUSB(DEVICE_HANDLE handle, void * dst, int count);
static int WriteUSB(DEVICE_HANDLE handle, void * dst, int count);
static int SeekUSB(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
static int WriteUSBText(DEVICE_HANDLE handle, void * pSrc, int count);
static int ReadUSBText(DEVICE_HANDLE handle, void * pDst, int count);
static int SeekUSBText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin);
static int USBIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData);

static uint16_t usb_rx_count(DEVICE_HANDLE handle);

USB_INFO_LPC17XX usb_info = {
								 .cur_rx_cnt = 0,
								 .rndis_err = 0,
								 .Sem_dev = NULL, 
								 .tot_rx_size = 0
								 };


const DEVICE_IO usb_device_io = {
    .DeviceID = USB,
    .OpenDevice = OpenUSB,
	.CreateDevice =	CreateUSB,
	.CloseDevice = CloseUSB,
    .ReadDevice = ReadUSB,
    .WriteDevice = WriteUSB,
    .SeekDevice = SeekUSB,
    .ReadDeviceText = WriteUSBText,
    .WriteDeviceText = ReadUSBText,
    .SeekDeviceText = SeekUSBText,
    .DeviceIOCtl = USBIOCtl,
    .DeviceName = "USB",
    };

DEVICE usb_device = {
    .device_io = (DEVICE_IO *)&usb_device_io,
    .flDeviceOpen = 0,
    .pDriverData = &usb_info,
    .pData = NULL
    };


USB_ClassInfo_RNDIS_Device_t Ethernet_RNDIS_Interface_Device =
	{
		.Config =
			{
				.ControlInterfaceNumber         = 0,

				.DataINEndpointNumber = CDC_TX_EPADDR,/**< Endpoint number of the RNDIS interface's IN data endpoint. */
				.DataINEndpointSize = CDC_TXRX_EPSIZE, /**< Size in bytes of the RNDIS interface's IN data endpoint. */
				.DataINEndpointDoubleBank = 1, /**< Indicates if the RNDIS interface's IN data endpoint should use double banking. */

				.DataOUTEndpointNumber = CDC_RX_EPADDR, /**< Endpoint number of the RNDIS interface's OUT data endpoint. */
				.DataOUTEndpointSize = CDC_TXRX_EPSIZE,  /**< Size in bytes of the RNDIS interface's OUT data endpoint. */
				.DataOUTEndpointDoubleBank = 1, /**< Indicates if the RNDIS interface's OUT data endpoint should use double banking. */

				.NotificationEndpointNumber = CDC_NOTIFICATION_EPADDR, /**< Endpoint number of the RNDIS interface's IN notification endpoint, if used. */
				.NotificationEndpointSize = CDC_NOTIFICATION_EPSIZE,  /**< Size in bytes of the RNDIS interface's IN notification endpoint, if used. */
				.NotificationEndpointDoubleBank = 1, /**< Indicates if the RNDIS interface's notification endpoint should use double banking. */

				.AdapterVendorDescription = "TWIGGY RNDIS", /**< String description of the adapter vendor. */
				.AdapterMACAddress = {{0x12, 0x1A, 0x01, 0xC1, 0x1F, 0x00}},
			},
	};

static int CreateUSB(void)
{
    USB_Init();


	vSemaphoreCreateBinary(usb_info.Sem_dev);
	if(usb_info.Sem_dev == NULL)
	{
		DEBUG_PUTS("USB: Semaphor_dev = NULL\n");
		return DEVICE_ERROR;
	}

    return DEVICE_OK; // true
}

static DEVICE_HANDLE OpenUSB(void)
{
	take_sem(usb_info.Sem_dev, portMAX_DELAY);
	usb_device.flDeviceOpen = DEVICE_OPEN;
	return 	(DEVICE_HANDLE)&usb_device;
}

static void CloseUSB(DEVICE_HANDLE handle)
{
	DEVICE *pDevice = (DEVICE *)handle;
	USB_INFO_LPC17XX *pUsbInfo = (USB_INFO_LPC17XX *)(pDevice->pDriverData);
	pDevice->flDeviceOpen = DEVICE_CLOSE;
	give_sem(pUsbInfo->Sem_dev);
}


static int WriteUSB (DEVICE_HANDLE handle, void * pSrc, int count)
{
    uint8_t err = 0;
	err = RNDIS_Device_SendPacket(&Ethernet_RNDIS_Interface_Device, pSrc, count);

	if(err != ENDPOINT_RWSTREAM_NoError){
		return 0;
	}
	return count;
}

static int ReadUSB(DEVICE_HANDLE handle, void * pDst, int count) {
 
   	USB_INFO_LPC17XX *pUSBInfo = (USB_INFO_LPC17XX *)(((DEVICE *)handle)->pDriverData);
	if(pUSBInfo->rndis_err != ENDPOINT_RWSTREAM_NoError) return 0;

	if((pUSBInfo->cur_rx_cnt + count) > pUSBInfo->tot_rx_size) count = pUSBInfo->tot_rx_size - pUSBInfo->cur_rx_cnt;

	memcpy(pDst, (pUSBInfo->buf + pUSBInfo->cur_rx_cnt), count);
	pUSBInfo->cur_rx_cnt += count;
	return count;
}


static int SeekUSB(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
    return -1;
}

static int USBIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData)
{

    switch(code)
	{
		case RX_DATA_COUNT:
			*((uint16_t *)pData) = usb_rx_count(handle);
            break;

        case POOL:
			USBDeviceMode_USBTask();
			USB_USBTask();
            break;
		case FLUSH_DEVICE:
	//		Endpoint_ClearIN();
            break;

		default: 
			return -1;
	}

	return -1;
}

static int WriteUSBText(DEVICE_HANDLE handle, void * pSrc, int count)
{
#if(_TEXT_)
	return -1;
#else
    return -1;
#endif
}

static int ReadUSBText(DEVICE_HANDLE handle, void * pDst, int count)
{
#if(_TEXT_)
    return -1;
#else
	return -1;
#endif
}

static int SeekUSBText(DEVICE_HANDLE handle, uint32_t  offset, uint8_t origin)
{
#if(_TEXT_)
    return -1;
#else
	return -1;
#endif
}

static uint16_t usb_rx_count(DEVICE_HANDLE handle)
{
	USB_INFO_LPC17XX *pUSBInfo = (USB_INFO_LPC17XX *)(((DEVICE *)handle)->pDriverData);
	

	if(RNDIS_Device_IsPacketReceived(&Ethernet_RNDIS_Interface_Device) == false){
		return 0;
	}


	uint16_t len = 0;
	pUSBInfo->rndis_err = RNDIS_Device_ReadPacket(&Ethernet_RNDIS_Interface_Device, pUSBInfo->buf, &len);
	pUSBInfo->cur_rx_cnt = 0;

	if(pUSBInfo->rndis_err != ENDPOINT_RWSTREAM_NoError){
		return 0;
	}

	pUSBInfo->tot_rx_size = len;
	return len;	
}

void USBDeviceMode_USBTask(void)
{
	RNDIS_Device_USBTask(&Ethernet_RNDIS_Interface_Device);
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	//USBConfigureInterface(NULL);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	//USBInterfaceDown();
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	RNDIS_Device_ConfigureEndpoints(&Ethernet_RNDIS_Interface_Device);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
//	RNDIS_Device_ProcessControlRequest(&Ethernet_RNDIS_Interface_Device);
}

#endif
