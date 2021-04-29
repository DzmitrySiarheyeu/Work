#ifndef _OMNICOMM_H_
#define _OMNICOMM_H_

#include "usart.h"
#include "crc8.h"
#include "reply.h"

//OmniComm frame prefix
#define OMNICOMM_MASTER_PREFIX			0x31
#define OMNICOMM_SLAVE_PREFIX			0x3E
#define OMNICOMM_OLD_PREFIX				0x44

//OmniComm status
#define OMNICOMM_BUSY			0x00
#define OMNICOMM_IDLE			0x01
#define OMNICOMM_FRAME_OK		0x04
#define OMNICOMM_FRAME_ERROR	0x03


//OmniComm timeout
#define OMNICOMM_FRAME_INTERVAL	200	//интервал в байтах. ≈сли между прерывани€ми USART_RX прошло
									//врем€, равное времени передачи байтов в количестве
									//OMNICOMM_FRAME_INTERVAL, считаем, что началс€ новый пакет
									//OMNICOMM_FRAME_INTERVAL = 1..8 дл€ кварца 7372800
#define OMNICOMM_TIMEOUT (unsigned char)(F_CPU / USART_BR * 10 * OMNICOMM_FRAME_INTERVAL / 128)//пересчитанный в такты таймера(с предделителем 128) OMNICOMM_FRAME_INTERVAL

//command type
#define OLD_COMMAND_TYPE 	0x01
#define NEW_COMMAND_TYPE	0x02

//OmniComm commands
#define SET_ID				0x01
#define ID_READ				0x02
#define SET_ADDR			0x03
#define SET_NET_MODE		0x04
#define CONFIG_READ			0x05
#define DATA_READ			0x06
#define DATA_CONTIN			0x07
#define FCURR_FMIN			0x08
#define FCURR_FMAX			0x09
#define TSLOPE_T0			0x0A
#define SET_FMIN			0x0B
#define SET_FMAX			0x0C
#define SET_PWM_MAX			0x0D
#define SET_PWM_MOD			0x0E
#define SET_TRL				0x0F
#define SET_FILT_LENG		0x11
#define SET_PASSW			0x12
#define SET_PERIOD			0x13
#define FILT_LENG_READ		0x14
#define PASSW_READ			0x15
#define SET_AUTO_MODE		0x17
#define SET_BOOT_MODE		0x77
#define DATA_READ_OLD		0x78
#define DATA_CONTIN_OLD     0x79

//exit codes
#define OMNICOMM_NO_ERRORS	0x00
#define OMNICOMM_ERROR		0x01

//omnicomm variables
extern unsigned char	omnicomm_addr, omnicomm_cmd, omnicomm_timeout_elapsed,
				omnicomm_crc;

extern volatile unsigned char omnicomm_status, omnicomm_counter, omnicomm_frame_size,omnicomm_command_type;
extern volatile unsigned long omnicomm_timer;

//function prototypes
void OmniComm_Init_timer (void);
void OmniComm_Reset_timer (void);
void OmniComm_Stop_timer (void);
void OmniComm_Init (void);
unsigned char OmniComm_Do_crc (unsigned char frame_size);
void OmniComm_Send_frame (void);
void OmniComm_Send_frame_old_data (void);
void OmniComm_Reply_error (void);
void OmniComm_Reply_ok (void);
unsigned char OmniComm_Check_frame (void);
unsigned char OmniComm_Check_old_frame (void);
void do_omnicom(void);

void omnicomm_de(void);
void omnicomm_tx(void);
void omnicomm_rx(void);

#endif
