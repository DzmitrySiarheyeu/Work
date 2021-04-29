#ifndef _MB_SERVER_DEVICE_H_
#define _MB_SERVER_DEVICE_H_

#include <stdint.h>

#include "Config.h"
#if(_MBS_)

#include "driverscore.h"

typedef struct
{
	uint8_t *idBuf;		// ����� ��� id ���������
	uint32_t *rxCount;	// �������� ������ �������� �������
	uint32_t *txCount;	// ������� ������������ �������
	uint8_t idCount;
}__attribute__ ((__packed__)) ID_STRUCT;

#define GET_SUPPORTED_IDs   		0
#define GET_SUPPORTED_IDs_NUMBER	1
#define GET_TX_PACKETS_COUNT		2
#define GET_RX_PACKETS_COUNT		3
#define MB_SERVER_RW				4

#define MB_TEXT_BUF_LENTH			300


#define MAX_MODBUS_BAD_READ_CNT     (10)	

// ��� ������������� ������ ������ �� User_Uart
typedef struct
{
	uint16_t addr; 		// ����� ������/������
	uint8_t  id;		// mb id
	char opType;		// ������/������
	char valType;		// ��� �������� ��� ������/������
	uint8_t valBuf[4];	// ����� ��� ��������
} __attribute__ ((__packed__)) MB_RW_STRUCT;


typedef struct __attribute__ ((__packed__)) tagSlaveValue   //  ��������� ��� �������� ����������� �� �������� ��������
{
	uint8_t		adress;
	uint16_t	reg;
	float		value;
	uint8_t		flag;
	uint16_t    bad_atmp_lim;
	uint16_t    bad_atmp_cnt;
}SLAVE_VALUE;


typedef struct __attribute__ ((__packed__)) tagSlaveInfo   //  ��������� ��� ��� ������ ��������
{
	uint8_t		adress;
	uint16_t	reg;
	uint16_t	quantity; 
}SLAVE_INFO;

typedef struct __attribute__ ((__packed__)) tagMBServerInfo
{
	SLAVE_VALUE		*pSlave_val;
	SLAVE_INFO		*pSlave_info;
	uint8_t			read_sl_count;    //  ������� ������� ������� ������ �� ������������
	uint8_t			value_sl_count;   //  ������� ������� ������� ���������� ���������� ���������
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
