#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include "uart.h"

void UART_init(uint32_t baud) {
    uint16_t baudrate = ((F_CPU + baud * 8L) / (baud * 16L) - 1);

    UBRR0H = (baudrate >> 8);
    UBRR0L = baudrate;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | /* Enable RX/TX lines */
            (1 << RXCIE0); /* Enable interrupt on RX complete*/

    /* 8 data bits, 2 stop bits, no parity */
    //UCSR0C = (1 << USBS0) | (1 << UCSZ00) | (1 << UCSZ01);
}

void UART_disable() {
    UBRR0 = 0;
    UCSR0B = 0;
}

void UART_transmit(uint8_t data) {
    while (!(UCSR0A & (1 << UDRE0))) {
    }
    UDR0 = data;
}
