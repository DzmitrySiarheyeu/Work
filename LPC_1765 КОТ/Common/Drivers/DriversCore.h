#ifndef DRIVERS_CORE_H
#define DRIVERS_CORE_H

#include <stdint.h>

#define __FALSE 0
#define __TRUE  1

// Коды ошибок
#define DEVICE_OK     0
#define DEVICE_ERROR  1
#define DEVICE_OPEN   1
#define DEVICE_CLOSE  0
#define DEVICE_TASK_PRIORITY_LOW  ( tskIDLE_PRIORITY + 1 )
#define DEVICE_TASK_PRIORITY_HIGH  ( tskIDLE_PRIORITY + 10 )

// Коды статуса
#define DEVICE_STATUS_TIMEOUT					1
#define DEVICE_STATUS_CH_TIMEOUT			2

// Коды устройств
#define UART_0  0x23
#define UART_1  0x24
#define UART_2  0x25
#define UART_3  0x26
#define MODEM   0x1A
#define GPS     0x1E
#define SPI     0x27
#define SSP_0   0x28     

#define SPI_1   0x29
#define SSP_1   0x2A
#define FLASH_1 0x19
							   
#define I2C_0   0x16
#define I2C_1   0x17
#define I2C_2   0x18
#define CLOCK   0x28
#define FREQ    0x30
#define ADC     0x40
#define IMP     0x50
#define DISCRETE 	0x60
#define STATS   0x63
#define USB		0x70
#define OWI     0x80
#define SAVER   0x90
#define MBS	    0x95
#define TCP_DEV 0x96
#define OMS	    0x97
#define CAN     0x98
#define OMNS	0x9A
#define ACCEL   0x9B

// Максимальная длина имени устройства
#define DEVICE_NAME_MAX_LENGHT  10

/*****************************************************/
/*****************************************************/
/********ГЛОБАЛЬНЫЕ ДЭФАЙНЫ ДЛЯ ВСЕХ ДРАЙВЕРОВ********/
/*****************************************************/
/*****************************************************/

typedef enum _STANDARD_IOCTL_REQUESTS
{
	POOL = 200,
	SET_READ_MODE,       
	READ_WITH_SHIFT,     
	READ_ONLY,           
	RX_DATA_COUNT,
	FREE_TX_SPACE,	    
	DEVICE_CONFIG,	    
	READ_ALL_DATA,		
	FLUSH_DEVICE,	
	DEVICE_VALID_CHECK,
	DEVICE_PRE_CLOSE,
	SET_RX_TIMEOUT,	
	GET_RX_TIMEOUT,    
	SET_DEF_BAUDRATE,
	SET_BAUDRATE,	
	SET_VALID_DATA,
	READ_LATEST,	
	RESET_DEVICE,		
	SNIF_START,		
	SNIF_STOP,		
	GET_STATUS_DEVICE,
	SET_OMNI_DEVICE
}STANDARD_IOCTL_REQUESTS;

#ifdef __cplusplus
extern "C" {
#endif

typedef void * DEVICE_HANDLE;

// Структура описывающая список устройств
typedef struct tagDeviceList
{
    uint8_t NumberOfItems;
    struct tagListItem *pFirstDevice;
}DEVICE_LIST;

typedef int (*DEVICE_CREATE_FUNC)(void);
typedef void (*DEVICE_CLOSE_FUNC)(DEVICE_HANDLE handle);
typedef DEVICE_HANDLE (*DEVICE_OPEN_FUNC)(void);
typedef int (*DEVICE_READ_FUNC)(DEVICE_HANDLE handle, void *pDst, int count);
typedef int (*DEVICE_WRITE_FUNC)(DEVICE_HANDLE handle, const void *pSrc, int count);
typedef int (*DEVICE_SEEK)(DEVICE_HANDLE handle, uint32_t offset, uint8_t origin);
typedef int (*DEVICE_READ_TEXT)(DEVICE_HANDLE handle, void *pDst, int count);
typedef int (*DEVICE_WRITE_TEXT)(DEVICE_HANDLE handle, void *pSrc, int count);
typedef int (*DEVICE_SEEK_TEXT)(DEVICE_HANDLE handle, uint32_t offset, uint8_t origin);
typedef int (*DEVICE_IOCTL)(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData);

typedef struct tagDeviceIO
{
    uint8_t DeviceID;                  //   Идентификатор устроцства
	DEVICE_CREATE_FUNC CreateDevice;
	DEVICE_CLOSE_FUNC CloseDevice;
    DEVICE_OPEN_FUNC OpenDevice;       //   Функция открытия устройства
    DEVICE_READ_FUNC ReadDevice;       //   Функция чтения данных из устройства
    DEVICE_WRITE_FUNC WriteDevice;     //   Функция записи данных в устройство
    DEVICE_SEEK SeekDevice;            //   Функция установки указателя на записывыемые или считываемые данные
    DEVICE_READ_TEXT ReadDeviceText;
    DEVICE_WRITE_TEXT WriteDeviceText;
    DEVICE_SEEK_TEXT  SeekDeviceText;
    DEVICE_IOCTL DeviceIOCtl;
    char DeviceName[ DEVICE_NAME_MAX_LENGHT ];     //   Имя устройства
}DEVICE_IO;

typedef struct tagDevice
{
    uint8_t flDeviceOpen;                          //   Флаг открытия устройства
    DEVICE_IO *device_io;        // Указатель на структуру ввода-вывода устройства
    void *pDriverData;                             //   Указатель на драйвер данного устройства
    void *pData;                                   //   Указатель на данные устройства
}DEVICE;

typedef struct tagListItem
{
	struct tagDeviceIO *pDevice_io;
	struct tagListItem *pNext;
}DRIVER_LIST_ITEM;

DEVICE_HANDLE OpenDevice(char *name);
void CloseDevice(DEVICE_HANDLE handle);
int ReadDevice(DEVICE_HANDLE handle, void *pDst, int count);
int WriteDevice(DEVICE_HANDLE handle, const void *pDst, int count);
int WriteDeviceLn (DEVICE_HANDLE handle, const char *str);
int ReadDeviceLnEx(DEVICE_HANDLE handle, char *buf, int len, uint32_t delay);
int SeekDevice(DEVICE_HANDLE handle, uint32_t offset, uint8_t origin);
int ReadDeviceText(DEVICE_HANDLE handle, void *pSrc, int count);
int WriteDeviceText(DEVICE_HANDLE handle, void *pSrc, int count);
int SeekDeviceText(DEVICE_HANDLE handle, uint32_t offset, uint8_t origin);
int DeviceIOCtl(DEVICE_HANDLE handle, uint8_t code, uint8_t *pData);

int AddDriver(DRIVER_LIST_ITEM *pDevice_io);
void InitDriverSystem(void);
#if(_UART0_)																   
    const extern DEVICE_IO uart0_device_io;
#endif
#if(_UART1_)
    const extern DEVICE_IO uart1_device_io;
#endif
#if(_UART2_)
    const extern DEVICE_IO uart2_device_io;
#endif
#if(_UART3_)
    const extern DEVICE_IO uart3_device_io;
#endif
#if(_FREQ_)
    const extern DEVICE_IO freq_device_io;
#endif
#if(_IMPULSE_)
    const extern DEVICE_IO Imp_device_io;
#endif
#if(_I2C0_)
    const extern DEVICE_IO i2c0_device_io;
#endif
#if(_I2C1_)
    const extern DEVICE_IO i2c1_device_io;
#endif
#if(_I2C2_)
    const extern DEVICE_IO i2c2_device_io;
#endif
#if(_MODEM_)
    const extern DEVICE_IO modem_device_io;
#endif
#if(_GPS_)
    const extern DEVICE_IO gps_device_io;
#endif
#if(_CLOCK_ || _CLOCK_RTC_)
    const extern DEVICE_IO clock_device_io;
#endif
#if(_SPI_)
    const extern DEVICE_IO spi_device_io;
#endif
#if(_SPI1_)
    const extern DEVICE_IO spi1_device_io;
#endif
#if(_FRAM_)
    const extern DEVICE_IO fram_device_io;
#endif
#if(_FLASH_)
    const extern DEVICE_IO flash_device_io;
#endif
#if(_ADC_)
	const extern DEVICE_IO ADC_device_io;
#endif
#if(_DISCRETE_)
	const extern DEVICE_IO discrete_device_io;
#endif
#if(_OWI_)
	const extern DEVICE_IO OWI_device_io;
#endif
#if(_STATS_)
	const extern DEVICE_IO stats_device_io;
#endif
#if(_USB_)
	const extern DEVICE_IO usb_device_io;
#endif
#if(_SAVER_)
	const extern DEVICE_IO saver_device_io;
#endif
#if(_SSP0_)
	const extern DEVICE_IO ssp0_device_io;
#endif
#if(_SSP1_)
	const extern DEVICE_IO ssp1_device_io;
#endif

#if(_MBS_)
	const extern DEVICE_IO MBS_device_io;
#endif

#if(_OMS_)
	const extern DEVICE_IO OMS_device_io;
#endif

#if(_TCP_REDIRECTOR_)
	const extern DEVICE_IO tcp_device_1_io;
	const extern DEVICE_IO tcp_device_2_io;
#endif
#if(_CAN_1_)
	const extern DEVICE_IO can1_device_io;
#endif
#if(_CAN_2_)
	const extern DEVICE_IO can2_device_io;
#endif
#if(_ADXL_345_)
	const extern DEVICE_IO accel_device_io;
#endif
#if(_OMNICOMM_SNIFFER_)
	const extern DEVICE_IO omns_device_io;
#endif

#ifdef __cplusplus
}
#endif

#endif
