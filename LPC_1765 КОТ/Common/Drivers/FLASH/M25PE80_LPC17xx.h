#ifndef _25PE80_LPC23XX_H
#define _25PE80_LPC23XX_H

#include "Config.h"
#if(_FLASH_)

#include "lpc17xx_gpio.h"
#include "DriversCore.h"


#define config_cs()			GPIO_SetDir(_FLASH_CS_PORT_, 1 << _FLASH_CS_PIN_, 1)
#define select_flash()		GPIO_ClearValue(_FLASH_CS_PORT_, 1 << _FLASH_CS_PIN_)
#define unselect_flash()		GPIO_SetValue(_FLASH_CS_PORT_, 1 << _FLASH_CS_PIN_)

#define config_hold()
#define unhold_flash()
#define hold_flash()

// Коды для работы с flash через IOCtl в отрыве от FAT (по умолчанию все заточено под FAT)

#define SWITCH_TO_RAW                   0       //переключиться в режим "чистой" записи на Flash
#define SET_FLASH_ADDR                  1       //SeekFlash не может установить 32-битный адрес
#define ERASE_BLOCK                     2

// Коды операций
#define READ				0x03	// Чтение из FLASH

#define ERASE_PAGE                  	0xDB    // Стирает страницу
#define ERASE_4K_BLOCK                  0x20    // Стирает блок 4 кб
#define ERASE_32K_BLOCK                 0x52    // Стирает блок 32 кб
#define ERASE_64K_BLOCK                 0xD8    // Стирает блок 64 кб
#define CHIP_ERASE                      0x60    // Стирает все
#define BYTE_PAGE_PROGRAM               0x02    // Запись 1-256 байт
#define BYTE_PAGE_WRITE					0x0A


#define WRITE_ENABLE                    0x06    // Разрешить запись
#define WRITE_DISABLE                   0x04    // Запретить запись

#define   READ_STATUS_REGISTR		0x05	// Чтение регистра статуса
#define WRITE_STATUS_REGISTR_B1         0x01    // Используется для снятия/включения глобальной защиты от записи
#define WRITE_STATUS_REGISTR_B2         0x31    // Используется для чего-то

// Коды STATUS_REGISTR

#define FLASH_STATUS_BUSY               (1 << 0)  // Флэш занят

//#define FAT_CLUSTER_SIZE                (4096)
                
typedef struct tagFLASH_INFO
{
      DEVICE_HANDLE *pSPI;
      uint32_t block_num;	         // Номер 4кб блока во флэш (в режиме FAT) / адрес во во флэш (в режиме RAW)
      uint32_t page_num;                 // Номер страницы внутри 4кб блока
      uint8_t work_for_FAT;		 // если 1, все операции записи, чтения и т.д. выполняются для FAT
}FLASH_INFO;

void test(DEVICE_HANDLE handle);
#endif

#endif



