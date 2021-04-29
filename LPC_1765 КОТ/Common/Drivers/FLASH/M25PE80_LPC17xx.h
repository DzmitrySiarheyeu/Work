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

// ���� ��� ������ � flash ����� IOCtl � ������ �� FAT (�� ��������� ��� �������� ��� FAT)

#define SWITCH_TO_RAW                   0       //������������� � ����� "������" ������ �� Flash
#define SET_FLASH_ADDR                  1       //SeekFlash �� ����� ���������� 32-������ �����
#define ERASE_BLOCK                     2

// ���� ��������
#define READ				0x03	// ������ �� FLASH

#define ERASE_PAGE                  	0xDB    // ������� ��������
#define ERASE_4K_BLOCK                  0x20    // ������� ���� 4 ��
#define ERASE_32K_BLOCK                 0x52    // ������� ���� 32 ��
#define ERASE_64K_BLOCK                 0xD8    // ������� ���� 64 ��
#define CHIP_ERASE                      0x60    // ������� ���
#define BYTE_PAGE_PROGRAM               0x02    // ������ 1-256 ����
#define BYTE_PAGE_WRITE					0x0A


#define WRITE_ENABLE                    0x06    // ��������� ������
#define WRITE_DISABLE                   0x04    // ��������� ������

#define   READ_STATUS_REGISTR		0x05	// ������ �������� �������
#define WRITE_STATUS_REGISTR_B1         0x01    // ������������ ��� ������/��������� ���������� ������ �� ������
#define WRITE_STATUS_REGISTR_B2         0x31    // ������������ ��� ����-��

// ���� STATUS_REGISTR

#define FLASH_STATUS_BUSY               (1 << 0)  // ���� �����

//#define FAT_CLUSTER_SIZE                (4096)
                
typedef struct tagFLASH_INFO
{
      DEVICE_HANDLE *pSPI;
      uint32_t block_num;	         // ����� 4�� ����� �� ���� (� ������ FAT) / ����� �� �� ���� (� ������ RAW)
      uint32_t page_num;                 // ����� �������� ������ 4�� �����
      uint8_t work_for_FAT;		 // ���� 1, ��� �������� ������, ������ � �.�. ����������� ��� FAT
}FLASH_INFO;

void test(DEVICE_HANDLE handle);
#endif

#endif



