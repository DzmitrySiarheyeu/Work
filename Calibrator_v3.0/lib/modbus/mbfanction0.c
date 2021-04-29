#include "mbfanction0.h"
#include "../LCD_133X65_UC1601S.h"
#include "../uart.h"
#include "../config.h"
#include <util/crc16.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include <string.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <stdint.h>

///////////////////////// ДЛЯ ПЕРВОГО ПОРТА //////////////////
MODBUS_INT CPT0[9];

#if (F_CPU != 14745600)
#warning Timer2 prescaler has to be recalculated according to CPU clock!\
         T35_Ticks and T15_Ticks might need adjustment.
#endif
static inline void StartModbusTimer0();

uint8_t use_rs485;
uint8_t DataBuffer0[ MBCLIENT_MAX_LENGHT ];

volatile uint8_t *pUDR0;
static volatile uint8_t * volatile pBuf0;
static volatile uint8_t BufferSize0 = 0;
static volatile uint8_t state0 = State_IDLE;

static void rx_disable() {
    UCSR0B &= ~(1 << RXEN0);
    if (use_rs485) rs485_tx_enable();
}

static void rx_enable() {
    UCSR0B |= 1 << RXEN0;
    if (use_rs485) rs485_rx_enable();
}

static uint16_t crc16(uint8_t *buf, uint8_t len, uint16_t prev_crc) {
    uint16_t crc = prev_crc;
    while (len--)
        crc = _crc16_update(crc, *(buf++));
    return crc;
}

uint8_t CheckCRC0() {
    //Проверяем, совпадает ли пришедшее CRC с вычисленным
    if (crc16(DataBuffer0, BufferSize0 - 2, 0xFFFF) != *(unsigned int *) (DataBuffer0 + BufferSize0 - 2))
        return CRC_ERROR; //Если нет - вернуть ошибку
    return 0;
}

/**
 * 
 * @param size
 * @param id client ID
 */
void MBSerialSend(uint8_t size, uint8_t id) {
    unsigned int crc;

    if (size > 252) {
        CPT0[SLAVE_EXSEPT_ERR_CNTR].i++; //нарастить соотв. каунтер
        DataBuffer0[1] |= MOD_ERROR_CODE_FLAG; //К номеру ф-ции логически прибавить значение ошибки (0x80)
        DataBuffer0[2] = ILLEGAL_DATA_VALUE; //записать код исключения
        size = 2;
    }
    rx_disable();

    //DataBuffer0[0] = MBClientGetHostID0(); //первый эл-т буфера - номер хоста
    DataBuffer0[0] = id;

    //дописываем CRC
    crc = crc16(DataBuffer0, size + 1, 0xFFFF);
    DataBuffer0[size + 1] = (unsigned char) (crc & 0xFF);
    DataBuffer0[size + 2] = (unsigned char) (crc >> 8);
    //Настраиваем указатель(используется в обработчике прерывания UART) на буфер 
    pBuf0 = DataBuffer0;
    BufferSize0 = size + 3; //размер посылаемого пакета размер данных+1(адрес)+2(CRC)
    UCR0 |= (1 << UDRIE0); //разрешение прерывания     
    while (!(UCSR0B & (1 << RXEN0))); //пока не очистится буфер.
}

uint8_t UartReceivePacket(void) {
    if (state0 == State_RECEIVING && TCNT2 >= T35_Ticks) {
        state0 = State_WAITING;
        return BufferSize0;
    }
    if (state0 == State_WAITING) {
        state0 = State_IDLE;
    }
    return 0;
}

/**
 * проверка на точ то принятый пакет - целый (не битый) пакет модбас
 * @param clientID
 * @return 
 */
uint8_t check_modbus_packet(unsigned char clientID) {
    //контроль размера полученных данных
    if (!((BufferSize0 < 4) || (BufferSize0 > MBCLIENT_MAX_LENGHT))) {
        //проверяем CRC
        if (!CheckCRC0()) {
            CPT0[BUS_MSG_CNTR].i++;
            //проверяем, нам ли предназначен пакет
            /* Ignore if we haven't resolved id yet */
            if (clientID == 0 || (DataBuffer0[0] == clientID)) {
                //если всё правильно возвращаем длину	        
                CPT0[SLAVE_MSG_CNTR].i++;
                return (BufferSize0 - 3);
            }
        } else {
            CPT0[BUS_COM_ERR_CNTR].i++;
        }
    } else {
        CPT0[BUS_COM_ERR_CNTR].i++;
    }

    return 0;
}

/**
 * обработка прерывания приёма данных в UART
 */
ISR(USART_RX_vect) {
    uint8_t temp = *pUDR0;
    // если мы обрабатываем ранее принятый пакет то ничего не делаем
    // кроме сброса таймера
    if (state0 == State_WAITING) {
        StartModbusTimer0();
        return;
    }
    // Если состояние State_IDLE то возможно это начало нового пакета
    if (state0 == State_IDLE) {
        // если не выдержана заданная протоколом пауза значит, значит данную последовательность мы не рассматриваем
        if (TCNT2 >= T35_Ticks) {
            // выдержана пауза, збрасываем счетчик принятых байт, будем рассматривать это как начало нового пакета            
            BufferSize0 = 0;
        } else {
            StartModbusTimer0(); //запуск таймера
            return;
        }
    } else {
        if (TCNT2 >= T15_Ticks) {
            state0 = State_IDLE;
            StartModbusTimer0();
            return;
        }
    }
    StartModbusTimer0(); //запуск таймера
    if (BufferSize0 == 0) {
        // первый байт пакета должен иметь определенное значение, если он его не имеет
        // то рассматривать данный пакет дальше не имеет смысла
        //        if (/*temp == MBClientGetHostID0() ||*/ temp == 0 || temp == 0xFF || temp == TEMP_HOST_ID
        //                /*|| temp == OMNICOMM_MASTER_PREFIX || temp == OMNICOMM_OLD_PREFIX*/) {
        if (temp != 0) {
            state0 = State_RECEIVING; //Перейти в состояние приёма
        } else {
            return;
        }
    }
    DataBuffer0[ BufferSize0++ ] = temp; //считать полученные данные в буфер
    if (BufferSize0 >= MBCLIENT_MAX_LENGHT)//если размер буфера превысил максимальное значение
        state0 = State_IDLE; //перейти в состояние ожидания
}

ISR(USART_TX_vect) {
    UCSR0B &= ~(1 << TXCIE0);
    state0 = State_IDLE; //Установить состояние инициализации
    rx_enable();
}

ISR(USART_UDRE_vect) { /* written to FIFO */
    *pUDR0 = *pBuf0; /* start transmission */
    pBuf0++;
    BufferSize0--;
    if (!BufferSize0) {
        UCSR0A |= 1 << TXC0;
        UCR0 &= ~(1 << UDRIE0); /* disable UDRE interrupt */
        UCSR0B |= 1 << TXCIE0;
    }
}

ISR(TIMER2_OVF_vect) {
    /* Set TCNT2 to MAX value */
    TCNT2 = 255;
    /* Disable timer */
    TCCR2B = 0x00;
}

static inline void StartModbusTimer0() {
    TCCR2A = 0x00;
    /* Prescaler = 128 */
    TCCR2B = (1 << CS20) | (1 << CS22);
    //Запуск таймера
    TCNT2 = 0;
}