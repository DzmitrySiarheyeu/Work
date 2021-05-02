#include <stdint.h>



#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

#define		K_A_0			0x04
#define		K_B_0			0x06
#define		K_C_0			0x08
#define		K_A_1			0x0a
#define		K_B_1			0x0c
#define		K_C_1			0x0e
#define		K_ALPHA_F		0x10
#define		K_ALPHA_A_0   	0x12
#define		K_ALPHA_A_1   	0x14
#define		MB_ID       	0x18
#define		RESERV_1      	0x19
#define		RESERV_2       	0x1b
#define		RESERV_3       	0x1d
#define		ERROR       	0x1f
#define		VERSION       	0x20
#define		WP_REG      	0x22
#define		PULL_UP     	0x23
#define		F_OUT     		0x24
#define		F_OUT_PIN   	0x25
#define		BT_STATE	   	0x26
#define		BT_IMP_CNT	   	0x27
#define		BT_IMP_CNT_RS  	0x29


#define		BOUDRATE 			19200
#define		BUTTONSTATE_LIMIT	4
#define		STATE_NUM			4   //  Количество состояний

#define		NUM_ADC_CHANNEL		2


typedef struct tagEpp_reg
{
	double A[NUM_ADC_CHANNEL];
	double B[NUM_ADC_CHANNEL];
	double C[NUM_ADC_CHANNEL];
	double Alpha_F;
	double Alpha_A[NUM_ADC_CHANNEL];
	unsigned char  	ID;
	unsigned long   Version;
	uint16_t CRC;	
}EPP_REG;


typedef struct tagKeyInfo
{
	unsigned char	state;			//  состояние кнопки
	unsigned short	count;			//  счетчик времени в определнном состоянии
	unsigned char	state_flag;		//  устанавливается для оповещения что кнопка нажата
}KEY_INFO;

				/*Состояния кнопки*/
#define		KEY_PRESSED				0   // Кнопка нажата
#define		KEY_PRESSING			1   // Кнопка нажимается (ждем некоторое время для устранения дребизга)
#define		KEY_FREE				2   // Кнопка отпущена
#define		KEY_FREEING				3   // Кнопка отпускается (ждем некоторое время для устранения дребизга)
#define		KEY_WAIT_FREE			4   // Кнопка ждет отпускания


#define		TIME_BOUNCE_DOWN		329  //

#define		PULL_UP_PIN				PB1
#define		PULL_UP_PORT			PORTB
#define		PULL_UP_DDR				DDRB

#define		BUTTON_PIN				PC3
#define		BUTTON_PORT				PORTC
#define		BUTTON_DDR				DDRC
#define     BUTTON_ISR_PIN			PCINT11
#define     BUTTON_ISR_GROUP		PCIE1  //(PCINT8 - PCINT15)

#define		LED_232_PIN				PD7
#define		LED_232_PORT			PORTD
#define		LED_232_DDR				DDRD

#define		LED_485_PIN				PD6
#define		LED_485_PORT			PORTD
#define		LED_485_DDR				DDRD

#define		LED_U_F_PIN				PD5
#define		LED_U_F_PORT			PORTD
#define		LED_U_F_DDR				DDRD

#define		MUX_MK_PIN				PD3
#define		MUX_MK_PORT				PORTD
#define		MUX_MK_DDR				DDRD

#define		MUX_RS_PIN				PD4
#define		MUX_RS_PORT				PORTD
#define		MUX_RS_DDR				DDRD


#define		BUTTON_STATE_232		0
#define		BUTTON_STATE_485		1
#define		BUTTON_STATE_F			2
#define		BUTTON_STATE_U			3


#define	ON	1
#define	OFF	0

//определяем порт и пин частотного выхода

#define FREQ_OUT_PORT     		PORTB
#define FREQ_OUT_PIN	   		PB2
#define FREQ_OUT_DDR	   		DDRB

//определяем максимальное значение измерительного таймера
#define MAX_TIMER_VALUE	65535

//определяем OCR регистр частотного выхода
#define FREQ_OUT_OCR	OCR1B

//определяем макрос переключения частотного выхода
#define FREQ_OUT_TOGGLE()	{ (TCCR1A ^= (1<<COM1B0)); }	//Set/Clear OC1A/OC1B on Compare Match
#define FREQ_OUT_OFF()		{ (TCCR1A &= ~((1<<COM1B0) | (1<<COM1B1)));}
#define FREQ_OUT_ON()	{ (TCCR1A |= (1<<COM1B1)); }


typedef struct tagDIO_Register
{
	double Freq;
	double ADC_Volt;
	double ADC_Shunt;
	double A[NUM_ADC_CHANNEL];
	double B[NUM_ADC_CHANNEL];
	double C[NUM_ADC_CHANNEL];
	double Alpha_Freq;
	double Alpha_ADC[NUM_ADC_CHANNEL];
	unsigned short ID;
	double Reserve_1;
	double Reserve_2;
	double Reserve_3;
	unsigned short Error;
	unsigned long Version;
	unsigned short wp_reg;
	unsigned short pull_up_reg;
	unsigned short fout_reg;
	unsigned short fout_pin_reg;
	unsigned short bt_state_reg;
	unsigned long  imp_cnt;
	unsigned short imp_cnt_reset;

}DIO_REGISTR;

typedef union tagMBDIO_Registr
{
	DIO_REGISTR Reg;
	unsigned short DIO_array[sizeof(DIO_REGISTR)/2];
}MBDIO_REGISTR;

extern MBDIO_REGISTR MB_REG;

//********************************
extern EPP_REG Ram_reg;

//********************************
extern double  ConvertFormat(double Value);
extern void InitUART(void);
extern void InitConstant();
extern int Mod_process(unsigned char REG);
extern int PassCheck(uint8_t REG);
