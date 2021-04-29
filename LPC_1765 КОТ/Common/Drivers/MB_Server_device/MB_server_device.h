#ifndef _MB_SERVER_DEVICE_H_
#define _MB_SERVER_DEVICE_H_

#include <stdint.h>

#include "Config.h"
#if(_MBS_)

#include "driverscore.h"

typedef struct
{
	uint8_t *idBuf;		// буфер для id устройств
	uint32_t *rxCount;	// счетчики удачно принятых пакетов
	uint32_t *txCount;	// счтечик отправленных пакетов
	uint8_t idCount;
}__attribute__ ((__packed__)) ID_STRUCT;

#define GET_SUPPORTED_IDs   		0
#define GET_SUPPORTED_IDs_NUMBER	1
#define GET_TX_PACKETS_COUNT		2
#define GET_RX_PACKETS_COUNT		3
#define MB_SERVER_RW				4

#define MB_TEXT_BUF_LENTH			300


#define MAX_MODBUS_BAD_READ_CNT     (10)	

// для произвольного чтения записи из User_Uart
typedef struct
{
	uint16_t addr; 		// адрес чтения/записи
	uint8_t  id;		// mb id
	char opType;		// чтение/запись
	char valType;		// тип величины для записи/чтения
	uint8_t valBuf[4];	// буфер для величины
} __attribute__ ((__packed__)) MB_RW_STRUCT;


typedef struct __attribute__ ((__packed__)) tagSlaveValue   //  Структура для хранения прочитанной из слейвера величины
{
	uint8_t		adress;
	uint16_t	reg;
	float		value;
	uint8_t		flag;
	uint16_t    bad_atmp_lim;
	uint16_t    bad_atmp_cnt;
}SLAVE_VALUE;


typedef struct __attribute__ ((__packed__)) tagSlaveInfo   //  Структура для для чтения слейвера
{
	uint8_t		adress;
	uint16_t	reg;
	uint16_t	quantity; 
}SLAVE_INFO;

typedef struct __attribute__ ((__packed__)) tagMBServerInfo
{
	SLAVE_VALUE		*pSlave_val;
	SLAVE_INFO		*pSlave_info;
	uint8_t			read_sl_count;    //  Счетчик который считает чтение из подчиненного
	uint8_t			value_sl_count;   //  Счетчик который считает количество прочтенных регистров
	uint8_t			read_sl_max;
	uint8_t			value_sl_max;
	uint8_t 		sens_pres_flag;  //  		  
#if(_MULTITASKING_)
	volatile xSemaphoreHandle Sem;
#else
	uint8_t Sem;
#endif
}MB_SERVER_INFO;


typedef struct __attribute__ ((__packed__)) tagMBSDevData
{
    uint32_t	text_data_point;
	uint8_t		adress;
	uint16_t	reg;
	uint8_t		read_mode;
}MBS_DATA;

void DelMBDev(void);
void init_MBS(void);


#endif // _MB_SERVER_DEVICE_

#endif
