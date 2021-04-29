#include "omnicomm.h"
#include "mbfanction0.h"
#include "uart.h"
#include "settings.h"
//#include "crc.h"

//omnicomm variables
unsigned char	omnicomm_addr, omnicomm_cmd;

volatile unsigned char omnicomm_status, omnicomm_frame_size, omnicomm_command_type;

volatile unsigned long omnicomm_timer;

unsigned char is_frame_ok(int omnicomm_frame_size)
{
	//if(omnicomm_status != OMNICOMM_FRAME_OK) // Здесь в отличие от датчика другой механиз низкоуровнего приема
	//	return 0;
	if(omnicomm_frame_size < 3) // Здесь в отличие от датчика другой механиз низкоуровнего приема
		return 0;
    if(OmniComm_Check_old_frame(omnicomm_frame_size)==OMNICOMM_FRAME_OK)
		return 1;
	if(OmniComm_Check_frame(omnicomm_frame_size) != OMNICOMM_FRAME_OK)
	{
		omnicomm_status = OMNICOMM_IDLE;
		return 0;
	}
	if(omn_param.e2_net_mode == INDEPENDENT_MODE && omnicomm_addr == 0xFF)
		return 1;
	if(omnicomm_addr == MBClientGetHostID0() || (omnicomm_addr != 0 && omnicomm_addr <= 6))
		return 1;
	omnicomm_status = OMNICOMM_IDLE;
	return 0;
}

void do_omnicom(int omnicomm_frame_size)
{
	if(omn_param.e2_report_on)
	{
		if(omnicomm_timer > (1000 * omn_param.e2_period_auto))
		{
			omnicomm_addr = MBClientGetHostID0();
			if(omnicomm_command_type == NEW_COMMAND_TYPE)
			{
				omnicomm_cmd = DATA_CONTIN;
				Data_read(NEW_COMMAND_TYPE);
			}
			else
				Data_read(OLD_COMMAND_TYPE);
			omnicomm_timer = 0;
		}
	}
	if (is_frame_ok(omnicomm_frame_size))
	{
		omn_param.e2_report_on = 0;
		switch (omnicomm_cmd)
		{
			case SET_ID:		{Set_id(); break;}
			case ID_READ:		{Id_read(); break;}
			case SET_ADDR:		{Set_addr(); break;}
			case SET_NET_MODE:	{Set_net_mode(); break;}
			case CONFIG_READ:	{Config_read(); break;}
			case DATA_READ:		{
									Data_read(NEW_COMMAND_TYPE); 
									break;
								}
			case DATA_READ_OLD:	{
									Disable_auto_old_mode();		
									Data_read(OLD_COMMAND_TYPE);
									break;
								}
			case DATA_CONTIN:	{
									Data_contin(NEW_COMMAND_TYPE);
									break;
								}//не описывал, т.к. мало информации по режиму. Должна поддерживаться старая команда 0x44 0xFF - нет информации, как применять в контексте протокола
			case DATA_CONTIN_OLD:{
									Data_contin(OLD_COMMAND_TYPE);
								 	break;
								}
			case FCURR_FMIN:	{Fcurr_fmin(); break;}
			case FCURR_FMAX:	{Fcurr_fmax(); break;}
			case TSLOPE_T0:		{Tslope_t0(); break;}
			case SET_FMIN:		{Set_fmin(); break;}
			case SET_FMAX:		{Set_fmax(); break;}
			case SET_PWM_MAX:	{Set_pwm_max(); break;}
			case SET_PWM_MOD:	{Set_pwm_mod(); break;}
			case SET_TRL:		{Set_filt_length(); /*Set_trl();*/ break;}
			case SET_FILT_LENG:	{Set_filt_length(); break;}
			case SET_PASSW:		{Set_passw(); break;}
			case SET_PERIOD:	{Set_period(); break;}
			case FILT_LENG_READ:{Filt_leng_read(); break;}
			case PASSW_READ:	{Passw_read(); break;}
			case SET_AUTO_MODE:	{Set_auto_mode(UartBuff[3]); break;}//команда связана с командой DATA_CONTIN, устанавливает режим вывода данных после старта контроллера, т.е. контроллер сразу после запуска должен "тупо" кидать данные по RS485
			default: OmniComm_Reply_ok();
		}
		//UARTResetRxBuf();
	}
}


unsigned char OmniComm_Do_crc (unsigned char frame_size)
{
	unsigned char i;
	unsigned char omnicomm_crc = 0;

	for(i = 0; i < frame_size - 1; i++)
	{
		omnicomm_crc = crc8(UartBuff[i], omnicomm_crc);
	}
	return(omnicomm_crc);
}


void OmniComm_Send_frame (void)	//перед вызовом этой функции в omnicomm_frame_size нужно записать размер пакета (количество байт в пакете,
{								//учитывая префикс и crc)
	
	omnicomm_status = OMNICOMM_BUSY;//"заняли" протокол
//	SET_DE485_0();//включили режим передачи
	UartBuff[0] = OMNICOMM_SLAVE_PREFIX;
	UartBuff[1] = omnicomm_addr;
	UartBuff[2] = omnicomm_cmd;
	UartBuff[omnicomm_frame_size - 1] = OmniComm_Do_crc(omnicomm_frame_size);
	UARTSend(omnicomm_frame_size);
}

void OmniComm_Send_frame_old_data (void)	//перед вызовом этой функции в omnicomm_frame_size нужно записать размер пакета (количество байт в пакете,
{								//учитывая префикс и crc)
	
	omnicomm_status = OMNICOMM_BUSY;//"заняли" протокол
	//SET_DE485_0();//включили режим передачи

	UARTSend(omnicomm_frame_size);
}

void OmniComm_Reply_error (void)//функция ответа мастеру об ошибке
{
	UartBuff[3] = OMNICOMM_ERROR;
	omnicomm_frame_size = 5;
	OmniComm_Send_frame();
}


void OmniComm_Reply_ok (void)//функция ответа мастеру об отсутствии ошибок
{
	UartBuff[3] = OMNICOMM_NO_ERRORS;
	omnicomm_frame_size = 5;
	OmniComm_Send_frame();
}

unsigned char OmniComm_Check_frame (int omnicomm_frame_size)	//функция проверяет пакет и возвращает
{											//признак наличия или отсутствия ошибки
	if (UartBuff[ 0 ] == OMNICOMM_MASTER_PREFIX && omnicomm_frame_size > 3)	//если пришёл префикс мастера
	{
		if (OmniComm_Do_crc(omnicomm_frame_size) == UartBuff[omnicomm_frame_size - 1])
		{
			omnicomm_addr = UartBuff[ 1 ];
			omnicomm_cmd = UartBuff[ 2 ];
			return(OMNICOMM_FRAME_OK);
		}
		else
		{
			omnicomm_addr = UartBuff[ 1 ];
			omnicomm_cmd = UartBuff[ 2 ];
			return(OMNICOMM_FRAME_ERROR);
		}
		
	}
	return(OMNICOMM_FRAME_ERROR);
}

unsigned char OmniComm_Check_old_frame (int omnicomm_frame_size)	//функция проверяет пакет и возвращает
{				
	if(UartBuff[0]==OMNICOMM_OLD_PREFIX)
	{
		if(UartBuff[1] == 0x4F)
		{
			omnicomm_cmd = DATA_READ_OLD;
	 		omnicomm_addr = 5;//MBClientGetHostID0();
			return(OMNICOMM_FRAME_OK);	
		}
		if(UartBuff[1] == 0x50)
		{
			omnicomm_cmd = DATA_CONTIN_OLD;
			omnicomm_addr = 5;//MBClientGetHostID0();
			return(OMNICOMM_FRAME_OK);
		}	
	}
	return(OMNICOMM_FRAME_ERROR);
}
 
