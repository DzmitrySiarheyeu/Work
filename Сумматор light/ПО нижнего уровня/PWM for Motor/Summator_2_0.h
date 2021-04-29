// Проект: 		Сумматор light 2.0
// Учереждение:	НИЧ БГУИР центр 11.2 Отдел подготовки опытного производства
// Разработал:	Яшин И.Н.
// Дата:		24.05.2011


#ifndef _SUMMATOR_H_
#define _SUMMATOR_H_


#define		STATE_NUM			3   //  Количество режимов ШИМ
#define		LED_PIN_STATE_NUM   4   //  Максимальное Количество изменений состояния пина для светодиода в теч 1с

/**************Константы для инициализации *********************/

#define		TIMER_TICK_H_20		114   //Число тиков для удержания высокого уровня на ноге (ШИМ 20%)
#define		TIMER_TICK_L_20		28  //Число тиков для удержания низкого уровня на ноге (ШИМ 20%)

#define		TIMER_TICK_H_30		92   //Число тиков для удержания высокого уровня на ноге (ШИМ 30%)
#define		TIMER_TICK_L_30		50  //Число тиков для удержания низкого уровня на ноге (ШИМ 30%)

#define		TIMER_TICK_H_0		255   //Число тиков для удержания высокого уровня на ноге (ШИМ 0%)
#define		TIMER_TICK_L_0		1  //Число тиков для удержания низкого уровня на ноге (ШИМ 0%)

#define		TIMER_TICK_20		(TIMER_TICK_H_20 + TIMER_TICK_L_20)  //Число тиков таймера (периуд 10с ШИМ 20%)	
#define		TIMER_TICK_30		(TIMER_TICK_H_30 + TIMER_TICK_L_30)  //Число тиков таймера (периуд 10с ШИМ 30%)	
#define		TIMER_TICK_0		(TIMER_TICK_H_0 + TIMER_TICK_L_0)  //Число тиков таймера (периуд 10с ШИМ 0%)

               /*Для мигания 1 раз в секунду*/
#define		TIMER_TICK_1pS_ST1		2880 // Горим 200 мс
#define		TIMER_TICK_1pS_ST2		11520 // Не горим 800 мс

               /*Для мигания 2 раза в секунду*/
#define		TIMER_TICK_2pS_ST1		2880 // Горим 200 мс
#define		TIMER_TICK_2pS_ST2		1440 // Не горим 100 мс
#define		TIMER_TICK_2pS_ST3		2880 // Горим 200 мс
#define		TIMER_TICK_2pS_ST4		7200 // Не горим 500 мс	

               /*Для мигания 3 раза в секунду*/
#define		TIMER_TICK_3pS_ST1		7200 // Горим 500 мс
#define		TIMER_TICK_3pS_ST2		7200 // Не горим 500 мс

				/*Состояния кнопки*/
#define		KEY_PRESSED				0   // Кнопка нажата
#define		KEY_PRESSING			1   // Кнопка нажимается (ждем некоторое время для устранения дребизга)
#define		KEY_FREE				2   // Кнопка отпущена
#define		KEY_FREEING				3   // Кнопка отпускается (ждем некоторое время для устранения дребизга)
#define		KEY_WAIT_FREE			4   // Кнопка ждет отпускания


#define		TIME_BOUNCE_DOWN		10  //Время в десятках мс для устранения дребезга



typedef struct tagPWMInfo
{
	unsigned char	count[2];  //  Числа до которых должен считать таймер для формирования сигнала
}PWM_INFO;


typedef struct tagLEDInfo
{
	unsigned short	count[LED_PIN_STATE_NUM];  //  Числа до которых должен считать таймер для нужного мигания диодом
}LED_INFO;


typedef struct tagSumInfo
{
	unsigned char	state;    		//  Текущий режим
	unsigned char	line_state;		//  Текущее состояние выходного пина
	PWM_INFO		pwm[STATE_NUM]; //  Два режима шим
	LED_INFO		led[STATE_NUM]; //
	unsigned char	led_state_count;
}SUM_INFO;


typedef struct tagKeyInfo
{
	unsigned char	state;			//  состояние кнопки
	unsigned short	count;			//  счетчик времени в определнном состоянии
}KEY_INFO;


#endif
