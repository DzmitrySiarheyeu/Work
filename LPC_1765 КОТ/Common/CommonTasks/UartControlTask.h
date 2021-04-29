#ifndef USER_UART_H
#define USER_UART_H

#include "config.h"

#if(_UART_CONTROL_TASK_)

	void UartControlTask( void *pvParameters );
	void InitUartControl(void);

#endif // _USER_UART_

#endif
