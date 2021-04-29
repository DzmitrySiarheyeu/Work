#ifndef MODEM_DRV_H
#define MODEM_DRV_H

#include "Config.h"
#if _MODEM_

#include "sio.h"
#include "DriversCore.h"

typedef struct __attribute__ ((__packed__)) tagMODEMData
{
    uint8_t Signal;            	//  Уровень сигнала
    uint8_t ConnectCount;      	//  Счетчик подключений
    char Dial_Num[10];         	//  Номер дозвона
    char Init_String[50];      	//  Строка для инициализации GPRS
    char Login[50];            	//  Имя пользователя
    char Password[50];         	//  Пороль
	char Op_name[20];          	//  Номер дозвона
    uint8_t PinEn;				//	Включен ввод пина в настройках
	uint8_t PinRequest;			//	Требуется ли ввод пина (запрос от сим карты)
    char Pin[5];
	char OwnNumUSSD[20];
	char OwnNum[14];
	char imei[16];
	char error_string[30];
	char last_command[10];
	uint8_t synk_flag;
	uint8_t cur_sim;
	uint8_t Roaming;	   //  Флаг показывающий разрешен ли роуминг или нет
	uint8_t Cur_Roaming;   //  Если работа видется в роуминге флаг установлен
	float	ext_pow_lim;
    uint16_t  DataPoint;
    uint16_t  DataPointText;
}MODEMData;


// Объект Modem
typedef struct tagMODEM_INFO
{
    DEVICE_HANDLE *pUart;
	sys_sem_t			sem;
}MODEM_INFO;

int ModemDial(DEVICE_HANDLE handle);
int CreateModem(void);


int cmd_operat_process(uint8_t ch) ;
void op_name(void);
void sig_level(void);
int find_out_own_num(void);
void modem_turn_off(void);

void init_modem (void);
void Modem_state_set(uint8_t state);
uint8_t Modem_state_get(void);
int take_modem_from_online(void);
int give_modem_to_online(void);
void modem_automate(DEVICE_HANDLE handle);
void modem_pool(DEVICE_HANDLE handle);  
void SetModemSettings(uint8_t flag);

int sms_check(char *buff);
int sms_read(char *index, char *buff, uint16_t count);
int sms_send(char *phon_num, char *buff);
int sms_delete(char *index);
int ussd_send(char *buf, int count);
int Ring_up(uint8_t *buf, uint16_t len);
int Sync_time_gsm(void);
void imei(void);
void sig_level(void);
uint8_t modem_online (void);
int At_send(char *buf, uint16_t len);
void SmsSend(uint32_t num);
int GprsCountersCheck(uint8_t flag_online);
void SetModemSettings(uint8_t flag);
uint8_t GetRoamingState(void);
float GetExtPowLim(void);
int cmd_operat(char *command, char *answer, uint32_t delay);
char *cmd_get_parametr(char *command, char *answer, uint32_t delay);

#if(OPERATOR_NAME == OPTION_ON)
	uint8_t * GetOpName(void);
#endif


#if(IMEI == OPTION_ON)
	int GetIMEI(char *buf);
#endif					 

#if(SIGNAL_LEVEL == OPTION_ON)
	uint8_t GetSigLevel(void);
#endif

#if(OWN_DIAL_NUMBER == OPTION_ON)
	uint8_t * GetSimNum(void);
#endif

extern MODEMData modemDate;
extern MODEM_INFO modem_info;
extern DEVICE modem_device;


/*-------------------------------------------------------------------------------------*/

#if(_MODEM_TYPE_ == _MODEM_TYPE_SIM300DZ_LPC17xx)
	#include "SIM300DZ_LPC17xx.h"
#elif(_MODEM_TYPE_ == _MODEM_TYPE_TM1_LPC23xx_)
	#include "TM1_LPC23xx.h"
#elif(_MODEM_TYPE_ == _MODEM_TYPE_TM11Q_STM32F1_)
	#include "TM11Q_stm32f10x.h"
#endif

#endif // _MODEM_

#endif // MODEM_DRV_H

