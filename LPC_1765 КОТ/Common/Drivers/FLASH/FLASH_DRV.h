#ifndef FLASH_DRV_H
#define FLASH_DRV_H

#include "config.h"
#if(_FLASH_)

#if(_FLASH_TYPE_ == _FLASH_TYPE_FM25C16_)
	#include "FM25C16_LPC17xx.h"
#elif(_FLASH_TYPE_ == _FLASH_TYPE_AT45DB_)
	#include "AT45DB_LPC17xx.h"
#elif(_FLASH_TYPE_ == _FLASH_TYPE_AT25DF161_17xx_)
	#include "AT25DF161_LPC17xx.h"
#elif(_FLASH_TYPE_ == _FLASH_TYPE_AT25DF161_23xx_)
	#include "AT25DF161_LPC23xx.h"
#elif(_FLASH_TYPE_ == _FLASH_TYPE_M25PE80_23xx_)
	#include "M25PE80_LPC23xx.h"
#elif(_FLASH_TYPE_ == _FLASH_TYPE_M25PE80_17xx_)
	#include "M25PE80_LPC17xx.h"
#elif(_FLASH_TYPE_ == _FLASH_TYPE_W25Q80_STM32F1_)
	#include "W25Q80_STM32F10x.h"
#endif

#endif // _FLASH_

#endif



