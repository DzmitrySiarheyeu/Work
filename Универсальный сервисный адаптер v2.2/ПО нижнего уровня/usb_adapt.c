#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "mbfanction0.h"
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include "base_type.h"
#include <string.h>
#include "mbclient.h"
#include "usb_adapt.h"
#include "ADC.h"
#include "Freq.h"
#include "util.h"
#include "fout.h"


//#define	_DOT_STEND_

#define DEBUG_
#define CRC_CODE_FLASH 0xD874

MBDIO_REGISTR MB_REG;

#define R_VERX	30000.0
#define R_NIZ	10000.0
#define R_SHUNT	0.47
#define K_GAIN	100.0
#define INIT_B_0 ((5.033/1024.0)*3.014)
#define INIT_B_1 ((5.033/1024.0)/(R_SHUNT*K_GAIN))

uint16_t CRC_FLASH	__attribute__ ((section (".crccode"))) = CRC_CODE_FLASH;

EPP_REG Epp_reg __attribute__ ((section (".also_var"))) = {.A[0] = 0, .B[0] = INIT_B_0, .C[0] = 0, .A[1] = 0, .B[1] = INIT_B_1, .C[1] = 0, .Alpha_F = 0.7, .Alpha_A[0] = 0.05, .Alpha_A[1] = 0.05, .ID = 99, .Version = 06022013, .CRC = 28400};

EPP_REG Epp_reg __attribute__ ((section (".also_var")));

EPP_REG Ram_reg;

__ATTR_EEPROM__ byte  Button_state;

byte  Bt_state;

void ButtonProc(void);


volatile KEY_INFO	Key_info;
//Инициализация UART
void InitUART(void)
{
	unsigned long BRATE;

	MBClientSetHostID0(Ram_reg.ID);
	//MBClientSetHostID0(eeprom_read_byte((void*)&EEP_ID));
	//код для инициализации нулевого UARTa
#ifdef _DOT_STEND_
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
#endif
	pUDR0 = &UDR0;
//	cbi(DE485_0_PORT, DE485_0_PIN);
//	DE485_0_DDR |= (1 << DE485_0_PIN);
	//eeprom_read_block(&BRATE, (void*)&EEP_Boudrate, sizeof(BRATE));
	BRATE = BOUDRATE;
	MBClientSetBaudrate0((uint8_t)(F_CPU/16/BRATE - 1));



}


//инициализация портов
void InitPorts (void)		
{

   	DDRD = 0x06;
   	DDRB |= 0x10;

	FREQ_OUT_PORT &= ~(1<<FREQ_OUT_PIN);	//устанавливается 0 на выходе
	FREQ_OUT_DDR |= (1<<FREQ_OUT_PIN);		//порт программируется как выход

	PULL_UP_DDR |= (1 << PULL_UP_PIN);
	PULL_UP_PORT &= ~(1 << PULL_UP_PIN);

   	BUTTON_DDR &= ~(1 << BUTTON_PIN);
	BUTTON_PORT |= (1 << BUTTON_PIN);

	PCICR = (1 << BUTTON_ISR_GROUP);    		//  Разрешаем прерывание для группы
	PCMSK1 = (1 << BUTTON_ISR_PIN);  	//  Разрешаем прерывание для конкретной ноги
	PCIFR = 0;									//Сбрасываем флаг прерывания по ногам

	LED_232_DDR |= (1 << LED_232_PIN);
	LED_485_DDR |= (1 << LED_485_PIN);
	LED_U_F_DDR |= (1 << LED_U_F_PIN);

	LED_232_PORT |= (1 << LED_232_PIN);
	LED_485_PORT |= (1 << LED_485_PIN);
	LED_U_F_PORT |= (1 << LED_U_F_PIN);

	MUX_MK_PORT &= ~(1 << MUX_MK_PIN);
	MUX_RS_PORT &= ~(1 << MUX_RS_PIN);

	MUX_MK_DDR |= (1 << MUX_MK_PIN);
	MUX_RS_DDR |= (1 << MUX_RS_PIN);

}
	




void InitTimer_Freq(void)
{
	//Таймер 1 в режиме захвата для ДОТ1	
//	TCCR1A = 0;
//	TCCR1B = (1 << ICNC1) | (1 << ICES1) | (0 << CS12) | (0 << CS11) | (1 << CS10);
//	TIMSK1 |= (1 << TOIE1) | (1 << ICIE1);
//	ACSR = 0;

	
	TCCR1A = (1 << COM1B1); // пока не выставиться флаг startFlag на ножке будет 0	
	TCCR1B = (1 << CS10) | (1 << ICES1) | (1 << ICNC1) ; // предделитель = 1
	TIMSK1 = (1 << TOIE1) | (1 << OCIE1B) | (1 << ICIE1);
	TCNT1 = 0;
	ACSR = 0;
}

void InitTimer_ModBus(void)
{	
	//прерывание modbus  Таймер 0
	TIMSK0 |= (1 << OCIE0A);
	TCCR0A=(1 << WGM01);
	TCCR0B = (1 << CS01) | (1 << CS00) ;
	OCR0A = 70;

}

void InitTimer_ADC(void)
{
	//Установка второго таймера на F=1024 Hz
	TIMSK2 |= (1 << OCIE2A); //прерывание на сравнение с OCR
	TCCR2A = (1 << WGM22);
	ASSR = 0;
	TCCR2B = (1 << CS22)|(0 << CS21)|(0 << CS20);
	OCR2A = 200; 
}



double  ConvertFormat(double Value)
{
	double TempValue;

	*((unsigned char *)&TempValue + 3) = *((unsigned char *)&Value    );
	*((unsigned char *)&TempValue + 2) = *((unsigned char *)&Value + 1);
	*((unsigned char *)&TempValue + 1) = *((unsigned char *)&Value + 2);
	*((unsigned char *)&TempValue + 0) = *((unsigned char *)&Value + 3);

	return ((double)TempValue);	
}

long  ConvertFormatForLong(long Value)
{
	long TempValue;

	*((unsigned char *)&TempValue + 3) = *((unsigned char *)&Value    );
	*((unsigned char *)&TempValue + 2) = *((unsigned char *)&Value + 1);
	*((unsigned char *)&TempValue + 1) = *((unsigned char *)&Value + 2);
	*((unsigned char *)&TempValue + 0) = *((unsigned char *)&Value + 3);

	return ((long)TempValue);	
}

int  ConvertFormatForInt(int Value)
{
	int TempValue;

	
	*((unsigned char *)&TempValue + 1) = *((unsigned char *)&Value + 0);
	*((unsigned char *)&TempValue + 0) = *((unsigned char *)&Value + 1);

	return ((int)TempValue);	
}



uint16_t crc_flash_count(uint8_t *addr, uint16_t len)
{
	uint16_t crc = 0xFFFF;
	uint8_t buf[8];
		// читаем кусками по 8 байт для вычисления crc 
	uint8_t bytes_to_read;
	while(len != 0)
	{
		if(len > 8)
		{
			bytes_to_read = 8;
			len -= 8;
		}
		else
		{
			bytes_to_read = len;
			len = 0; // выход из цикла
		}
		
		// читает кусок
		GetMemBuf(buf, (PGM_VOID_P)addr, bytes_to_read);
		// считаем кусок crc
		crc = CRC16(buf, bytes_to_read, crc);	

		addr += bytes_to_read;
	}

	return crc;
}

void Write_falsh(void)
{
	unsigned short crc = 0xFFFF;
	crc = CRC16((uint8_t *)&Ram_reg, sizeof(EPP_REG) - 2, 0xFFFF);
	Ram_reg.CRC = crc;
	SetMemBuf((PGM_VOID_P)&Epp_reg, (uint8_t *)&Ram_reg, sizeof(EPP_REG));
}


void Switch_Led(void)
{
	PORTD |= ((1 << LED_232_PIN)|(1 << LED_485_PIN)|(1 << LED_U_F_PIN));   //  Тушим все светодиоды
	switch(Bt_state)
	{
		case BUTTON_STATE_232:
			LED_232_PORT &= ~(1 << LED_232_PIN);
			break;
		case BUTTON_STATE_485:
			LED_485_PORT &= ~(1 << LED_485_PIN);
			break;
		case BUTTON_STATE_U:
			LED_U_F_PORT &= ~(1 << LED_U_F_PIN);
			PULL_UP_PORT |= (1 << PULL_UP_PIN);
			break;
		case BUTTON_STATE_F:
			LED_U_F_PORT &= ~(1 << LED_U_F_PIN);
			PULL_UP_PORT &= ~(1 << PULL_UP_PIN);
			break;
		default:
			LED_232_PORT &= ~(1 << LED_232_PIN);
			break;
	}
}

void Switch_MUX(void)
{
	switch(Bt_state)
	{
		case BUTTON_STATE_232:
			MUX_RS_PORT &= ~(1 << MUX_RS_PIN);
			MUX_MK_PORT |=  (1 << MUX_MK_PIN);
#ifndef _DOT_STEND_
			UCSR0B &= ~((1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0));
#endif
			break;
		case BUTTON_STATE_485:
			MUX_RS_PORT |=  (1 << MUX_RS_PIN);
			MUX_MK_PORT |=  (1 << MUX_MK_PIN);
#ifndef _DOT_STEND_
			UCSR0B &= ~((1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0));
#endif
			break;
		case BUTTON_STATE_U:
		case BUTTON_STATE_F:
			MUX_MK_PORT &= ~(1 << MUX_MK_PIN);
			MUX_RS_PORT |=  (1 << MUX_RS_PIN);
#ifndef _DOT_STEND_
			UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
#endif
			break;
		default:
			break;
	}
}




void InitConstant(void)
{

	uint16_t len = (uint16_t)&Epp_reg.CRC - (uint16_t)&Epp_reg;

	uint16_t crc = crc_flash_count((uint8_t *)&Epp_reg, len);


	uint16_t crc2;		
	//читаем crc2
	GetMemBuf((uint8_t *)&crc2, (PGM_VOID_P)(&Epp_reg.CRC), 2);	

	if(crc != crc2)
	{
		MB_REG.Reg.Error = ConvertFormatForInt((int)1);
		Ram_reg.A[0] = 0;
		Ram_reg.B[0] = INIT_B_0;
		Ram_reg.C[0] = 0;
		Ram_reg.A[1] = 0;
		Ram_reg.B[1] = INIT_B_1;
		Ram_reg.C[1] = 0;
		Ram_reg.Alpha_F = 0.7;
		Ram_reg.Alpha_A[0] = 0.05;
		Ram_reg.Alpha_A[1] = 0.05;
		Ram_reg.ID = 99;
		Ram_reg.Version = 06022013; 
		Ram_reg.CRC = 28400;
		SetMemBuf((PGM_VOID_P)&Epp_reg, (uint8_t *)&Ram_reg, sizeof(EPP_REG));
	}
	else
	{
		MB_REG.Reg.Error = ConvertFormatForInt((int)0);				//верная контрольная сумма
		GetMemBuf((uint8_t *)&Ram_reg, (PGM_VOID_P)(&Epp_reg), sizeof(EPP_REG));
	}


	MB_REG.Reg.A[0] = ConvertFormat(Ram_reg.A[0]);
	MB_REG.Reg.B[0] = ConvertFormat(Ram_reg.B[0]);
	MB_REG.Reg.C[0] = ConvertFormat(Ram_reg.C[0]);
	MB_REG.Reg.A[1] = ConvertFormat(Ram_reg.A[1]);
	MB_REG.Reg.B[1] = ConvertFormat(Ram_reg.B[1]);
	MB_REG.Reg.C[1] = ConvertFormat(Ram_reg.C[1]);
	MB_REG.Reg.Alpha_Freq = ConvertFormat(Ram_reg.Alpha_F);
	MB_REG.Reg.Alpha_ADC[0] = ConvertFormat(Ram_reg.Alpha_A[0]);
	MB_REG.Reg.Alpha_ADC[1] = ConvertFormat(Ram_reg.Alpha_A[1]);
	MB_REG.Reg.ID = ConvertFormatForInt(Ram_reg.ID);   
	MB_REG.Reg.Version = (unsigned long)ConvertFormatForLong(Ram_reg.Version); 
	MB_REG.Reg.wp_reg = 0; 
	MB_REG.Reg.pull_up_reg = ConvertFormatForInt(1);
	MB_REG.Reg.fout_reg = ConvertFormatForInt(0);
	MB_REG.Reg.fout_pin_reg = ConvertFormatForInt(0);
	MB_REG.Reg.imp_cnt = ConvertFormatForInt(0);
	MB_REG.Reg.imp_cnt_reset = ConvertFormatForInt(0);
	
	Key_info.state = KEY_FREE;
	Key_info.count = 0;  
	Key_info.state_flag = 0; 
	
	Bt_state = eeprom_read_byte(&Button_state);
	if(Bt_state >= BUTTONSTATE_LIMIT){
		Bt_state = 0;
		eeprom_busy_wait();
		eeprom_write_byte(&Button_state, Bt_state);
	}
	
	MB_REG.Reg.bt_state_reg = ConvertFormatForInt(Bt_state);	  	
}

int PassCheck(uint8_t REG)
{
	if(REG == WP_REG) return 0;
	if(ConvertFormatForInt(MB_REG.Reg.wp_reg) != 123)
		return -1;
	else return 0;
}

int Mod_process(uint8_t REG)   // 
{
	uint16_t temp = 0;
	if(REG == WP_REG) return 0;

	MB_REG.Reg.wp_reg = 0;

	if(REG == BT_STATE){
		temp = ConvertFormatForInt(MB_REG.Reg.bt_state_reg);
		if(temp >= BUTTONSTATE_LIMIT) return -1;
		Bt_state = temp;
		Key_info.state_flag = 1;
		return 0;
	}

	if(REG == F_OUT){
		set_Freq(ConvertFormatForInt(MB_REG.Reg.fout_reg));
		MB_REG.Reg.fout_pin_reg = ConvertFormatForInt(0);
		return 0;
	}
	if(REG == PULL_UP){
		if(ConvertFormatForInt(MB_REG.Reg.pull_up_reg) == 0){
			PULL_UP_PORT |= (1 << PULL_UP_PIN);
			Bt_state = BUTTON_STATE_U;
		}
		else{
			PULL_UP_PORT &= ~(1 << PULL_UP_PIN);
			Bt_state = BUTTON_STATE_F;
		}
		Key_info.state_flag = 1;
		ButtonProc();
		return 0;
	}
	if(REG == F_OUT_PIN){
		if(ConvertFormatForInt(MB_REG.Reg.fout_reg) == 0)
		{
			if(ConvertFormatForInt(MB_REG.Reg.fout_pin_reg) == 0)
				ClearFreqOut();
			else SetFreqOut();

			return 0;
		}
		else
		{
			MB_REG.Reg.fout_pin_reg = ConvertFormatForInt(0);
			return -1;
		}
	}

	if(REG == BT_IMP_CNT_RS){
		if(MB_REG.Reg.imp_cnt_reset == 1){
			MB_REG.Reg.imp_cnt = ConvertFormatForInt(0);
			MB_REG.Reg.imp_cnt_reset = ConvertFormatForInt(0);
		}
	}

	switch (REG)   // Смотрим в какой регистр произошла запись
	{
		case K_A_0:
			Ram_reg.A[0] = ConvertFormat(MB_REG.Reg.A[0]);
			break;
		case K_B_0:
			Ram_reg.B[0] = ConvertFormat(MB_REG.Reg.B[0]);
			break;
		case K_C_0:
			Ram_reg.C[0] = ConvertFormat(MB_REG.Reg.C[0]);
			break;
		case K_A_1:
			Ram_reg.A[1] = ConvertFormat(MB_REG.Reg.A[1]);
			break;
		case K_B_1:
			Ram_reg.B[1] = ConvertFormat(MB_REG.Reg.B[1]);
			break;
		case K_C_1:
			Ram_reg.C[1] = ConvertFormat(MB_REG.Reg.C[1]);
			break;
		case K_ALPHA_F:
			Ram_reg.Alpha_F = ConvertFormat(MB_REG.Reg.Alpha_Freq);
			break;
		case K_ALPHA_A_0:
			Ram_reg.Alpha_A[0] = ConvertFormat(MB_REG.Reg.Alpha_ADC[0]);
			break;
		case K_ALPHA_A_1:
			Ram_reg.Alpha_A[1] = ConvertFormat(MB_REG.Reg.Alpha_ADC[1]);
			break;
		case MB_ID:
			Ram_reg.ID = ConvertFormatForInt(MB_REG.Reg.ID);
			break;
		default:
			return 0;
	}
	
	Write_falsh();
	return 0;
}

inline void ADCProc(void)
{
	volatile double temp;

	temp = ADCGetRoundedValue(0);
	MB_REG.Reg.ADC_Volt = ConvertFormat(temp);

	temp = ADCGetRoundedValue(1);
	MB_REG.Reg.ADC_Shunt = ConvertFormat(temp);
}

void ButtonProc(void)
{
	if(Key_info.state_flag)
	{
		Key_info.state_flag = 0;
		Switch_Led();
		Switch_MUX();
		eeprom_busy_wait();
		eeprom_write_byte(&Button_state, Bt_state);	 //  Сохраняем в памяти новое состояние кнопки
		MB_REG.Reg.bt_state_reg = ConvertFormatForInt(Bt_state);
	}
}


int main(void)
{
	
	unsigned char Check_flag = 0;

#ifndef DEBUG	
	wdt_enable(WDTO_1S);
	wdt_reset();
#endif
	InitPorts();
	InitConstant();
	ADCInit();	
	InitTimer_ModBus();
	InitTimer_Freq();	
	InitTimer_ADC();
	InitUART();
	Switch_Led();
	Switch_MUX();
	set_Freq(0);
	ClearFreqOut();
	sei();

	

	while(1)
	{
#ifndef DEBUG			
			wdt_reset();
#endif
			Check_flag = MBClientReceive0();
			if(Check_flag)
			{
				PORTB ^= (1 << 4);
				MBClientProcess(DataBuffer0+1, Check_flag); // Обрабатываем пакет с первого порта	
				Check_flag = 0;
			}	
	    FREQProcess(); 
		ADCProc();
		ButtonProc();
	}
	return 0;	//
}




//частота 3290 Гц
SIGNAL(TIMER0_COMPA_vect)
{
	MBClientIncTimer0();

	unsigned char pin_flag = (PINC & (1 << BUTTON_PIN));   //  Запоминаем что на ноге
	switch(Key_info.state)
	{
		case KEY_PRESSED:                     // Если кнопка нажата, нужно дождаться ее отпускания
			Key_info.state = KEY_WAIT_FREE;   // перехожим в состояние ожидания отпускания
			break;
		case KEY_PRESSING:                   //  Фиксируется нажатие кнопки
			if(!pin_flag)                    //  Кнопка должна пробыть в нажатом состоянии не менее 100мс для принятия нажатия
			{
				Key_info.count++;
				if(Key_info.count >= TIME_BOUNCE_DOWN)   // Если продержались 100 мс - значит кнопка устойчиво нажата
				{
					Bt_state++;                    //  меняем состояние
					if(Bt_state >= STATE_NUM) Bt_state = 0;   //  Обеспечиваем круговое изменение состояния
					Key_info.state = KEY_PRESSED;   // Кнопка нажата и нажатие зафиксировано
					Key_info.state_flag = 1;  //  Устанавливаем флаг для оповещения о нажатии кнопки
				}
			}
			else  //  Если идет дребезг или изменение уровня вызвано помехой - пологаем что кнопка отпущена
			{
				Key_info.count = 0;
				Key_info.state = KEY_FREE;
			}
			break;
		case KEY_FREE:   // В этом состоянии кнопка отпущена
			if(!pin_flag) Key_info.state = KEY_PRESSING;  // Если вдруг видим высокий уровень - пологаем что кнопку нажимают
			break;
		case KEY_FREEING:  // Кнопка отпускается. 
			if(!pin_flag)
			{
				Key_info.count++;
				if(Key_info.count >= TIME_BOUNCE_DOWN)  // Если четко в течении 100 мс фиксируем что кнопка отпущена
				{                                       // значит она отпущена 
					Key_info.state = KEY_FREE;
				}
			}
			else Key_info.count = 0;
			break;
		case KEY_WAIT_FREE:  // Ожидаем отпускания кнопки
			if(pin_flag) Key_info.state = KEY_FREEING;
			break;
	}
}

SIGNAL(TIMER2_COMPA_vect)
{
	//static unsigned char flag = 0;
	static unsigned short count = 0;
	static unsigned short count_uf = 0;
	IsrTimeOutADC();
	count++;
	count_uf++;
	if(count >= 1150)
	{
		PORTD ^= (1 << 2);
		count = 0;
	}
	if(count_uf >= 100)
	{
		if(Bt_state == BUTTON_STATE_U)
		{
			LED_U_F_PORT ^= (1 << LED_U_F_PIN);	
		}
		count_uf = 0;
	}
}

ISR (TIMER1_COMPB_vect)
{
	IsrFout();
}

ISR(PCINT1_vect)
{
	static uint8_t x;
	uint8_t y, z, tmp;
	unsigned long temp_cnt = 0;

	y = BUTTON_PORT;

	z = x & (~y); 

	tmp = (z & (1 << BUTTON_PIN)) && 1;
	if(tmp != 0)
	{
		temp_cnt = ConvertFormatForInt(MB_REG.Reg.imp_cnt);
		temp_cnt++;
		MB_REG.Reg.imp_cnt = ConvertFormatForInt(temp_cnt);
	}
}


 



