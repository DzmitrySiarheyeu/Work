 /*
 * Copyright (c) 2003, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: shell.c,v 1.1 2006/06/07 09:43:54 adam Exp $
 *
 */
#include "Config.h"
#if(_TELNET_)

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>


#include "telnet.h"
#include "shell.h"

#include "sms.h"
#include "ff.h"    
#include "VFS.h"
#include "clock_drv.h"
#include "ModemTask.h"  
#include "Security.h"

struct ptentry {
  char *commandstr;
  void (* pfunc)(TL_CONNECTION *conn, char *str, char *data);
};

static void time(TL_CONNECTION *conn, char *str, char *data);
static void date(TL_CONNECTION *conn, char *str, char *data);
static void stat(TL_CONNECTION *conn, char *str, char *data);
static void open(TL_CONNECTION *conn, char *str, char *data);
static void dir(TL_CONNECTION *conn, char *str, char *data);
static void firm(TL_CONNECTION *conn, char *str, char *data);
static void option(TL_CONNECTION *conn, char *str, char *data);
static void cd(TL_CONNECTION *conn, char *str, char *data);
static void cd_dot_dot(TL_CONNECTION *conn, char *str, char *data);
static void unknown(TL_CONNECTION *conn, char *str, char *data);
static void help(TL_CONNECTION *conn, char *str, char *data);
static  int scan(TL_CONNECTION *conn);

struct TagDir curr_dir = {.path = "/", .offset = 1};
// Флаг пройденной аутентификации
static int telnet_auth = 0;
void clr_telnet_auth(void)
{
	telnet_auth = 0;	
}

/*---------------------------------------------------------------------------*/
const static struct ptentry parsetab[] =
  {{"open", open},
   {"dir", dir},
   {"..", cd_dot_dot},
   {"cd", cd},
   {"stat", stat},
   {"time", time},
   {"date", date},
   {"firmware", firm},
   {"option", option},
   {"help", help},
   {"?", help},
//   {"exit", shell_quit},

   /* Default action */
   {NULL, unknown}};
/*---------------------------------------------------------------------------*/

#if(LWIP_STATS && MEM_STATS)

    extern struct stats_ lwip_stats;
    #define USE_LWIP_STATS        (1)

#endif // LWIP_STATS && MEM_STATS

/*---------------------------------------------------------------------------*/
static void
parse(TL_CONNECTION *conn, char *cmd, char *data, struct ptentry *t)
{
  struct ptentry *p;
  for(p = t; p->commandstr != NULL; ++p) {
    if(strncmp(p->commandstr, cmd, strlen(p->commandstr)) == 0) {
      break;
    }
  }

  p->pfunc(conn, cmd, data);
}
/*---------------------------------------------------------------------------*/
static void
help(TL_CONNECTION *conn, char *str, char *data)
{
  if(!strcmp((const char *)data, "date"))
  {
      shell_output(conn, "Input format - date DD.MM.YYYY\n\r", "DD = [00,31]; MM = [00,12]; YYYY = [2010,2100]\n\r");
  }

  else if(!strcmp((const char *)data, "time"))
  {
      shell_output(conn, "Input format - time HH.MM.SS\n\r", "HH = [00,24]; MM = [00,60]; SS = [00,60]\n\r");
  }

  else if(!strcmp((const char *)data, "stat"))
  {
      shell_output(conn, "Supported stats:\n", "\r");

#if(USE_LWIP_STATS)
      shell_output(conn, "lwIP\n", "\r");
#endif

      shell_output(conn, "Flash\n", "\r");
      shell_output(conn, "Common\n", "\r");
  }

  else if(!strcmp((const char *)data, "open"))
  {
      shell_output(conn, "Command format: open <[path/]file_name>\n", "\r");
  }

  else if(!strcmp((const char *)data, "cd"))
  {
      shell_output(conn, "Command format: cd <[path/]folder_name>\n", "\r");
  }

  else if(!strcmp((const char *)data, ".."))
  {
      shell_output(conn, "Command format: ..\n", "\r");
  }

  else if(!strcmp((const char *)data, "dir"))
  {
      shell_output(conn, "Command format: dir\n", "\r");
  }

  else if(!strcmp((const char *)data, "firmwire"))
  {
      shell_output(conn, "Command format: firmwire\n", "\r");
  }

  else if(!strcmp((const char *)data, "option"))
  {
      shell_output(conn, "Command format: ""comand"" ""parameter""\n", "\r");
  }

  /*else if(!strcmp((const char *)data, "exit"))
  {
      shell_output("Command format: exit\n", "");
  }*/

  else if(data == NULL)
  {
      shell_output(conn, "Available commands:\n", "\r");
      shell_output(conn, "open     - open a file\n", "\r");
      shell_output(conn, "dir      - explore current directory\n", "\r");
      shell_output(conn, "cd       - change directory\n", "\r");
      shell_output(conn, "..       - previous directory\n", "\r");
      shell_output(conn, "stat     - show stats\n", "\r");
      shell_output(conn, "time     - set time\n", "\r");
      shell_output(conn, "date     - set date\n", "\r");
      shell_output(conn, "firmware - install a firmware\n", "\r");
	  shell_output(conn, "option   - perform an option\n", "\r");
      shell_output(conn, "help, ?  - show help\n", "\r");
  //    shell_output("exit     - exit shell\n", "");
  }
  else 
      shell_output(conn, "Unknown command: ", data);

  shell_prompt(conn);
}

/*---------------------------------------------------------------------------*/
static void
unknown(TL_CONNECTION *conn, char *str, char *data)
{
    shell_output(conn, "Unknown command: ",str);
    shell_output(conn, "\n","\r");
    shell_prompt(conn);
}
/*---------------------------------------------------------------------------*/
void
shell_start(TL_CONNECTION *conn)
{
  shell_output(conn, "Enter your login ", "and password: ");
}
/*---------------------------------------------------------------------------*/
static void
shell_wellcome(TL_CONNECTION *conn)
{
  shell_output(conn, "lwIP command shell\n\r", "\r");
  shell_output(conn, "Type '?' or 'help' for help\n", "\r");
  shell_prompt(conn);
}
/*---------------------------------------------------------------------------*/
void
shell_input(TL_CONNECTION *conn, char *cmd, u16_t size)
{
  u8_t i = 0, j;

  j = strlen(cmd);

  for(; i < j; i++) 
  {
      if(cmd[i] == ' ') //Ищем пробел, отделяющий команду
      {
          cmd[i] = 0;
          break;
      }
  }

  if( i == j ) 
  	//Нет пробела
	//Ошибка ввода
  	parse(conn, cmd, NULL, (struct ptentry *)parsetab); 
  else 
  {
  	// Аутентификация не пройдена 
  	// Введенные данные расцениваются как логин и пароль
  	if(telnet_auth == 0)
  	{
	  // cmd       - логин
	  // остальное - пароль
	  // Пытаемся пройти аутентификацию
	  int auth;
      auth = NetAccessRequest(cmd, cmd + i + 1);
	 
	  // Аутентификация пройдена?
	  if(auth == 1)
	  {
	  	// Да
	  	telnet_auth	= 1;
		// Выводим приветствие
		shell_wellcome(conn);
	  }	
	  else
	  {
	  	// Нет
	  	shell_output(conn, "Incorrect login or password.\n\r", "");
		// Вводим логин и пароль еще раз
	  	shell_start(conn);
	  }

  	}

	else
	  // Аутентификация была пройдена
	  // Ищем команду
  	  parse(conn, cmd, cmd + i + 1, (struct ptentry *)parsetab);
  }
}
/*---------------------------------------------------------------------------*/
static void time(TL_CONNECTION *conn, char *str, char *data)
{
#if(_CLOCK_ || _CLOCK_RTC_)
	
CLOCKData cur_time;
DEVICE_HANDLE hClock;

	hClock = OpenDevice("Clock"); 

	if(data == 0)
	{
	 	if(hClock == 0) 
		{
			shell_output(conn, "Unable to open clock!\n", "\r");
			shell_prompt(conn);	
			return;
		}
		
		SeekDevice(hClock, SEC_D, 0);
		ReadDevice(hClock, &cur_time.Sec, 3);
		CloseDevice(hClock);

		char time_str[3+1+3+1+3+1+1];
		
		sprintf(time_str, "%02d.%02d.%02d\0", cur_time.Hour, cur_time.Min, cur_time.Sec);
		shell_output(conn, time_str, "\n\r");
		shell_prompt(conn);
		return;	
	}

	u8_t hh, mm, ss;	

	if(isdigit(data[0]) &&
	isdigit(data[1]) &&
	isdigit(data[3]) &&
	isdigit(data[4]) &&
	isdigit(data[6]) &&
	isdigit(data[7]) &&
	strlen(data) == 8)
	{
		hh = strtol(data, NULL, 10);
		mm = strtol(data + 3, NULL, 10);
		ss = strtol(data + 6, NULL, 10);

		if((hh > 24) || (mm > 60) || (ss > 60)) 
		{
			shell_output(conn, "Invalid data value!\n", "\r");
			shell_prompt(conn);
		}
		else
		{
			if(hClock == 0) 
			{
				shell_output(conn, "Unable to open clock!\n", "\r");
			    shell_prompt(conn);	
				return;
			}

			SeekDevice(hClock, SEC_D, 0);
			WriteDevice(hClock, &ss, 1);
			
			SeekDevice(hClock, MIN_D, 0);
			WriteDevice(hClock, &mm, 1);
			
			SeekDevice(hClock, HOUR_D, 0);
			WriteDevice(hClock, &hh, 1);

			CloseDevice(hClock);

			shell_output(conn, "Done!\n", "\r");
			shell_prompt(conn);
			return;
#else //_CLOCK_ 
			shell_output(conn, "There is no clock in current config.\n", "\r");
			shell_prompt(conn);
#endif //_CLOCK_ 
		}
	}
	else
	{
		shell_output(conn, "Invalid data format!\n", "\r");
		shell_prompt(conn);
	}

	CloseDevice(hClock);
}
/*---------------------------------------------------------------------------*/
static void date(TL_CONNECTION *conn, char *str, char *data)
{

#if(_CLOCK_ || _CLOCK_RTC_)
	
CLOCKData cur_time;
DEVICE_HANDLE hClock;

	hClock = OpenDevice("Clock"); 

	if(data == 0)
	{
	 	if(hClock == 0) 
		{
			shell_output(conn, "Unable to open clock!\n", "\r");
			shell_prompt(conn);	
			return;
		}
		
		SeekDevice(hClock, DATE_D, 0);
		ReadDevice(hClock, &cur_time.Date, 4);
		CloseDevice(hClock);

		char time_str[3+1+3+1+4+1+1];
		
		sprintf(time_str, "%02d.%02d.%04d\0", cur_time.Date, cur_time.Month, cur_time.Year + 2000);
		shell_output(conn, time_str, "\n\r");
		shell_prompt(conn);
		return;	
	}

	u16_t yyyy, mm, dd;
	u8_t temp;

    if(isdigit(data[0]) &&
       isdigit(data[1]) &&
       isdigit(data[3]) &&
       isdigit(data[4]) &&
       isdigit(data[6]) &&
       isdigit(data[7]) &&
       isdigit(data[8]) &&
       isdigit(data[9]) &&
       strlen(data) == 10)
    { 
      dd = strtol(data, NULL, 10);
      mm = strtol(data + 3, NULL, 10);
      yyyy = strtol(data + 6, NULL, 10);


      if((dd > 31) || (mm > 12) || (yyyy > 2100) || (yyyy < 2010))
      { 
        shell_output(conn, "Invalid data value!\n", "\r");
        shell_prompt(conn);
      }
      else
      {
		if(hClock == 0) 
		{
			shell_output(conn, "Unable to open clock!\n", "\r");
            shell_prompt(conn);	
			return;
		}

		temp = (u8_t)dd;
        SeekDevice(hClock, DATE_D, 0); 
        WriteDevice(hClock, &temp, 1);

		temp = (u8_t)mm;
        SeekDevice(hClock, MONTH_D, 0);
        WriteDevice(hClock, &temp, 1);

		temp = (u8_t)(yyyy - (u16_t)2000);
        SeekDevice(hClock, YEAR_D, 0); 
        WriteDevice(hClock, &temp, 1);

		CloseDevice(hClock);

        shell_output(conn, "Done!\n", "\r");
        shell_prompt(conn);
        return;
#else
        shell_output(conn, "There is no clock in current config.\n", "\r");
        shell_prompt(conn);
#endif
      }	
    }
    else
    {
      shell_output(conn, "Invalid data format!\n", "\r");
      shell_prompt(conn);
    }
	CloseDevice(hClock);
}
/*---------------------------------------------------------------------------*/
static void stat(TL_CONNECTION *conn, char *str, char *data)
{
#if(USE_LWIP_STATS)

  char s[30];

  if(!strcmp((const char *)data, "lwIP"))
  {
      sprintf(s,"avail: %lu\n\r", lwip_stats.mem.avail);
      shell_output(conn, "Mem:\n\r", s);
      sprintf(s,"used: %lu\n\r", lwip_stats.mem.used);
      shell_output(conn, s, "\r");
      sprintf(s,"max: %lu\n\r", lwip_stats.mem.max);
      shell_output(conn, s, "\r");
      sprintf(s,"err: %lu\n\r", lwip_stats.mem.err);
      shell_output(conn, s, "\r");
      shell_prompt(conn);
  }
  else 

#endif

  if(!strcmp((const char *)data, "Flash"))
  {
      shell_output(conn, "There is nothing here yet:(\n", "\r");
      shell_prompt(conn);
  }
  else if(!strcmp((const char *)data, "Common"))
  {
      shell_output(conn, "Ther is nothing here yet:(\n", "\r");
      shell_prompt(conn);
  }
  else
  {
      unknown(conn, NULL, data);
      shell_prompt(conn);
  }
}
/*---------------------------------------------------------------------------*/
static void open(TL_CONNECTION *conn, char *str, char *data)
{
  char path[TELNET_MAX_LEN];
  FRESULT Res;
  FILE_OBJ fil;

  if(data != NULL)
  {
      strncpy(path , curr_dir.path, curr_dir.offset); //Копируем в path путь к текущему каталогу
      if(path[curr_dir.offset - 1] != '/') 
      {
          path[curr_dir.offset] = '/';
          strncpy(path + curr_dir.offset + 1, data, strlen(data)); //Добавляем имя файла
          path[curr_dir.offset + strlen(data) + 1] = 0;
      }
      else 
      {
          strncpy(path + curr_dir.offset, data, strlen(data)); //Добавляем имя файла 
          path[curr_dir.offset + strlen(data)] = 0;
      }

      Res = (FRESULT)VFS_open(&fil, path, FA_OPEN_EXISTING | FA_READ); 
      if(Res == FR_OK)
      {
          VFS_fseek(&fil, 0);
          VFS_read(&fil, (uint8_t *)path, TELNET_MAX_LEN - 3);//path нам больше не нужен, поэтуму читаем файл в него

          if( fil.l == 0xffff ) //т.е.  fil.l == -1 (какая - то ошибка)
          {
              shell_output(conn, "Error while reading file!\n", "\r");
              shell_prompt(conn);
			  VFS_close(&fil);
              return;
          }

          path[fil.l] = 0;
          shell_output(conn, path, "");
          while(fil.l == TELNET_MAX_LEN - 3)//если мы прочитали TELNETD_CONF_LINELEN - 3 символов,
                                                      //т.е. если длина файла >= TELNETD_CONF_LINELEN - 3, читаем дальше
          { 
              VFS_read(&fil, (uint8_t *)path, TELNET_MAX_LEN - 3);
              path[fil.l] = 0;
              shell_output(conn, path, "\r");
          }
          shell_output(conn, "\n", "\r");
          shell_prompt(conn);
          VFS_close(&fil);
      }
      else 
      {
          shell_output(conn, "File not found!\n", "\r");
          shell_prompt(conn);
      }
  }
  else 
  {
      shell_output(conn, "File name not found!\n", "\r");
      shell_prompt(conn);
  }
}
/*---------------------------------------------------------------------------*/
static void dir(TL_CONNECTION *conn, char *str, char *data)
{
    scan(conn);
}
/*---------------------------------------------------------------------------*/
static void cd(TL_CONNECTION *conn, char *str, char *data)
{   
    if(data != NULL)
    {
        u8_t temp = curr_dir.offset;

        if(curr_dir.path[curr_dir.offset - 1] != '/') 
        {
            curr_dir.path[curr_dir.offset] = '/';
            curr_dir.offset++;
        }

        strncpy(curr_dir.path + curr_dir.offset, data, strlen(data));
        curr_dir.offset += strlen(data);
        curr_dir.path[curr_dir.offset] = 0;

        if( scan(conn) )
        {
            curr_dir.offset = temp;
            curr_dir.path[curr_dir.offset] = 0;
        }
    }
    else 
    {
        shell_output(conn, "Directory name not found!\n", "\r");
        shell_prompt(conn);
    }
}
/*---------------------------------------------------------------------------*/
static void cd_dot_dot(TL_CONNECTION *conn, char *str, char *data)
{ 
    char *c;

    c = strrchr(curr_dir.path, '/'); 
    if( c != strchr(curr_dir.path, '/')) 
    {
        *c = 0;
        curr_dir.offset = c - curr_dir.path;
    }
    else 
    {
        *(c + 1) = 0;
        curr_dir.offset = c + 1 - curr_dir.path;
    }

    scan(conn);
}
/*---------------------------------------------------------------------------*/

extern xQueueHandle telnet_queue;

static void option(TL_CONNECTION *conn, char *str, char *data)
{
	// Максимальный размер = команда (до 10 символов) + пробел + параметры (до 40 символов) 
	#define MAX_OPTION_LEN 100//512//(10 + 1 + 40)

	MODEM_MESSAGE message;
	
	// Длина не должна превышать максимальный размер
	int opt_len = strlen(data);
	if(opt_len > MAX_OPTION_LEN)	
	{
		shell_output(conn, "Invalid option length!\n", "\r");
    	shell_prompt(conn);	
		return;
	}			

	int len = 0;
	// Ищем пробел-разделитель команды и параметров
	while((data[len] != ' ') && (len < opt_len))
		len++;

	uint8_t ID;
	// Разделитель найден?
	if(len < opt_len)
	{
		// Да
		// Ставим на его место нуль-терминатор
		data[len] = 0;
		// Получаем ID команды
		ID = get_sms_ID(data);
		
		// Команда опознана?
		if(ID == 0xff)
		{
			// Нет
			shell_output(conn, "Invalid option name!\n", "\r");
    		shell_prompt(conn);	
			return;	
		}	
		
		int res;
//		int response_flag = 0;

		// Если команда должна возвращать данные, выставляем флаг
	//	if(data[len + 1] == '?' || data[len + 1] == '!' || ID == SMS_USSD || ID == SMS_AT_COM)
		//	response_flag = 1;	

		// Пытаемся выполнить команду с полученными параметрами
		message.ID = SMS_PERFORM;
		message.par_1 = ID;
		message.par_2 = MAX_OPTION_LEN - len - 1;
		message.privat_data = (uint8_t *)&data[len + 1];
		message.queue =  telnet_queue;

		xQueueSendToBack(modem_queue, ( void * )&message, 10);

		res = xQueueReceive(telnet_queue, (void *)&message, 30000);

		//res = 0;
		/*if(strcmp((char *)&message.privat_data, "ERR") == 0)
			res = -1;
		else res = 0;  */
	//	res = sms_perform(ID, (uint8_t *)&data[len + 1], MAX_OPTION_LEN - len - 1);
		
		// Команда выполнена?
		if(res == 0)
		{
			// Нет
			shell_output(conn, "ERROR!\n", "\r");
    		shell_prompt(conn);	
			return;	
		}
		
		// нужно выдать ответ
	//	if(response_flag == 1)
		//{
			data[MAX_OPTION_LEN - 1] = 0;
			shell_output(conn, &data[len + 1], "\n\r");
    		shell_prompt(conn);	
			memset(data, 0, MAX_OPTION_LEN);
	//	}
	//	else
	//	{
			// Команда выполнена
			// Выводим подтверждение
		//	shell_output("Done!\n", "");
	    //	shell_prompt();
	//		memset(data, 0, MAX_OPTION_LEN);	
	//	}
	}
	else
	{
		// Не нашли разделитель
		shell_output(conn, "Invalid option format!\n", "\r");
    	shell_prompt(conn);	
		return;	
	}
}

/*---------------------------------------------------------------------------*/
#include "main.h"
//int PutMesToComQueue(uint8_t dst, uint8_t *privat_data, uint16_t data);

static void firm(TL_CONNECTION *conn, char *str, char *data)
{
/*	if(*data == '?') 	
	{
		int res;

		*data = 0;
		res = sms_perform(SMS_VERS, (uint8_t *)data, 20);

		if(res == 0)
		{
			shell_output(data, "\n\r");			
			shell_prompt();	
		}
		else
		{
			shell_output("Unable to get version!\n", "\r");
			shell_prompt();	
		}
	}
	else if(strcmp((const char *)data, "update") == 0)
	{
		PutMesToComQueue(BOOT_REQUEST, 0, 0, 200);
    	shell_output("Now going to the bootloader...\n", "\r");	
	}  */
	shell_output(conn, "OK", "\n\r");			
	shell_prompt(conn);
}

/*---------------------------------------------------------------------------*/

static int scan(TL_CONNECTION *conn)
{
    FILE_OBJ fil;
    DIR_OBJ dir;
    char buf[25];                      // должен быть как минимум: "максимальный размер имени в FAT" + 1 + 1 + 5

    shell_output(conn, "\t", curr_dir.path);
    shell_output(conn, "\n", "\r");

    if ( !VFS_opendir(&dir, curr_dir.path) ) 
    {
       while ((VFS_readdir(&fil, &dir) == FR_OK) && fil.name[0] != NULL)
        {
            if(fil.attrib & AM_DIR) 
            {
                strncpy(buf, "<DIR>\t", 7);
            }
            else 
            {
                strncpy(buf, "<FIL>\t", 7);
            }

            strncpy(buf + 6, fil.name, strlen(fil.name));
            strncpy(buf + 6 + strlen(fil.name), "\n", 1);
            buf[6 + strlen(fil.name) + 1] = 0;
            shell_output(conn, buf, "\r");
        } 
        shell_prompt(conn);
        VFS_closedir(&dir);
        return 0;
    }
    else 
    {
        shell_output(conn, "Directory not found!\n", "\r");
        shell_prompt(conn);
    }

    return -1;
}

#endif
/*---------------------------------------------------------------------------*/
