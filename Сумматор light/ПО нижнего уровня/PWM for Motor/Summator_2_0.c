#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h> 
#include <avr/wdt.h> 
#include <avr/interrupt.h>
//#include "base_type.h"
#include "Summator_2_0.h"


volatile SUM_INFO	Sum_Info;
volatile KEY_INFO	Key_info;



void InitTimers(void)
{

	//Таймер 0 для опроса кнопки
	TIMSK0 |= (1 << OCIE0A);
	TCCR0A = (1 << WGM01);
	TCCR0B = ((1 << CS02)|(0 << CS01)|(1 << CS00));
	OCR0A = 144;

	//Таймер 1 для мигания светодиодом	
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS12) | (0 << CS11) | (1 << CS10);
	TIMSK1 = (1 << OCIE1A);
	OCR1A = 1000;

	


	//Таймер 2 для генерации требуемого сигнала
	TIMSK2 |= (1 << OCIE2B);
	TCCR2A = (1 << WGM21) | (1 << COM2B0);
	TCCR2B = ((1 << CS22)|(1 << CS21)|(1 << CS20));
	OCR2B = 10;
}

void InitPorts(void)
{
	// Порт B оставляем без изменений

	DDRC = 0x38;  // 3,4,5 пины - выходы
	PORTC &= ~((1 << 3) | (1 << 4) | (1 << 5));  //  Сбрасываем эти выходы в ноль

	DDRD = 0x08;   // 3 пин - выход
	PORTD &= ~(1 << 3); // Скидываем выход в ноль
}

void InitInfoStructs(void)
{
	Sum_Info.state = 0;
	Sum_Info.line_state = 0;

	Sum_Info.pwm[0].count[0] = TIMER_TICK_H_20;
	Sum_Info.pwm[0].count[1] = TIMER_TICK_L_20;

	Sum_Info.pwm[1].count[0] = TIMER_TICK_H_30;
	Sum_Info.pwm[1].count[1] = TIMER_TICK_L_30;

	Sum_Info.pwm[2].count[0] = TIMER_TICK_H_0;
	Sum_Info.pwm[2].count[1] = TIMER_TICK_L_0;

	Sum_Info.led[0].count[0] = TIMER_TICK_1pS_ST1;
	Sum_Info.led[0].count[1] = TIMER_TICK_1pS_ST2;
	Sum_Info.led[0].count[2] = TIMER_TICK_1pS_ST1;
	Sum_Info.led[0].count[3] = TIMER_TICK_1pS_ST2;

	Sum_Info.led[1].count[0] = TIMER_TICK_2pS_ST1;
	Sum_Info.led[1].count[1] = TIMER_TICK_2pS_ST2;
	Sum_Info.led[1].count[2] = TIMER_TICK_2pS_ST3;
	Sum_Info.led[1].count[3] = TIMER_TICK_2pS_ST4;

	Sum_Info.led[2].count[0] = TIMER_TICK_3pS_ST1;
	Sum_Info.led[2].count[1] = TIMER_TICK_3pS_ST2;
	Sum_Info.led[2].count[2] = TIMER_TICK_3pS_ST1;
	Sum_Info.led[2].count[3] = TIMER_TICK_3pS_ST2;

	Key_info.state = KEY_FREE;
	Key_info.count = 0;
}


int main(void)
{
	InitPorts();
	InitInfoStructs();
	InitTimers();
	sei();
	while(1) wdt_reset();
	return 0;
}


SIGNAL(TIMER0_COMPA_vect)
{

	unsigned char pin_flag = (PINB & 0x01);   //  Запоминаем что на ноге
	switch(Key_info.state)
	{
		case KEY_PRESSED:                     // Если кнопка нажата, нужно дождаться ее отпускания
			Key_info.state = KEY_WAIT_FREE;   // перехожим в состояние ожидания отпускания
			break;
		case KEY_PRESSING:                   //  Фиксируется нажатие кнопки
			if(pin_flag)                    //  Кнопка должна пробыть в нажатом состоянии не менее 100мс для принятия нажатия
			{
				Key_info.count++;
				if(Key_info.count >= TIME_BOUNCE_DOWN)   // Если продержались 100 мс - значит кнопка устойчиво нажата
				{
					Sum_Info.state++;                    //  меняем состояние
					if(Sum_Info.state == 2)
					{
						TCCR2B = 0;
					}
					else
					{
						TCCR2B = ((1 << CS22)|(1 << CS21)|(1 << CS20));
					}
					if(Sum_Info.state >= STATE_NUM) Sum_Info.state = 0;   //  Обеспечиваем круговое изменение состояния
					Key_info.state = KEY_PRESSED;   // Кнопка нажата и нажатие зафиксировано
				}
			}
			else  //  Если идет дребезг или изменение уровня вызвано помехой - пологаем что кнопка отпущена
			{
				Key_info.count = 0;
				Key_info.state = KEY_FREE;
			}
			break;
		case KEY_FREE:   // В этом состоянии кнопка отпущена
			if(pin_flag) Key_info.state = KEY_PRESSING;  // Если вдруг видим высокий уровень - пологаем что кнопку нажимают
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
			if(!pin_flag) Key_info.state = KEY_FREEING;
			break;
	}

}



SIGNAL(TIMER1_COMPA_vect)
{
	
	OCR1A = Sum_Info.led[Sum_Info.state].count[Sum_Info.led_state_count];

	if(Sum_Info.led_state_count & 0x01)  // Если последний бит = 1 значит состояние не четное
		PORTC &= ~(1 << 3);				 // и необходимо зажечь диод
	else
		PORTC |= (1 << 3);

	Sum_Info.led_state_count++;
	if(Sum_Info.led_state_count >= LED_PIN_STATE_NUM)
		Sum_Info.led_state_count = 0;
}


SIGNAL(TIMER2_COMPB_vect)
{
	if(Sum_Info.line_state == 0)  
	{
		OCR2B = Sum_Info.pwm[Sum_Info.state].count[0];
		Sum_Info.line_state = 1;
		//TCCR2B |= ((1 << CS22)|(1 << CS21)|(1 << CS20));
	}
	else if(Sum_Info.line_state == 1)
	{
		OCR2B = Sum_Info.pwm[Sum_Info.state].count[1];
		Sum_Info.line_state = 0;
		//TCCR2B |= ((1 << CS22)|(1 << CS21)|(1 << CS20));
	}

	TCNT2 = 0;
}




