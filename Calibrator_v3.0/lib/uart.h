#ifndef UART_H
#define UART_H

#include <stdio.h>

void UART_init(uint32_t baudrate);
void UART_disable();
void UART_transmit(uint8_t data);

#endif /* UART_H */
