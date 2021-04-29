#include "Config.h"
#if(_MODEM_)
#ifndef STD_MODEM_H				
#define STD_MODEM_H

#include <stdint.h>
#include "DriversCore.h"         
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h" 
#include "ppp/ppp.h"
												 
#include "sio.h"


														   
#ifdef __cplusplus
extern "C" {
#endif
						    
#define		GPRS_COUNTERS_NUM		5

#define TSEC                100
//#define MODEM_UART_NAME "UART0"
//#define MODEM_DIAL_NUMBER "*99#"
//#define MODEM_INIT_STRING "AT+CGDCONT=1,\"IP\",\"web2.velcom.by\""
//#define MODEM_INIT_STRING "AT+CGDCONT=1,\"IP\",\"internet.life.com.by\"\r"
//#define MODEM_INIT_STRING "AT+CGDCONT=1,\"IP\",\"mts\"\r"


//#define PPP_USER                "web2"
//#define PPP_PASS                "web2"

//#define PPP_USER                "mts"
//#define PPP_PASS                "mts"

//#define PPP_USER                ""
//#define PPP_PASS                ""


#define CONFIG_STATUS()						GPIO_SetDir(MODEM_STATUS_PORT, 1 << MODEM_STATUS_PIN, 0)
#define CONFIG_STATUS_PIN_MODE()			set_ResistorMode(MODEM_STATUS_PORT, MODEM_STATUS_PIN, PINSEL_PINMODE_PULLDOWN)
#define CONFIG_POWER_ON()  					GPIO_SetDir(MODEM_POWER_ON_PORT, 1 << MODEM_POWER_ON_PIN, 1)


#if (MULTY_SIM == OPTION_ON)
	#define CONFIG_F_SIM()					GPIO_SetDir(SIM_1_PRESENT_PORT, 1 << SIM_1_PRESENT_PIN, 0)
	#define CONFIG_S_SIM()	 				GPIO_SetDir(SIM_2_PRESENT_PORT, 1 << SIM_2_PRESENT_PIN, 0)
	#define CONFIG_SS()						GPIO_SetDir(SIM_SELECT_PORT, 1 << SIM_SELECT_PIN, 1)

	#define IS_S_SIM_NO_PRESENT()           (FIO_ReadValue(SIM_2_PRESENT_PORT) & (1 << SIM_2_PRESENT_PIN))   
	#define IS_F_SIM_NO_PRESENT()           (FIO_ReadValue(SIM_1_PRESENT_PORT) & (1 << SIM_1_PRESENT_PIN))

	#define S_SIM_ON()              		FIO_SetValue(SIM_SELECT_PORT, 1 << SIM_SELECT_PIN)   
	#define F_SIM_ON()              		FIO_ClearValue(SIM_SELECT_PORT, 1 << SIM_SELECT_PIN)
	
	              
#else
	#define CONFIG_F_SIM()				
	#define CONFIG_S_SIM()	 				
	#define CONFIG_SS()					

	#define IS_S_SIM_NO_PRESENT()              1  
	#define IS_F_SIM_NO_PRESENT()              0

	#define S_SIM_ON()              	   
	#define F_SIM_ON()              		
#endif

#define GET_MODEM_SATUS()      				(GPIO_ReadValue(MODEM_STATUS_PORT) & (1 << MODEM_STATUS_PIN))

// Состояния модема
#define IDLE            0   //  О модеме ниченго не известно
#define POWER_ON        1   //  Модем включен
#define ERR             2   //  Состояние ошибки
#define NO_SIM          3   //  отсутствует сим карта
#define NO_PIN          4
#define CHECK_REG       5   //  проверка оператора
#define OPERATOR_REG    6   //  оператор найден
#define GPRS_DISCONNECTING 7
#define GPRS_CONNECTING 9   //  подключение по GPRS
#define ON_LINE         11   //  в состоянии подключения по GPRS
#define PPP_ON          12
#define GPRS_ON		    13
#define GPRS_OFF		14
#define OFF_LINE	    15
#define PPP_CLOSING	    16
#define PPP_OFF	    	17
#define TURN_OFF        18


#define MAX_MODEM_DATA_OFFSET         180
#define MAX_MODEM_DATA_TEXT_OFFSET    145

#define WITH_COMMOND_MODE_FLAG           1
#define WITHOUT_COMMOND_MODE_FLAG        0

/*---------------------------------Коды для настройки----------------------------------*/
#define   DIAL_NUMBER       0
#define   INIT_STR          1
#define   LOGIN_CON         2
#define   PASSWORD_CON      3
#define   PIN_EN            4
#define   PIN               5
#define   ALL_SETTINGS      6

#define   MODEM_AUTOMATE_CTL         7
#define   MODEM_POOL_CTL    8
/*------------------------------------------------------------------------------------*/


//-**************************Задачи*************************************************

#define       SMS_PROC          0
#define       GPRS_CONNECT      1
#define       GPRS_SEND         2
#define       RESET_FROM_SMS    3

//-**********************************************************************************

#ifdef __cplusplus
}
#endif



#endif
#endif
