#ifndef CONFIG_H
#define CONFIG_H


#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

//void vUserDefrag(void);
																																							  
#define INIT_ON		0
#define SAVER_INIT	0

#define DEVICE_ON   1
#define DEVICE_OFF  0

#define OPTION_ON	1
#define OPTION_OFF	0

#define  _WDT_LPC_ 1
#define  _DAILY_RESET_  1

#define  SUPPLY_SENSOR			"A3"


#define		QNaN		0x7FC00000
#define		SNaN		0x7F800001

#define		SET_SNaN(x)    *((uint32_t *)x) = SNaN
#define		IS_SNaN(x)     (*((uint32_t *)&x) == SNaN)?1:0

#define		SET_QNaN(x)    *((uint32_t *)x) = QNaN
#define		IS_QNaN(x)     (*((uint32_t *)&x) == QNaN)?1:0

#define true  TRUE
#define false FALSE

enum PROFILES
{
	HOME_STOP		= 0,
	HOME_MOVE		= 1,
	ROAMING_STOP	= 2,
	ROAMING_MOVE	= 3,
};

/*****************************************************/
/*****************************************************/
/****ВКЛЮЧЕНИЕ ДРАЙВЕРОВ ВЫСОКОУРОВНЕВЫХ УСТРОЙСТВ****/
/*****************************************************/
/*****************************************************/


/********************ФЛЭШ-ПАМЯТЬ**********************/

#define _FLASH_					DEVICE_ON     
#define _FLASH_M25PE80_			DEVICE_ON
#if(_FLASH_ == DEVICE_ON)

/*
 Дефайны возможных конфигураций
 НЕ МЕНЯТЬ
 */	
#define _FLASH_TYPE_FM25C16_			0
#define _FLASH_TYPE_AT45DB_				1
#define _FLASH_TYPE_AT25DF161_17xx_		2
#define _FLASH_TYPE_AT25DF161_23xx_		3
#define _FLASH_TYPE_M25PE80_23xx_		4
#define _FLASH_TYPE_M25PE80_17xx_		5

#define	_FLASH_L_L_DRIVER_SSP0_		0
#define	_FLASH_L_L_DRIVER_SSP1_		1

			/*
			 Собственно конфигурация флэш-памяти
			 */
			#define _FLASH_TYPE_			_FLASH_TYPE_M25PE80_17xx_		
			#define	_FLASH_L_L_DRIVER_		_FLASH_L_L_DRIVER_SSP0_
			#define _FLASH_L_L_DRIVER_NAME_ "SSP0"
			/*
			 Настройка ноги ChipSelect
			 */
			#define _FLASH_CS_PORT_						1
			#define _FLASH_CS_PIN_						21

			#define	_FAT_FLASH_PAGE_SIZE_					256
			#define _FLASH_FAT_SECTOR_SIZE_				    512
			#define _SAVER_FLASH_PAGE_SIZE_				    256
			#define _FLASH_PROG_PAGE_SIZE_					256

#endif // _FLASH_ == DEVICE_ON

/**********************ЧАСЫ***************************/

#define _CLOCK_			DEVICE_OFF 
#define _CLOCK_RTC_		DEVICE_ON      

#if(_CLOCK_ == DEVICE_ON)


	
#define _CLOCK_TYPE_M41T56_			0
#define _CLOCK_TYPE_M41T81_			1

#define	_CLOCK_L_L_DRIVER_I2C0_		0
#define	_CLOCK_L_L_DRIVER_I2C1_		1
#define	_CLOCK_L_L_DRIVER_I2C2_		2

			/*
			 Собственно конфигурация часов
			 */
			#define _CLOCK_TYPE_			_CLOCK_TYPE_M41T81_		
			#define	_CLOCK_L_L_DRIVER_		_CLOCK_L_L_DRIVER_I2C1_
			#define _CLOCK_L_L_DRIVER_NAME_ "I2C1"

	#define	_TIME_SYNC_GPS_			 DEVICE_OFF
	#define	_TIME_SYNC_GSM_			 DEVICE_ON

	#define	CLOCK_SYNC_HOUR			21
	#define	CLOCK_SYNC_MIN			59

#endif 	// _CLOCK_


#if(_CLOCK_RTC_ == DEVICE_ON)

#define _CLOCK_TYPE_M41T56_			0
#define _CLOCK_TYPE_M41T81_			1
#define _CLOCK_TYPE_GPS_			2
#define _CLOCK_TYPE_LPC17xx_RTC_	3

			/*
			 Собственно конфигурация часов
			 */
			#define _CLOCK_TYPE_			_CLOCK_TYPE_LPC17xx_RTC_		

			#define	_TIME_SYNC_GPS_			 DEVICE_OFF
			#define _TIME_SYNC_GSM_			 DEVICE_ON
			#define _TL_TIME_STAMP_          DEVICE_OFF

			#define	CLOCK_SYNC_HOUR			21
			#define	CLOCK_SYNC_MIN			59

#endif 	// _CLOCK_RTC_

/**********************АЦП****************************/

#define _ADC_		DEVICE_OFF                      //  Подключение АЦП
#if(_ADC_ == DEVICE_ON)

	#define			ADC_FREQ			10000     //  Частота преобразования АЦП
	#define			ADC_BUF_SIZE		10        //  Буфер для ЦАП данных
	#define 		NUMBER_OF_CH_ADC	5          //  Требуемое количество каналов АЦП
	#define			ADC_REF				3.30F

	#define _ADC_CH_0		DEVICE_OFF		       //   Включение нулевого канала АЦП
	#if(_ADC_CH_0 == DEVICE_ON)

		#define		PROGRAM_ADC_0_CH		0      //   Номер программного канала для данного физического канала АЦП
		#define		ADC_0_PORT				0
		#define		ADC_0_PIN				23
		#define		ADC_0_PIN_FUN			1
		#define		ADC_0_KOEF				1.0F
	
	#endif // _ADC_CH_0

	#define _ADC_CH_1		DEVICE_OFF
	#if(_ADC_CH_1 == DEVICE_ON)

		#define		PROGRAM_ADC_1_CH		0
		#define		ADC_1_PORT				0
		#define		ADC_1_PIN				24
		#define		ADC_1_PIN_FUN			1 
		#define		ADC_1_KOEF				1.0F
	
	#endif // _ADC_CH_1

	#define _ADC_CH_2		DEVICE_OFF
	#if(_ADC_CH_2 == DEVICE_ON)

		#define		PROGRAM_ADC_2_CH		3
		#define		ADC_2_PORT				0
		#define		ADC_2_PIN				25
		#define		ADC_2_PIN_FUN			1
		#define		ADC_2_KOEF				3.5F
	
	#endif // _ADC_CH_2

	#define _ADC_CH_3		DEVICE_OFF
	#if(_ADC_CH_3 == DEVICE_ON)

		#define		PROGRAM_ADC_3_CH		2
		#define		ADC_3_PORT				0
		#define		ADC_3_PIN				26
		#define		ADC_3_PIN_FUN			1
		#define		ADC_3_KOEF				3.5F
	
	#endif // _ADC_CH_3

	#define _ADC_CH_4		DEVICE_OFF
	#if(_ADC_CH_4 == DEVICE_ON)

		#define		PROGRAM_ADC_4_CH		0
		#define		ADC_4_PORT				1
		#define		ADC_4_PIN				30
		#define		ADC_4_PIN_FUN			3
		#define		ADC_4_KOEF				1.0F
	
	#endif // _ADC_CH_4

	#define _ADC_CH_5		DEVICE_OFF
	#if(_ADC_CH_5 == DEVICE_ON)

		#define		PROGRAM_ADC_5_CH		4
		#define		ADC_5_PORT				1
		#define		ADC_5_PIN				31
		#define		ADC_5_PIN_FUN			3
		#define		ADC_5_KOEF				11.0F
	
	#endif // _ADC_CH_5

	#define _ADC_CH_6		DEVICE_OFF
	#if(_ADC_CH_6 == DEVICE_ON)

		#define		PROGRAM_ADC_6_CH		1
		#define		ADC_6_PORT				0
		#define		ADC_6_PIN				2 
		#define		ADC_6_PIN_FUN			2
		#define		ADC_6_KOEF				11.0F
	
	#endif // _ADC_CH_6

	#define _ADC_CH_7		DEVICE_ON
	#if(_ADC_CH_7 == DEVICE_ON)

		#define		PROGRAM_ADC_7_CH		0
		#define		ADC_7_PORT				0
		#define		ADC_7_PIN				3
		#define		ADC_7_PIN_FUN			2
		#define		ADC_7_KOEF				11.0F
	
	#endif // _ADC_CH_7
	
#endif // _ADC_ 

/********************MODBUS***************************/

#define _MODBUS_	DEVICE_ON

extern int MBPort;

#if(_MODBUS_ == DEVICE_ON)

#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h" 


			#define	_MB_SERIAL_			1
			#define	_MB_SERIAL_CLIENT_	1

			#define	_MB_TCP_ 			1
			#define	_MB_TCP_CLIENT_		1

			#define	_MB_CLIENT_			1
			#define _MBC_				1
			#define _MBC_L_L_DRIVER_NAME_ 		"TCP_RD_1"

			#define	_MODBUS_SERVER_		1
			#define _MBS_L_L_DRIVER_NAME_ 		"UART1"

			#define _MD_ID_						99

			#define		_MB_DIAGNOSTIC_FUNC_		0
			#define		_MB_RD_IN_REG_FUNC_			1
			#define		_MB_WR_S_REG_FUNC_			0
			#define		_MB_WR_FILE_FUNC_			0
			#define		_MB_RD_FILE_FUNC_			0

			#define 	MB_TCP_DEFAULT_PORT 		MBPort/* TCP listening port. */

			#define 	_MB_DE_CONTROL_				0


			#define U1LSR_TEMT_BITBAND (*(volatile unsigned *)0x42200298)
			#define		INIT_DE485()			GPIO_SetDir(2, 1 << 5, 1)
			#define		SET_DE485()				FIO_SetValue(2, 1 << 5)       
			#define		CLR_DE485()				{while(U1LSR_TEMT_BITBAND == 0); FIO_ClearValue(2, 1 << 5);}     


#endif // _MODBUS_

#define _MBS_	DEVICE_ON

#if(_MBS_ == DEVICE_ON)

#endif // _MBS_

/********************DISPLAY***************************/

#define _DISPLAY_	DEVICE_ON

#if(_DISPLAY_ == DEVICE_ON)

extern uint8_t LCDContrast;
	
	#define _DISPLAY_L_L_DRIVER_NAME_     "I2C0"
	#define _LCD_CONTRAST_			  LCDContrast

#endif

/********************SAVER***************************/

#define _SAVER_	   DEVICE_ON

#if(_SAVER_ == DEVICE_ON)

			#define _SAVER_L_L_DRIVER_NAME_ 		"FLASH"

			#define MAX_BUF_LEN						65

			#define SEGMENT_NUMBER				3
			#define	SEG_RAM_START_SECTOR		2048
			#define	SEG_RAM_END_SECTOR			2224

			#define		FLAGS_SEG				0
			#define		LOG_SEG					1
			#define		LOG_COUNTERS_SEG		2

			#define		FLAGS_SEG_SIZE			32
			#define		LOG_SEG_SIZE			64
			#define     LOG_COUNTERS_SEG_SIZE  	32

			#define		FLAGS_SEG_REC_LEN			10	// длина записи указывается вместе с crc (+2) !!!! 
			#define		LOG_REC_LEN		    		64
			#define     LOG_COUNTERS_SEG_REC_LEN 	10

			#define		FLAGS_SEG_CRC			1
			#define		LOG_REC_CRC				0
			#define     LOG_COUNTERS_SEG_CRC  	1
			
			#define		IMP_1_SEG		0

#endif // _SAVER_



/********************MODEM***************************/

#define _MODEM_	   DEVICE_ON

#if(_MODEM_ == DEVICE_ON)
/*
 Дефайны возможных конфигураций
 НЕ МЕНЯТЬ
 */	
#define _MODEM_TYPE_SIM300DZ_LPC17XX		0


#define	_MODEM_L_L_DRIVER_UART0_		0
#define	_MODEM_L_L_DRIVER_UART1_		1
#define	_MODEM_L_L_DRIVER_UART2_		2
#define	_MODEM_L_L_DRIVER_UART3_		3


			/*
			 Собственно конфигурация модема
			 */
			#define _MODEM_TYPE_					_MODEM_TYPE_SIM300DZ_		
			#define	_MODEM_L_L_DRIVER_				_MODEM_L_L_DRIVER_UART3_
			#define _MODEM_L_L_DRIVER_NAME_ 		"UART3"

			#define MODEM_STATUS_PORT				2
			#define MODEM_STATUS_PIN				13

			#define MODEM_POWER_ON_PORT				1
			#define MODEM_POWER_ON_PIN				27

			#define MODEM_DTR_PORT					0
			#define MODEM_DTR_PIN					18

			#define EXT_POW_OFF						OPTION_ON
			#define MODEM_EXT_POW_PORT				2
			#define MODEM_EXT_POW_PIN				4
			
			#if EXT_POW_OFF
				#define MODEM_EXT_POW_PORT				2
				#define MODEM_EXT_POW_PIN				4
				#define CONFIG_EXT_POW_ON()  			GPIO_SetDir(MODEM_EXT_POW_PORT, 1 << MODEM_EXT_POW_PIN, 1)
				#define EXT_POW_KEY_UP()      			GPIO_SetValue(MODEM_EXT_POW_PORT, 1 << MODEM_EXT_POW_PIN)
				#define EXT_POW_KEY_DOWN()    			GPIO_ClearValue(MODEM_EXT_POW_PORT, 1 << MODEM_EXT_POW_PIN)
			#endif
			
			#define CONFIG_DTR()						GPIO_SetDir(MODEM_DTR_PORT, 1 << MODEM_DTR_PIN, 1)
			#define MODEM_DTR_ON()      				GPIO_ClearValue(MODEM_DTR_PORT, 1 << MODEM_DTR_PIN)
			#define MODEM_DTR_OFF()     				GPIO_SetValue(MODEM_DTR_PORT, 1 << MODEM_DTR_PIN)
			#define POWER_ON_KEY_UP()      				GPIO_ClearValue(MODEM_POWER_ON_PORT, 1 << MODEM_POWER_ON_PIN)
			#define POWER_ON_KEY_DOWN()    				GPIO_SetValue(MODEM_POWER_ON_PORT, 1 << MODEM_POWER_ON_PIN)


			#define SIM_1_PRESENT_PORT				2
			#define SIM_1_PRESENT_PIN				3

			#define SIM_2_PRESENT_PORT				2
			#define SIM_2_PRESENT_PIN				2

			#define SIM_SELECT_PORT					0
			#define SIM_SELECT_PIN					6

			#define PIN_ENABLE 						OPTION_OFF
			#define PIN_VALUE 						"0000"

			#define MODEM_TICK_DELAY 				100
			#define MODEM_DIAL_NUMBER 				"*99#"
			#define MODEM_INIT_STRING 			/*"AT+CGDCONT=1,\"IP\",\"vmi.velcom.by\""*/	"mts"//"AT+CGDCONT=1,\"IP\",\"web2.velcom.by\""
			#define PPP_USER                	/*"vmi"*/	"mts"
			#define PPP_PASS                	/*"vmi"*/	"mts"


			#define PPP_RX_SIZE						400

			#define		IMEI						OPTION_ON
			#define		OPERATOR_NAME				OPTION_ON
			#define		OWN_DIAL_NUMBER				OPTION_ON
			#define		SIGNAL_LEVEL				OPTION_ON	
            #define     _TIME_SYNC_CLTS_            1

#endif // _MODEM_


/********************1-WIRE***************************/

#define _OWI_	   DEVICE_OFF

#if(_OWI_ == DEVICE_ON)

#define _OWI_TYPE_DS2482S_			0

#define	_OWI_L_L_DRIVER_I2C0_		0
#define	_OWI_L_L_DRIVER_I2C1_		1
#define	_OWI_L_L_DRIVER_I2C2_		2

			#define _OWI_TYPE_						_OWI_TYPE_DS2482S_		
			#define	_OWI_L_L_DRIVER_				_OWI_L_L_DRIVER_I2C1_
			#define _OWI_L_L_DRIVER_NAME_ 			"I2C1"
			#define MAX_OWI_DEVICES   				5

#endif // _OWI_

/********************GPS***************************/

#define _GPS_	   DEVICE_OFF

#if(_GPS_ == DEVICE_ON)

#define _GPS_TYPE_S1315R_			0

#define	_GPS_L_L_DRIVER_UART0_		0
#define	_GPS_L_L_DRIVER_UART1_		1
#define	_GPS_L_L_DRIVER_UART2_		2
#define	_GPS_L_L_DRIVER_UART3_		3

			#define _GPS_TYPE_						_GPS_TYPE_S1315R_		
			#define	_GPS_L_L_DRIVER_				_GPS_L_L_DRIVER_UART2_
			#define _GPS_L_L_DRIVER_NAME_ 			"UART2"

			#define GPS_RST_PORT					1
			#define GPS_RST_PIN						1

			#define GPS_POWER_ON_PORT				1
			#define GPS_POWER_ON_PIN				4

			#define GPS_BUF_SIZE					512



#endif // _GPS_


/********************СТАТИСТИКА************************/

#define _STATS_		DEVICE_ON

/******************************************************/



/****************ДИСКРЕТНЫЕ ВХОДЫ**********************/

#define _DISCRETE_				DEVICE_ON
#if(_DISCRETE_ == DEVICE_ON)

    #define DISCRETE_ACTIVE_LEVEL   1
	#define DISCRETE_BUF_SIZE     	32
	#define NUMBER_OF_CH_FR_DIS    	4
	#define NUMBER_OF_CH_DIS    	4

	#define	DIS_CH_0_PORT			0
	#define	DIS_CH_0_PIN			4

	#define	DIS_CH_1_PORT			0
	#define	DIS_CH_1_PIN			24

	#define	DIS_CH_2_PORT			0
	#define	DIS_CH_2_PIN			23

	#define	DIS_CH_3_PORT			1
	#define	DIS_CH_3_PIN			26

#endif

/******************************************************/



/*****************ЧАСТОТНЫЕ ВХОДЫ**********************/
												
#define _FREQ_					DEVICE_OFF
#if(_FREQ_ == DEVICE_ON)

	#define			FREQ_BUF_SIZE		16         //  Буфер для данных
	#define 		NUMBER_OF_CH_FREQ	4          //  Требуемое количество каналов 
	
	#define		_TIM_0_CH_0_	DEVICE_ON
	#if(_TIM_0_CH_0_ == DEVICE_ON)
		
			#define		TIM_0_0_PROG_CH		3
			#define		TIM_0_0_PORT		1
			#define		TIM_0_0_PIN			26
			#define		TIM_0_0_PIN_FUN		3 					

	#endif
	
	#define		_TIM_0_CH_1_	DEVICE_OFF
	#if(_TIM_0_CH_1_ == DEVICE_ON)
		
			#define		TIM_0_1_PROG_CH		0
			#define		TIM_0_1_PORT		0
			#define		TIM_0_1_PIN			0 
			#define		TIM_0_1_PIN_FUN		0					

	#endif
	
	#define		_TIM_1_CH_0_	DEVICE_OFF
	#if(_TIM_1_CH_0_ == DEVICE_ON)
		
			#define		TIM_1_0_PROG_CH		0
			#define		TIM_1_0_PORT		0
			#define		TIM_1_0_PIN			0 
			#define		TIM_1_0_PIN_FUN		0					

	#endif
	
	#define		_TIM_1_CH_1_	DEVICE_OFF
	#if(_TIM_1_CH_1_ == DEVICE_ON)
		
			#define		TIM_1_1_PROG_CH		0
			#define		TIM_1_1_PORT		0
			#define		TIM_1_1_PIN			0 
			#define		TIM_1_1_PIN_FUN		0					

	#endif
	
	#define		_TIM_2_CH_0_	DEVICE_ON
	#if(_TIM_2_CH_0_ == DEVICE_ON)
		
			#define		TIM_2_0_PROG_CH		0
			#define		TIM_2_0_PORT		0
			#define		TIM_2_0_PIN			4 
			#define		TIM_2_0_PIN_FUN		3					

	#endif
	
	#define		_TIM_2_CH_1_	DEVICE_OFF
	#if(_TIM_2_CH_1_ == DEVICE_ON)
		
			#define		TIM_2_1_PROG_CH		0
			#define		TIM_2_1_PORT		0
			#define		TIM_2_1_PIN			0
			#define		TIM_2_1_PIN_FUN		0 					

	#endif
	
	#define		_TIM_3_CH_0_	DEVICE_ON
	#if(_TIM_3_CH_0_ == DEVICE_ON)
		
			#define		TIM_3_0_PROG_CH		2
			#define		TIM_3_0_PORT		0
			#define		TIM_3_0_PIN			23 
			#define		TIM_3_0_PIN_FUN		3					

	#endif
	
	#define		_TIM_3_CH_1_	DEVICE_ON
	#if(_TIM_3_CH_1_ == DEVICE_ON)
		
			#define		TIM_3_1_PROG_CH		1
			#define		TIM_3_1_PORT		0
			#define		TIM_3_1_PIN			24 
			#define		TIM_3_1_PIN_FUN		3					

	#endif
	
	#define 	FREQ_TIM_0		(_TIM_0_CH_0_ || _TIM_0_CH_1_)
	#define 	FREQ_TIM_1		(_TIM_1_CH_0_ || _TIM_1_CH_1_)
	#define 	FREQ_TIM_2		(_TIM_2_CH_0_ || _TIM_2_CH_1_)
	#define 	FREQ_TIM_3		(_TIM_3_CH_0_ || _TIM_3_CH_1_)		

#endif

/******************************************************/



/*****************ИМПУЛЬСНЫЕ ВХОДЫ*********************/
												
#define _IMPULSE_				DEVICE_ON
#if(_IMPULSE_ == DEVICE_ON)

	#define NUMBER_OF_CH_IMP    	3

	#define	IMP_CH_0_PORT			0
	#define	IMP_CH_0_PIN			4

	#define	IMP_CH_1_PORT			0
	#define	IMP_CH_1_PIN			24

	#define	IMP_CH_2_PORT			0
	#define	IMP_CH_2_PIN			23

	#define	IMP_CH_3_PORT			1
	#define	IMP_CH_3_PIN			26
	
	#define UIN_CAP_CH_PORT		2
	#define UIN_CAP_CH_PIN		12

#endif

/******************************************************/



/*****************************************************/
/*****************************************************/
/*****ВКЛЮЧЕНИЕ ДРАЙВЕРОВ НИЗКОУРОВНЕВЫХ УСТРОЙСТВ****/
/*****************************************************/
/*****************************************************/

/************************SSP**************************/		

#define _SSP0_ 		DEVICE_ON

#if(_SSP0_ == DEVICE_ON)	 

	#define _SSP0_PORT_			    1
	#define _SSP0_SCK_PIN_			20
	#define _SSP0_MISO_PIN_			23
	#define _SSP0_MOSI_PIN_			24
	/*
	 0: - данные считываются по фронту сигнала тактирования
	 1: - данные считываются по срезу сигнала тактирования
	 */
	#define _SSP0_CPHA_				0
   	/*
	 0: - сигнала тактирования = 0 если нет передачи данных
	 1: - сигнала тактирования = 1 если нет передачи данных 
	 */
	#define _SSP0_CPOL_				1
	/*
	 Частота сигнала тактирования
	 */
	#define _SSP0_CLOCK_RATE_		25000000

#endif // _SSP0_

#define _SSP1_ 		DEVICE_OFF

#if(_SSP1_ == DEVICE_ON)

	#define _SSP1_	DEVICE_ON
		
	#define _SSP1_SCK_PORT_			
	#define _SSP1_SCK_PIN_					
	#define _SSP1_MISO_PIN_					
	#define _SSP1_MOSI_PIN_			
	/*
	 0: - данные считываются по фронту сигнала тактирования
	 1: - данные считываются по срезу сигнала тактирования
	 */
	#define _SSP1_CPHA_				
   	/*
	 0: - сигнала тактирования = 0 если нет передачи данных
	 1: - сигнала тактирования = 1 если нет передачи данных 
	 */
	#define _SSP1_CPOL_				
	/*
	 Частота сигнала тактирования
	 */
	#define _SSP1_CLOCK_RATE_				

#endif // _SSP1_
 
#define _SSP_ 		(_SSP0_ || _SSP1_)

/************************I2C**************************/

#define _I2C0_ 		DEVICE_ON

#if(_I2C0_ == DEVICE_ON)

	#define _I2C0_PORT_			    0
	#define _I2C0_SCL_PIN_			27
	#define _I2C0_SDA_PIN_			28
	#define _I2C0_CLOCK_RATE_		400000		

#endif // _I2C0_ 

#define _I2C1_ 		DEVICE_ON

#if(_I2C1_ == DEVICE_ON)

	#define _I2C1_PORT_			    0
	#define _I2C1_SCL_PIN_			20
	#define _I2C1_SDA_PIN_			19
	#define _I2C1_CLOCK_RATE_		400000

#endif // _I2C1_ 

#define _I2C2_ 		DEVICE_OFF

#if(_I2C2_ == DEVICE_ON)

	#define _I2C2_PORT_			    
	#define _I2C2_SCL_PIN_			
	#define _I2C2_SDA_PIN_			
	#define _I2C2_CLOCK_RATE_		

#endif // _I2C2_
 
#define _I2C_ 		(_I2C0_ || _I2C1_ || _I2C2_)

/************************UART*************************/

#define _UART0_ 		DEVICE_OFF

#if(_UART0_ == DEVICE_ON)

	#define _UART0_PORT_			    
	#define _UART0_RXD_PIN_			
	#define _UART0_TXD_PIN_			
	#define _UART0_BAUDRATE_

	#define _UART0_RX_BUFF_SIZE_ 	512
	#define _UART0_TX_BUFF_SIZE_ 	512
	#define RX_TIME_OUT_VALUE_0		2000       //  время в миллисекундах		

#endif // _UART0_ 

#define _UART1_ 		DEVICE_ON

#if(_UART1_ == DEVICE_ON)

	#define _UART1_PORT_      		2
	#define _UART1_RXD_PIN_   		1
	#define _UART1_TXD_PIN_	  		0
	#define _UART1_RS485_DE_PIN_	5
	#define _UART1_BAUDRATE_  		19200
	
	#define _UART1_RX_BUFF_SIZE_ 	512
	#define _UART1_TX_BUFF_SIZE_ 	512
	#define RX_TIME_OUT_VALUE_1		200       //  время в миллисекундах	
	
	#define USE_RS485_CONTROL			1	

#endif // _UART1_ 

#define _UART2_ 		DEVICE_OFF

#if(_UART2_ == DEVICE_ON)

	#define _UART2_PORT_		0			    
	#define _UART2_RXD_PIN_		11			
	#define _UART2_TXD_PIN_		10		
	#define _UART2_BAUDRATE_	9600
	
	#define _UART2_RX_BUFF_SIZE_ 	512
	#define _UART2_TX_BUFF_SIZE_ 	512
	#define RX_TIME_OUT_VALUE_2		200       //  время в миллисекундах		

#endif // _UART2_

#define _UART3_ 		DEVICE_ON

#if(_UART3_ == DEVICE_ON)

	#define _UART3_PORT_		0			    
	#define _UART3_RXD_PIN_		1			
	#define _UART3_TXD_PIN_		0			
	#define _UART3_BAUDRATE_	115200	
	
	#define _UART3_RX_BUFF_SIZE_ 	512
	#define _UART3_TX_BUFF_SIZE_ 	512
	#define RX_TIME_OUT_VALUE_3		500      //  время в миллисекундах	

#endif // _UART3_


#define UART_BUF_SIZE      		512

#define Uart_0_ISR		UART0_IRQHandler
#define Uart_1_ISR		UART1_IRQHandler 
#define Uart_2_ISR		UART2_IRQHandler
#define Uart_3_ISR		UART3_IRQHandler  		
 
#define _UART_ 		(_UART1_ || _UART1_ || _UART2_ || _UART3_)

/************************USB_CDC*************************/

/************************USB_CDC*************************/

#define _USB_ 		DEVICE_ON
#if(_USB_ == DEVICE_ON)

	#define USB_BUF_RX_SIZE          700

#endif //_USBCDC_


#define _TCP_REDIRECTOR_  DEVICE_ON
#if(_TCP_REDIRECTOR_ == DEVICE_ON)

extern int MBPort;	

	#define TCP_REDIRECTOR_PORT      MBPort
	#define TCP_READ_DEF_TIMEOUT     10

#endif


/*****************************************************/
/*****************************************************/
/*********НАСТРОЙКА И ВКЛЮЧЕНИЕ ПРОЧИХ МОДУЛЕЙ********/
/*****************************************************/
/*****************************************************/


/**********ПОЛЬЗОВАТЕЛЬСКИЕ ТАЙМЕРЫ*******************/

#define _TIMERS_ 		OPTION_ON

#if(_TIMERS_ == OPTION_ON)

/*Включение таймеров*/
#define       LED_TIMER_ON              OPTION_ON
#define       SEND_DATA_TIMER_ON        OPTION_OFF
#define       SMS_TIMER_ON        		OPTION_ON
#define       TIME_SYNC_TIMER_ON		OPTION_ON
#define       SAVE_DATA_TIMER_ON        OPTION_OFF
#define       COMMON_POOL_TIMER_ON      OPTION_ON
#define       SENSORS_POOL_TIMER_ON     OPTION_ON
#define       ADC_POOL_TIMER_ON         OPTION_OFF
#define       MBS_TIMER_ON				OPTION_ON
#define 	  DISPLAY_UPDATE_TIMER_ON   OPTION_ON
#define 	  CLOCK_POOL_TIMER_ON		OPTION_ON
#define 	  DISCRETE_POOL_TIMER_ON	OPTION_ON
#define 	  SIG_CHECK_ON				OPTION_ON
#define		  RESETE_TO_CLOCK_TIMER_ON	OPTION_ON
#define		  PING_TIMER_ON				OPTION_ON
#define		  WDT_TIMER_ON				OPTION_ON

#endif //  _TIMERS_

/****************************************************/




/**********************ДАТЧИКИ***********************/

#define _SENSORS_ 		OPTION_ON

#if(_SENSORS_ == OPTION_ON)

//--------------------------------------------------------------------------------------------
//----Используемые функции для первоначальной обработки данных датчика------------------------
//--------------------------------------------------------------------------------------------

#define         _BUTTON_FUNC_           OPTION_ON  // Функция для работы кнопки
#if(_BUTTON_FUNC_)
	#define LONG_PRESS_INTERVAL         10
#endif

#define         _EXP_FUNC_              OPTION_ON  // Функция экспоненциального усреднения
#define         _IMP_FUNC_              OPTION_OFF  // Функция для импульсного датчика
#define         _DESCRETE_FUNC_         OPTION_ON  // Функция для дискретного входа
#define         _CLOCK_SENS_FUNC_       OPTION_OFF  // Функция для счетных датчиков
#define         _VIRT_SENS_FUNC_        OPTION_OFF  // Функция для виртуальных датчиков
#define         _TEMPER_FUNC_           OPTION_OFF  // Функция для температурного датчика
#define         _AVERAGE_FUNC_			OPTION_ON  // Скользящее среднее
#define 		_MB_QUAL_FUNC_			OPTION_ON  // функция качества передачи для ModBus Server
#define         _GZ_RADIUS_FUNC_		OPTION_OFF  // Функция для геозоны - радиус
#define         _GZ_RECT_FUNC_			OPTION_OFF  // Функция для геозоны - квадрат
#define         _MB_FUNC_ 				OPTION_ON  // Функция для чтения ushort из девайса ModBus
#define			_NULL_DATA_FUNC_     	OPTION_ON  //

//--------------------------------------------------------------------------------------------
//----Используемые функции для вторичной обработки данных датчика-----------------------------
//--------------------------------------------------------------------------------------------

#define         _APROC_FUNC_            OPTION_OFF  //  Функция линейно-кусочной апроксимации
#define         _LAPROC_FUNC_           OPTION_OFF  //  Линейная функция
#define         _NULL_FUNC_             OPTION_ON  //  Пустая функция (ничего не делает)
#define         _TANK_FUNC_             OPTION_OFF  //  Функция для определения объема жидкости в бочке с выпуклыми торцами

#endif //  _SENSORS_

/****************************************************/




/*********************FTP СЕРВЕР*********************/

#define _FTP_ 		OPTION_ON

#if(_FTP_ == OPTION_ON)

#define _FULL_FTP_       1

#endif //  _FTP_

/****************************************************/

/******************_HTTP_******************/

#define _HTTP_ DEVICE_ON

#if(_HTTP_ == DEVICE_ON)

	#define		USER_HTTP_REQ_PROC_SPACE		(1024+256)
	#define		HTTP_GZIP_ON	DEVICE_ON
		
#ifdef _PIPELINES_
	static int
	PIPELINES_process(char *request);
#endif

#define CGI_LOGIN_PROCESS 		{"LOGIN", LOGIN_process}
#define CGI_SENSOR_PROCESS 		{"SENSOR", SENSOR_process}
#define CGI_CONFIG_PROCESS 		{"CONFIG", CONFIG_process}
#define CGI_INFO_PROCESS 		{"INFO", INFO_process}
#define CGI_FILE_PROCESS 		{"FILE", FILE_process}
#define CGI_DATETIME_PROCESS 	{"DATETIME", DATETIME_process}
#define CGI_REINIT_PROCESS 		{"REINIT", REINIT_process}
#define CGI_CAMERA_PROCESS 		{"CAMERA", CAMERA_process}
#define CGI_TERMINAL_PROCESS 	{"TERMINAL", TERMINAL_process}
#define CGI_CANSNIFFER_PROCESS 	{"CANSNIFFER", CANSNIFFER_process}
#define CGI_DISP_PROCESS 	{"DISP", DISP_process}

#ifdef _PIPELINES_
	#define CGI_PIPELINES_PROCESS 	{"PIPELINES", PIPELINES_process}	
#endif
	
#ifdef _PIPELINES_

#define PIPELINES_XML_RESP_HEAD  	"<?xml version=\"1.0\"?><body><pipeinfo>"
#define PIPELINES_XML_RESP_TAIL  	"</pipeinfo></body>"

#define PIPELINES_XML_ELEM_BEGIN	"<%s%d>"
#define PIPELINES_XML_ELEM_END		"</%s%d>"

#endif // _PIPELINES_


#endif


/****************************************************/

/*******************SLIP ПРОТОКОЛ*******************/

#define _SLIP_ 		OPTION_OFF

#if(_SLIP_ == OPTION_ON)

#define _DEFAULT_IP_ADDR_       	0x0100A8C0
#define _DEFAULT_GW_IP_ADDR_       	0x0200A8C0

#endif //  _SLIP_

/****************************************************/


/*******************Ping*****************************/

#define _PING_ 		OPTION_ON

#if(_PING_ == OPTION_ON)

		#define			PING_RCV_TIMEO			1

		#define			PING_MIN_TIME		20000
		#define			PING_MAX_TIME		0xFFFFFFFF
		#define			PING_DEF_TIME		60000

#endif //  _PING_

/****************************************************/



/******************_UART_CONTROL_TASK_Т******************/

#define _UART_CONTROL_TASK_ DEVICE_ON

#if(_UART_CONTROL_TASK_ == DEVICE_ON)

	#define UART_CONTROL_DRIVER_NAME "UART1"

#endif
/********************************************************/


/*******************TELNET СЕРВЕР*******************/

#define _TELNET_ 		OPTION_ON

#if(_TELNET_ == OPTION_ON)

#endif //  _FTP_

/**********************SECURITY**********************/

#if(INIT_ON)
	#define _SECURITY_ 		OPTION_OFF
#else
	#define _SECURITY_ 		 OPTION_ON
#endif

#if(_SECURITY_ == OPTION_ON)

	#define		MAX_LOGIN_LENGTH		8
	#define		MAX_PASS_LENGTH			8
	
	#define		ADMIN_LOGIN				"admin"

	#define		PHONE_NUM_QUANTITY		10

	#define 	FNV_32_PRIME 			((uint32_t)0x01000193)
	#define 	FNV_32_INIT_VAL 		((uint32_t)0x811c9dc5)

	#define READ_SERIAL_NUM				58
	#define 	IAP_ADDRESS 			0x1FFF1FF1

#endif //  _SECURITY_

/****************************************************/




/******************МОДУЛЬ АВТОГРАФ******************/

#define _AUTOGRAPH_ 		OPTION_OFF

#if(_AUTOGRAPH_ == OPTION_ON)

	#define     AG_LOG_SIZE             ((_FLASH_SAVER_PAGE_SIZE_ / ARCH_REC_LEN) * ARCH_SEG_SIZE)

	#define		AG_REC_NUM_ON_SEG       (_FLASH_SAVER_PAGE_SIZE_ / ARCH_REC_LEN)

	#define		_ON_BORD_VOL_ 			OPTION_ON

	#define		_ADC_SUPPLY_CH_			5

#endif //  _AUTOGRAPH_

/****************************************************/


/******************МОДУЛЬ NETBIOS******************/

#define _NETBIOS_ 		OPTION_OFF

#if(_NETBIOS_ == OPTION_ON)

	#define NETBIOS_LWIP_NAME "KOT"

#endif //  _NETBIOS_

/****************************************************/

				
/**********************СОБЫТИЯ***********************/

#define _EVENTS_ 		OPTION_ON

#if(_EVENTS_ == OPTION_ON)

	#define		MAX_EVENTS_IN_FILE		30  //  Максимальное число событий заданных в файле конфигурации
	#define		MAX_MESSAGE_SIZE        64

#endif //  _EVENTS_

/****************************************************/




/************************CMC*************************/

#define _SMS_ 		OPTION_ON

#if(_SMS_ == OPTION_ON)
	
#define 		Pipelines_SMS
//#define       SMS_TIME_SAVE       0
//#define       SMS_PORT            1
//#define       SMS_IP              2
#define       SMS_RESET           3
#define       SMS_TIME_SMS        4
#define       SMS_SERIAL          5
#define       SMS_PASSWORD        6
#define       SMS_LOGGPRS_1       7
#define       SMS_STRINIT_1       8
#define       SMS_PASSGPRS_1      9
#define       SMS_IDPROCE         10
//#define       SMS_SIM		      11
#define       SMS_VERS	          12
//#define   	  SMS_TIME_SEND	  	  13
//#define   	  SMS_AN		  	  14
//#define   	  SMS_FREQ	  		  15
//#define   	  SMS_COUNT		  	  16
//#define       SMS_LOGGPRS_2       17
//#define       SMS_STRINIT_2       18
//#define       SMS_PASSGPRS_2      19
#define       SMS_USSD			  20
//#define       SMS_RING_UP		  21
//#define       SMS_TIME_SYNC		  22   
#define		SMS_UST_P1 				21
#define		SMS_UST_S1 				22
#define		SMS_UST_P2 				23
#define		SMS_UST_S2 				24
#define		SMS_UST_P3 				25
#define		SMS_UST_S3 				26
#define		SMS_UST_P4 				27
#define		SMS_UST_S4 				28
#define		SMS_SIM_IP				29
#define		SMS_PORT				30
#define		SMS_TIME_DELTA			31
#define     SMS_AT_COM		  		32
#define     SMS_FW_UPDATE			33
#define     SMS_NUM_USSD			34
#define     SMS_ALL_GPRS_SETTINGS	35

#endif //  _SMS_

/****************************************************/




/************************WDT*************************/
/************************WDT*************************/

#define _WDT_ 		OPTION_ON

#define 	WDT_COUNTERS_NUM		3

#if(_WDT_ == OPTION_ON)

#endif //  _WDT_

#if(_INT_WDT_)
	#define WDT_TOUT_us   (1120000)    
#endif

#if(_EXT_WDT_)
	#define 	EXT_WDT_PORT			4
	#define 	EXT_WDT_PIN			    29
#endif

/****************************************************/


/****************Адрес прошивки и ее версия**********/
/****************Адрес прошивки и ее версия**********/

//#define			VERS_ADDR		(0x000000D4 + 0x5000)
#define			BOOT_VERS_ADDR	(0x000000D4)
/*
Номер прошивки A.B.C.D

A - Версия программного обеспечения
B - Версия платы
С - Версия релиза (добавления, нестандартные решения под когото)
D - Исправления багов

*/

// 3.0.0.0 05.05.2015 - обновленный юсб и все остальное сделанное на этот момент в универсальном регистраторе
// 3.0.0.1 14.07.2015 - исправлен баг wap страницы, в которой могло не закрыться устройство модбасс и оставался захваченный семафор модбасс
//						Точно исправлен пароль
// 3.1.0.1 22.01.2015 - Глобальная актуализация кода, вызванная не ответом на смс SIM_IP (до этого был старый код модема)
// 3.1.0.2 16.06.2016 - Актуализация - исправлен SSP, включена опция backlog в тсп

#define			VERS_ADDR		"3.1.0.2"

/****************************************************/




/*************ПОДДЕРЖКА МНОГОЗАДАЧНОСТИ***************/

#define _MULTITASKING_  OPTION_ON

#if(_MULTITASKING_ == OPTION_ON)

	#define create_sem(sem)								 vSemaphoreCreateBinary(sem)		
	#define take_sem(sem, block_time) 					 xSemaphoreTake(sem, block_time)
	#define give_sem(sem) 								 xSemaphoreGive(sem)
	#define give_sem_from_isr(sem, higher_pr_task_woken) xSemaphoreGiveFromISR(sem, higher_pr_task_woken)
	#define delay(block_time)                            vTaskDelay(block_time)

#else
	
	#define create_sem(sem)								 sem=1		
	#define take_sem(sem, block_time) 					 1
	#define give_sem(sem) 								 
	#define give_sem_from_isr(sem, higher_pr_task_woken)
	#define delay(block_time) 
	
#endif // _MULTITASKING_			

//#endif

/*****************************************************/
/*****************************************************/
/************Сохранение и отправка данных*************/
/*****************************************************/
/*****************************************************/

#define			SAVE_DATA_MIN_TIME		1000
#define			SAVE_DATA_MAX_TIME		600000
#define			SAVE_DATA_DEF_TIME		30000


#define			SEND_DATA_MIN_TIME		60000
#define			SEND_DATA_MAX_TIME		600000
#define			SEND_DATA_DEF_TIME		300000


#define			SMS_CHECK_MIN_TIME		60000
#define			SMS_CHECK_MAX_TIME		3600000
#define			SMS_CHECK_DEF_TIME		300000

#define   SIZE_COMMON_QUEUE       50

#define   COMMON_POOL_TIMER       0
#define   SENSORS_POOL_TIMER      1
#define   LED_TIMER               2
#define   MBS_TIMER		          3
#define   SEND_DATA_TIMER         4
#define   DATA_REMARK             5
#define   MODEM_POOL_TIMER        6
#define   SMS_TIMER               7
#define   LOG                     8
#define   SYSTEM_RESTART          9
#define   TELNET_TIMER            10
#define   ADC_POOL_TIMER          11
#define   BOOT_REQUEST            12
#define   BUTTON				  13
#define   DISPLAY_UPDTAE_TIMER	  14
#define   CLOCK_POOL_TIMER        15
#define   DISCRETE_POOL_TIMER     16
#define   SIG_CHECK_TIMER	      17
#define   RESETE_TO_CLOCK_TIMER	  18
#define   TIME_SYNC_TIMER         19
#define   PING_TIMER			  20
#define   S485					  21
#define   MBS_RW				  22
#define   UART_TERM				  23
#define   DEV_REINIT			  24
#define   WDT_TIMER				  25


/*****************************************************/
/*****************************************************/
/************ДЭФАЙНЫ ДЛЯ ФАЙЛОВ ПАРАМЕТРОВ************/
/*****************************************************/
/*****************************************************/



#define PARAMETRS_FILE_NAME    	"/at45db/DEVCON.ini"
#define SENSOR_FILE_NAME       	"/at45db/SENSORS.ini"
#define EVENTS_FILE_NAME       	"/at45db/EVENTS.ini"
#define TIMERS_FILE_NAME    	"/at45db/TIMERS.ini"
#define AG_LOG_FILE_NAME    	"/at45db/AG/LOG.bin"
#define AG_LOG_DIR_NAME	  		"/at45db/AG"
#define MESSAGES_FILE_NAME      "/at45db/MES.ini"

#define AUTOGRAPH_FILE_NAME 	PARAMETRS_FILE_NAME

#define MAX_SIZE_CONFIG_FILE   2000

#ifndef SEG_RAM_START_SECTOR
#define  SECTORS_NUM  512
#else
#define  SECTORS_NUM   (((SEG_RAM_START_SECTOR * _SAVER_FLASH_PAGE_SIZE_)/_FAT_FLASH_PAGE_SIZE_) /(_FLASH_FAT_SECTOR_SIZE_/ _FAT_FLASH_PAGE_SIZE_))
#endif


/*****************************************************/
/*****************************************************/
/***********ТЕКСТОВЫЙ РЕЖИМ РАБОТЫ********************/
/*****************************************************/
/*****************************************************/

#define _TEXT_  OPTION_ON

#if (_TEXT_ == OPTION_ON)

	#define TEXT_BUF_LENTH			301

    extern char BufferText[TEXT_BUF_LENTH];

    #if(_MULTITASKING_)
		extern 	xSemaphoreHandle xDataText_Sem;
	#else
		extern 	uint8_t xDataText_Sem;
	#endif // _MULTITASKING_

#endif  // _TEXT_


/*****************************************************/
/*****************************************************/
/********************DEBUG ВЫВОД**********************/
/*****************************************************/
/*****************************************************/

#define _DEBUG_OUTPUT_  OPTION_ON

#if (_DEBUG_OUTPUT_ == OPTION_ON)

	#include "debug_system.h"
#else
//	#define DEBUG_PUTS
//	#define DEBUG_PRINTF_ONE
#endif  // _DEBUG_OUTPUT_



/*****************************************************/
/*****************************************************/
/******************НАСТРОЙКА ЛОГА*********************/
/*****************************************************/
/*****************************************************/

#define _LOG_  OPTION_ON

#if (_LOG_ == OPTION_ON)

	void PutMesToLog(char *mes, uint8_t type, uint8_t level);

	#define 		PUT_MES_TO_LOG(x, y, z) 	PutMesToLog(x, y, z)
	#define         FILE_NAME               	"/at45db/LOG.txt"
	#define 		LOG_RECORD_LEN				64

	#define         RECORDS_PER_SECTOR          (_SAVER_FLASH_PAGE_SIZE_ / LOG_RECORD_LEN)
	#define			LOG_MAX_NUMBER_OF_RECORDS	(LOG_SEG_SIZE * RECORDS_PER_SECTOR)	
	#define         ARCH_MAX_RECORDS			(LOG_MAX_NUMBER_OF_RECORDS - RECORDS_PER_SECTOR)    // это количиесто отображается в архиве		

#else
	#define PUT_MES_TO_LOG(x, y, z)
#endif  // _LOG_


//#endif








#define SRAM1  __attribute__ ((section (".new_bss")))

#endif


//#define _LPC1765_          	(1)
//#define _LWIP_PORT_        	1
//#define _FAT_				1
//#define _DEBUG_SYS_         1	







