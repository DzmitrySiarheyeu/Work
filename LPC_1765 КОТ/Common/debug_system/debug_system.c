#include "Config.h"

#include <stdarg.h>
#include <stdio.h>

#ifdef  DEBUG
    	 
static xSemaphoreHandle xSemaphoreDebug = NULL;
extern FILE std_dbg_out;

void debug_puts_RTOS(const char *str)
{
  if(xSemaphoreDebug == NULL)
    vSemaphoreCreateBinary( xSemaphoreDebug );
  if(xSemaphoreDebug == NULL)
    return;
  if( xSemaphoreTake( xSemaphoreDebug, ( portTickType ) 1000 ) == pdTRUE )
  {
    //puts(str);
	fprintf(&std_dbg_out, "%s", str);
    xSemaphoreGive( xSemaphoreDebug );
  }
}

void debug_printf_RTOS(const char *format,...)
{
  va_list args;
  if(xSemaphoreDebug == NULL)
    vSemaphoreCreateBinary( xSemaphoreDebug );
  if(xSemaphoreDebug == NULL)
    return;
  if( xSemaphoreTake( xSemaphoreDebug, ( portTickType ) 1000 ) == pdTRUE )
  {
    va_start(args, format);
    //vprintf(format, args);
	vfprintf(&std_dbg_out, format, args);
    va_end(args);
    xSemaphoreGive( xSemaphoreDebug );
  }
}

void debug_vprintf_RTOS(const char *format, va_list args)
{
  if(xSemaphoreDebug == NULL)
  vSemaphoreCreateBinary( xSemaphoreDebug );
  if(xSemaphoreDebug == NULL)
    return;
  if( xSemaphoreTake( xSemaphoreDebug, ( portTickType ) 1000 ) == pdTRUE )
  {
    //vprintf(format, args);
	vfprintf(&std_dbg_out, format, args);
    xSemaphoreGive( xSemaphoreDebug );
  }
}

#else

void debug_puts_RTOS(const char *str){}

void debug_printf_RTOS(const char *format,...){}

void debug_vprintf_RTOS(const char *format, __va_list args){}
	   	
#endif


