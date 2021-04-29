#include "usart.h"
#include "mbfanction0.h"
#include "omnicomm.h"

#define PORT_DE PORTD
#define DDR_DE DDRD
#define PIN_DE 2

__ALSO_VAR__ volatile byte timer0;
__ALSO_VAR__ volatile byte *pUDR0;
__ALSO_VAR__ volatile byte * volatile pBuf0;
byte DataBuffer0[ MBCLIENT_MAX_LENGHT ];
__ALSO_VAR__ volatile byte BufferSize0 = 0;

__ALSO_VAR__ volatile byte state0 = State_INITIAL;

void SET_DE485_0(void){ cbi(UCSR0B, RXEN0); sbi(PORT_DE, PIN_DE); }
void CLR_DE485_0(void){ cbi(PORT_DE, PIN_DE); sbi(UCSR0B, RXEN0); }


//Инициализация UART
void InitUART(void)
{
	//unsigned long BRATE;

	// настройка UART
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
	pUDR0 = &UDR0;
	// настройка порта 
	cbi(PORT_DE, PIN_DE);
	DDR_DE |= (1 << PIN_DE);
	// настройка скорости передачи
	//eeprom_read_block(&BRATE, (void*)&EEP_Boudrate0, sizeof(BRATE));
	MBClientSetBaudrate0((byte)(F_CPU/16/19200 - 1));
	//код устройства задается
	MBClientSetHostID0(99);//eeprom_read_byte((void*)&EEP_ID0));

	//есть целая теория почему так должно быть
	PORTD=1<<0;

	sbi(TIMSK0, TOIE0);	
}

SIGNAL(SIG_OVERFLOW0)
{
	MBClientIncTimer0();
	omnicomm_timer++;
}

__MBCL_RECEIVE__ byte UartRecivePacket(void)
{
	static byte stage = 0;
		
	switch(stage)
	{
		case 0:
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//  Этот код закоментарен специально для одновременной работы Omnicomm и Modbus
// при этом MODBUS паузы выдерживаются не правильно !!!! Конкретно для этого проекта - одно
// устройство на линии это приемлемо. При нескольких работа MODBUS скорее всего будет не корректной

		/*	BufferSize0 = 0;			//Очистка буфера
			state0 = State_INITIAL;	//!!!!!!!!!!!!!!!!!!!!!
			stage++;		
			break;
		case 1:
			if(timer0 > T15_Ticks)
			{
				stage++;
				CLR_DE485_0();
			}
			break;
		case 2:
			*/
			if(state0 == State_INITIAL)
			{
				if(timer0 > T35_Ticks)
				{
					stage++;
					BufferSize0 = 0;
					state0 = State_IDLE; //Установить состояние инициализации
				}
			}
			else
			{
				state0 = State_IDLE; //Установить состояние инициализации
				BufferSize0 = 0;
				stage++;
			}
			break;
		case 1:
			if(state0 == State_RECEIVING || state0 == State_WAITING)
			{
				stage++;
			}
			break;
		case 2:
			if(timer0 > T15_Ticks)
			{
				stage++;
				state0=State_WAITING;
			}
		case 3:
			if(timer0 > T35_Ticks)
			{
				stage = 0;
				return BufferSize0;
			}	
			break;

	}
	return 0;
}

//обработка прерывания приёма данных в UART
SIGNAL(USART_RX_vect)
{

	byte temp = *pUDR0;
	//Если находимся между 1.5 и 3.5 временными интервалами
	if (state0 == State_WAITING)
	{
		//очистить буфер и выйти
		BufferSize0 = 0;
		return;
	}
	//Если не в состоянии инициализации
	if (state0 != State_INITIAL)
	{
		if (BufferSize0 == 0) state0=State_RECEIVING;//Перейти в состояние приёма
		DataBuffer0[ BufferSize0++ ] = temp; //считать полученные данные в буфер
		if(BufferSize0 >= USART_PACKET_MAX_LENGHT)//если размер буфера превысил максимальное значение
			state0 = State_WAITING;//перейти в состояние ожидания
	}
	StartModbusTimer0();//запуск таймера
}

SIGNAL(USART_TX_vect)
{
	cbi(UCSR0B, TXCIE0);
	CLR_DE485_0();
	state0 = State_IDLE; //Установить состояние инициализации	
	StartModbusTimer0();
}

SIGNAL(USART_UDRE_vect)
{

	*pUDR0 = *pBuf0; /* start transmition */
	pBuf0++;
	BufferSize0--;
	if(!BufferSize0)
	{
		cbi(UCR0, UDRIE0); /* disable UDRE interrupt */
		sbi(UCSR0B,TXCIE0);
	}
}
