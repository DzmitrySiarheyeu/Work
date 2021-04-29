#ifndef DEBUG_SYSTEM_H
#define DEBUG_SYSTEM_H

#include <stdarg.h>

#if (_DEBUG_OUTPUT_ == OPTION_ON) 
	   
void debug_puts_RTOS(const char *str);
void debug_printf_RTOS(const char *format,...);
void debug_vprintf_RTOS(const char *, va_list);

#define DEBUG_PUTS(x)   debug_puts_RTOS(x)
#define DEBUG_PRINTF_ONE(x,y)   debug_printf_RTOS(x,y)
#define DEBUG_PRINTF(x){ debug_printf_RTOS x; }

#else

#define DEBUG_PUTS(x)
#define DEBUG_PRINTF(x)
#define DEBUG_PRINTF_ONE(x,y)
		 
#endif //DEBUG

#define DEBUG_OUT_USB OPTION_OFF
#if(DEBUG_OUT_USB == OPTION_ON)

#define USB_DEBUG_PRINTF_ONE(x,y) DEBUG_PRINTF_ONE(x,y)

#else

#define USB_DEBUG_PRINTF_ONE(x,y)

#endif

#endif
