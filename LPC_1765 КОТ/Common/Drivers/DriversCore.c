#include "Config.h"
#include <stdlib.h>
#include <string.h>
#include "DriversCore.h"


DEVICE_LIST device_list = { .NumberOfItems = 0, .pFirstDevice = NULL };


#if(_UART0_)
	DRIVER_LIST_ITEM uart0_item;
#endif
#if(_UART1_)
    DRIVER_LIST_ITEM uart1_item;
#endif
#if(_UART2_)
    DRIVER_LIST_ITEM uart2_item;
#endif
#if(_UART3_)
    DRIVER_LIST_ITEM uart3_item;
#endif
#if(_I2C0_)
	DRIVER_LIST_ITEM i2c0_item;
#endif
#if(_I2C1_)
	DRIVER_LIST_ITEM i2c1_item;
#endif
#if(_I2C2_)
	DRIVER_LIST_ITEM i2c2_item;
#endif
#if(_SPI_)
	DRIVER_LIST_ITEM spi_item;
#endif
#if(_SPI1_)
	DRIVER_LIST_ITEM spi1_item;
#endif
#if(_SSP0_)
	DRIVER_LIST_ITEM ssp0_item;
#endif
#if(_SSP1_)
	DRIVER_LIST_ITEM ssp1_item;
#endif
#if(_STATS_)
	DRIVER_LIST_ITEM stats_item;
#endif
#if(_OWI_)
	DRIVER_LIST_ITEM owi_item;
#endif
#if(_ADC_)
	DRIVER_LIST_ITEM adc_item;
#endif
#if(_DISCRETE_)
	DRIVER_LIST_ITEM discrete_item;
#endif
#if(_IMPULSE_)
	DRIVER_LIST_ITEM imp_item;
#endif
#if(_FREQ_)
	DRIVER_LIST_ITEM freq_item;
#endif
#if(_FLASH_)
	DRIVER_LIST_ITEM flash_item;
#endif
#if(_FRAM_)
	DRIVER_LIST_ITEM fram_item;
#endif
#if(_CLOCK_ || _CLOCK_RTC_)
	DRIVER_LIST_ITEM clock_item;
#endif
#if(_MODEM_)
	DRIVER_LIST_ITEM modem_item; //
#endif
#if(_GPS_)
	DRIVER_LIST_ITEM gps_item;
#endif
#if(_USB_)
	DRIVER_LIST_ITEM usb_item;
#endif
#if(_SAVER_)
	DRIVER_LIST_ITEM saver_item;
#endif
#if(_MBS_)				
	DRIVER_LIST_ITEM mbs_item;
#endif
#if(_OMS_)				
	DRIVER_LIST_ITEM oms_item;
#endif
#if(_TCP_REDIRECTOR_)
	DRIVER_LIST_ITEM tcp_item_1;
	DRIVER_LIST_ITEM tcp_item_2;
#endif
#if(_CAN_1_)				
	DRIVER_LIST_ITEM can1_item;
#endif
#if(_CAN_2_)				
	DRIVER_LIST_ITEM can2_item;
#endif
#if(_ADXL_345_)				
	DRIVER_LIST_ITEM accel_item;
#endif
#if(_OMNICOMM_SNIFFER_)
	DRIVER_LIST_ITEM omns_item;
#endif	

void init_VFS(void);

void InitDriverSystem(void)
{


#if(_USB_)
	usb_item.pDevice_io = (DEVICE_IO *)&usb_device_io;
	usb_item.pNext = NULL;
    AddDriver(&usb_item);
#endif
	
#if(_UART0_)
	uart0_item.pDevice_io = (DEVICE_IO *)&uart0_device_io;
	uart0_item.pNext = NULL;
    AddDriver(&uart0_item);
#endif

#if(_UART1_)
    uart1_item.pDevice_io = (DEVICE_IO *)&uart1_device_io;
	uart1_item.pNext = NULL;
    AddDriver(&uart1_item);
#endif

#if(_UART2_)
    uart2_item.pDevice_io = (DEVICE_IO *)&uart2_device_io;
	uart2_item.pNext = NULL;
    AddDriver(&uart2_item);
#endif

#if(_UART3_)
    uart3_item.pDevice_io = (DEVICE_IO *)&uart3_device_io;
	uart3_item.pNext = NULL;
    AddDriver(&uart3_item);
#endif

#if(_I2C0_)
	i2c0_item.pDevice_io = (DEVICE_IO *)&i2c0_device_io;
	i2c0_item.pNext = NULL;
    AddDriver(&i2c0_item);
#endif

#if(_I2C1_)
	i2c1_item.pDevice_io = (DEVICE_IO *)&i2c1_device_io;
	i2c1_item.pNext = NULL;
    AddDriver(&i2c1_item);
#endif

#if(_I2C2_)
	i2c2_item.pDevice_io = (DEVICE_IO *)&i2c2_device_io;
	i2c2_item.pNext = NULL;
    AddDriver(&i2c2_item);
#endif

#if(_SPI_)
	spi_item.pDevice_io = (DEVICE_IO *)&spi_device_io;
	spi_item.pNext = NULL;
    AddDriver(&spi_item);
#endif

#if(_SPI1_)
	spi1_item.pDevice_io = (DEVICE_IO *)&spi1_device_io;
	spi1_item.pNext = NULL;
    AddDriver(&spi1_item);
#endif

#if(_SSP0_)
	ssp0_item.pDevice_io = (DEVICE_IO *)&ssp0_device_io;
	ssp0_item.pNext = NULL;
	AddDriver(&ssp0_item);
#endif

#if(_SSP1_)
	ssp1_item.pDevice_io = (DEVICE_IO *)&ssp1_device_io;
	ssp1_item.pNext = NULL;
	AddDriver(&ssp1_item);
#endif

#if(_STATS_)
	stats_item.pDevice_io = (DEVICE_IO *)&stats_device_io;
	stats_item.pNext = NULL;
    AddDriver(&stats_item);
#endif

#if(_OWI_)
	owi_item.pDevice_io = (DEVICE_IO *)&OWI_device_io;
	owi_item.pNext = NULL;
    AddDriver(&owi_item);
#endif

#if(_ADC_)
	adc_item.pDevice_io = (DEVICE_IO *)&ADC_device_io;
	adc_item.pNext = NULL;
    AddDriver(&adc_item);
#endif

#if(_DISCRETE_)
	discrete_item.pDevice_io = (DEVICE_IO *)&discrete_device_io;
	discrete_item.pNext = NULL;
    AddDriver(&discrete_item);
#endif

#if(_IMPULSE_)
	imp_item.pDevice_io = (DEVICE_IO *)&Imp_device_io;
	imp_item.pNext = NULL;
    AddDriver(&imp_item);
#endif

#if(_FREQ_)
	freq_item.pDevice_io = (DEVICE_IO *)&freq_device_io;
	freq_item.pNext = NULL;
    AddDriver(&freq_item);
#endif

#if(_ADXL_345_)
	accel_item.pDevice_io = (DEVICE_IO *)&accel_device_io;
	accel_item.pNext = NULL;
    AddDriver(&accel_item);
#endif

#if(_FLASH_)
	flash_item.pDevice_io = (DEVICE_IO *)&flash_device_io;
	flash_item.pNext = NULL;
    AddDriver(&flash_item);
	delay(10);
	init_VFS();
#endif

#if(_FRAM_)
	fram_item.pDevice_io = (DEVICE_IO *)&fram_device_io;
	fram_item.pNext = NULL;
    AddDriver(&fram_item);
#endif

#if(_CLOCK_ || _CLOCK_RTC_)
	clock_item.pDevice_io = (DEVICE_IO *)&clock_device_io;
	clock_item.pNext = NULL;
    AddDriver(&clock_item);
#endif

#if(_MODEM_)
	modem_item.pDevice_io = (DEVICE_IO *)&modem_device_io;
	modem_item.pNext = NULL;
    AddDriver(&modem_item); 
#endif

#if(_MBS_)
	mbs_item.pDevice_io = (DEVICE_IO *)&MBS_device_io;
	mbs_item.pNext = NULL;
	AddDriver(&mbs_item);
#endif
	 
#if(_OMS_)
	oms_item.pDevice_io = (DEVICE_IO *)&OMS_device_io;
	oms_item.pNext = NULL;
	AddDriver(&oms_item);
#endif

#if(_GPS_)
	gps_item.pDevice_io = (DEVICE_IO *)&gps_device_io;
	gps_item.pNext = NULL;
    AddDriver(&gps_item);
#endif

#if(_SAVER_)
	saver_item.pDevice_io = (DEVICE_IO *)&saver_device_io;
	saver_item.pNext = NULL;
	AddDriver(&saver_item);
#endif
#if(_TCP_REDIRECTOR_)
	tcp_item_1.pDevice_io = (DEVICE_IO *)&tcp_device_1_io;
	tcp_item_1.pNext = NULL;
	AddDriver(&tcp_item_1);
	tcp_item_2.pDevice_io = (DEVICE_IO *)&tcp_device_2_io;
	tcp_item_2.pNext = NULL;
	AddDriver(&tcp_item_2);
#endif
#if(_CAN_1_)
	can1_item.pDevice_io = (DEVICE_IO *)&can1_device_io;
	can1_item.pNext = NULL;
    AddDriver(&can1_item);
#endif
#if(_CAN_2_)
	can2_item.pDevice_io = (DEVICE_IO *)&can2_device_io;
	can2_item.pNext = NULL;
    AddDriver(&can2_item);
#endif
//#if(_ADXL_345_)
//	can2_item.pDevice_io = (DEVICE_IO *)&accel_device_io;
//	can2_item.pNext = NULL;
//    AddDriver(&accel_item);
//#endif
#if(_OMNICOMM_SNIFFER_)
	omns_item.pDevice_io = (DEVICE_IO *)&omns_device_io;
	omns_item.pNext = NULL;
    AddDriver(&omns_item);
#endif
}

int ReadDevice(DEVICE_HANDLE handle, void *pDst, int count)
{
    if(handle == NULL) return -1;
    return ((DEVICE_IO *)((DEVICE *)handle)->device_io)->ReadDevice(handle, pDst, count);
}

int WriteDevice(DEVICE_HANDLE handle, const void *pDst, int count)
{
    if(handle == NULL) return -1;
    return ((DEVICE_IO *)((DEVICE *)handle)->device_io)->WriteDevice(handle, pDst, count);
}
int SeekDevice(DEVICE_HANDLE handle, uint32_t offset, uint8_t origin)
{
    if(handle == NULL) return -1;
    return ((DEVICE_IO *)((DEVICE *)handle)->device_io)->SeekDevice(handle, offset, origin);
}

int WriteDeviceLn (DEVICE_HANDLE handle, const char *str) {
   return WriteDevice(handle, str, strlen(str));
}

int ReadDeviceLnEx(DEVICE_HANDLE handle, char *buf, int len, uint32_t delay)
{
	uint32_t timeout = 0, count = 0;
	if(handle == NULL) return 0;
	DeviceIOCtl(handle, GET_RX_TIMEOUT, (uint8_t *)&timeout);
	DeviceIOCtl(handle, SET_RX_TIMEOUT, (uint8_t *)&delay);
	memset(buf, 0, len);
	len--; //Последний символ в любом случае должен быть 0 - конец строки
	do
	{
		count += ReadDevice(handle, buf + count, len - count);
		if(count == 0)
				break;
		while(*buf == '\r' || *buf == '\n')
		{
			memmove(buf, buf + 1, len); //len - 1 уже учтено!!!!
			if(count > 0) count--;
		}
		if(count != 0 && buf[count - 1] == '\n')
			break;
		DeviceIOCtl(handle, GET_STATUS_DEVICE, (uint8_t *)&delay);
	}while(delay != DEVICE_STATUS_TIMEOUT && count < len);
	DeviceIOCtl(handle, SET_RX_TIMEOUT, (uint8_t *)&timeout);
	return count;
}

int ReadDeviceText(DEVICE_HANDLE handle, void *pDst, int count)
{
    if(handle == NULL) return -1;
    return ((DEVICE_IO *)((DEVICE *)handle)->device_io)->ReadDeviceText(handle, pDst, count);
}

int WriteDeviceText(DEVICE_HANDLE handle, void *pSrc, int count)
{
    if(handle == NULL) return -1;
    return ((DEVICE_IO *)((DEVICE *)handle)->device_io)->WriteDeviceText(handle, pSrc, count);
}

int SeekDeviceText(DEVICE_HANDLE handle, uint32_t offset, uint8_t origin)
{
    if(handle == NULL) return -1;
    return ((DEVICE_IO *)((DEVICE *)handle)->device_io)->SeekDeviceText(handle, offset, origin);
}

int DeviceIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData)
{
    if(handle == NULL) return -1;
    return ((DEVICE_IO *)((DEVICE *)handle)->device_io)->DeviceIOCtl(handle, code, pData);
}

void CloseDevice(DEVICE_HANDLE handle)
{
    if(handle == NULL) return;
    return ((DEVICE_IO *)((DEVICE *)handle)->device_io)->CloseDevice(handle);
}

int AddDriver(DRIVER_LIST_ITEM *pItem)
{	
	int err = 0;
	err = ((DEVICE_IO *)(pItem->pDevice_io))->CreateDevice();   // Создаем девайс, на который указывает текущий элемент (инициализация железа по сути)

	if(err == DEVICE_ERROR) return DEVICE_ERROR;
	device_list.NumberOfItems++;	      //  Наращиваем количество добавленных элементов

    if(device_list.pFirstDevice == NULL)  // Если вносим первый элемент в список
    {
        device_list.pFirstDevice = pItem;  // Указатель первого элемента устанавливаем на этот элемент
        pItem->pNext = NULL;	           // Указатель в элементе на следующий элемент обнуляем
    }
    else		   // Если список уже содержит элементы
    {
        pItem->pNext = device_list.pFirstDevice;  // Помещаем элемен в начало, присваивая адрес предыдущего элемента (бывшего первого в списке) его указателю на следующий элемент 
        device_list.pFirstDevice = pItem;	      // Помечаем что этот элемент - первый элемент в списке
    }
    return DEVICE_OK;
}

DEVICE_HANDLE OpenDevice(char *name)
{
    DRIVER_LIST_ITEM *pItem;
    pItem = device_list.pFirstDevice;
    DEVICE_IO *pDevice_io;
    while(pItem != NULL)						//  Циклемся пока в списке есть элементы
    {
        pDevice_io = (DEVICE_IO *)pItem->pDevice_io;		  // Вытаскиваем с элемента указатель на объект девайса
        if(strcmp(pDevice_io->DeviceName, name) == 0)	  // Сравниваем требуемое имя с именем девайса извлеченного из данного элемента
        {
           
            return pDevice_io->OpenDevice();	   // Если нашли требуемый девайс, открываем его и возвращаем результат фуекции открытия (хендл девайса)

        }
        pItem = pItem->pNext;
    }
    return NULL;
}

int IsDevicePresent(char *name)
{
    DRIVER_LIST_ITEM *pItem;
    pItem = device_list.pFirstDevice;
    DEVICE_IO *pDevice_io;
    while(pItem != NULL)						//  Циклемся пока в списке есть элементы
    {
        pDevice_io = (DEVICE_IO *)pItem->pDevice_io;		  // Вытаскиваем с элемента указатель на объект девайса
        if(strcmp(pDevice_io->DeviceName, name) == 0)	  // Сравниваем требуемое имя с именем девайса извлеченного из данного элемента
        {
           
            return 1;

        }
        pItem = pItem->pNext;
    }
    return 0;
}


