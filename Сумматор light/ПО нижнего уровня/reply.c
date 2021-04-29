#include "mbfanction0.h"
#include "reply.h"
#include <string.h>
#include <stdlib.h>
#include "dot.h"
#include "freq_dot.h"

unsigned char EEMEM e2_net_mode, e2_k_t0, e2_t, e2_pwm_mode, e2_filt_length, e2_period_auto, e2_passw, e2_report_on, e2_password[8];
unsigned int EEMEM e2_n, e2_f_curr, e2_k_t, e2_f_min_t, e2_f_max_t, e2_pwm_max, e2_trl;
unsigned long EEMEM e2_id, e2_password_hi, e2_password_lo;

unsigned char EEMEM id_was_set, passw, filt_length_accessed;

unsigned char EEMEM old_contin;

unsigned char period_auto = 1;

unsigned char net_mode, k_t0, t, pwm_mode, filt_length, period_auto, passw, report_on;
unsigned int n, f_curr, k_t, f_min_t, f_max_t, pwm_max, trl;
unsigned long id, password_hi, password_lo;

void update_data(void);


void update_data(void)
{
	f_curr = (unsigned short)(1000.0 * VolumePercent);
	n = 1023.0 * VolumePercent;
}

void Read_parameters (void)
{
	unsigned char a,b;

	net_mode = eeprom_read_byte(&e2_net_mode);
	k_t0 = eeprom_read_byte(&e2_k_t0);
	pwm_mode = eeprom_read_byte(&e2_pwm_mode);
	filt_length = eeprom_read_byte(&e2_filt_length);
	period_auto = eeprom_read_byte(&e2_period_auto);
	passw = eeprom_read_byte(&e2_passw);

	a=eeprom_read_byte(&e2_report_on);
	b=eeprom_read_byte(&old_contin);

	if(a)
	{
		report_on = 1;
		omnicomm_command_type = NEW_COMMAND_TYPE;
	}
	else if(b)
	{
		report_on =	1;
		omnicomm_command_type = OLD_COMMAND_TYPE;
	}
	else
		report_on = 0;

	passw = eeprom_read_byte(&e2_passw);

	k_t = eeprom_read_word(&e2_k_t);
	f_min_t = eeprom_read_word(&e2_f_min_t);
	f_max_t = eeprom_read_word(&e2_f_max_t);
	pwm_max = eeprom_read_word(&e2_pwm_max);
	trl = eeprom_read_word(&e2_trl);

	eeprom_read_block(&id, &e2_id, 4);

	if(period_auto == 0)
		period_auto = 1;

	/*if (net_mode == INDEPENDENT_MODE)
		my_addr = 0xFF;
		else
			my_addr = net_addr;*/
}

void Set_id (void)
{
	if (eeprom_read_byte(&id_was_set))//id уже записали?
		return;
		else
		{
			id = (unsigned long)DataBuffer0[6];
			id <<= 8;
			id |= (unsigned long)DataBuffer0[5];
			id <<= 8;
			id |= (unsigned long)DataBuffer0[4];
			id <<= 8;
			id |= (unsigned long)DataBuffer0[3];
			eeprom_busy_wait();
			eeprom_write_block(&id, &e2_id, 4);
			eeprom_busy_wait();
			eeprom_write_byte(&id_was_set, 1);//признак того, что id уже записан
			OmniComm_Reply_ok();
		}
}

void Id_read (void)
{
	DataBuffer0[6] = (unsigned char)(id >> 24);
	DataBuffer0[5] = (unsigned char)(id >> 16);
	DataBuffer0[4] = (unsigned char)(id >> 8);
	DataBuffer0[3] = (unsigned char)id;
	omnicomm_frame_size = 8;
	OmniComm_Send_frame();
}

void Set_addr (void)
{
	unsigned char net_addr;
	net_addr = DataBuffer0[3];
	eeprom_busy_wait();
	eeprom_write_byte(&EEP_ID0, net_addr);
	MBClientSetHostID0(net_addr);
	OmniComm_Reply_ok();
}

void Set_net_mode (void)
{
	net_mode = DataBuffer0[3];
	eeprom_busy_wait();
	eeprom_write_byte(&e2_net_mode, net_mode);
	OmniComm_Reply_ok();
}

void Config_read (void)
{
	DataBuffer0[6] = (unsigned char)(id >> 24);
	DataBuffer0[5] = (unsigned char)(id >> 16);
	DataBuffer0[4] = (unsigned char)(id >> 8);
	DataBuffer0[3] = (unsigned char)id;
	DataBuffer0[8] = (unsigned char)(f_max_t >> 8);
	DataBuffer0[7] = (unsigned char)f_max_t;
	DataBuffer0[10] = (unsigned char)(f_min_t >> 8);
	DataBuffer0[9] = (unsigned char)f_min_t;
	DataBuffer0[12] = (unsigned char)(k_t >> 8);
	DataBuffer0[11] = (unsigned char)k_t;
	DataBuffer0[13] = k_t0;
	DataBuffer0[15] = (unsigned char)(pwm_max >> 8);
	DataBuffer0[14] = (unsigned char)pwm_max;
	DataBuffer0[17] = (unsigned char)(trl >> 8);
	DataBuffer0[16] = (unsigned char)trl;
	DataBuffer0[18] = MBClientGetHostID0();
	DataBuffer0[19] = net_mode;
	DataBuffer0[20] = pwm_mode;
	omnicomm_frame_size = 22;
	OmniComm_Send_frame();
}

void Data_read (unsigned int _type)
{
	char str[6];
	
	int i,length;
	if(_type == NEW_COMMAND_TYPE)
	{
		update_data();
		DataBuffer0[3] = t;
		DataBuffer0[4] = (unsigned char)n;
		DataBuffer0[5] = (unsigned char)(n >> 8);
		DataBuffer0[6] = (unsigned char)f_curr;
		DataBuffer0[7] = (unsigned char)(f_curr >> 8);
		omnicomm_frame_size = 9;
		OmniComm_Send_frame();
	}
	else
	{
		
		//omnicomm_frame_size=0;
		update_data();
		strcpy((char *)DataBuffer0, "");
		
		//frequency
		strcat((char *)DataBuffer0, "F=");
		utoa(f_curr, str, 16);

		length=strlen(str);
		if(length<4)
		{
			for(i=0; i<(4-length);i++)
			{
				strcat((char *)DataBuffer0,"0");
			}
		}
		else
		{
			if(length>4)
				strcpy(str,"0000");
		}
		strcat((char *)DataBuffer0, str);
		
		//temperature
		strcat((char *)DataBuffer0, " t=");
		utoa((unsigned int) t, str, 16);

		length=strlen(str);
		if(length<2)
			for(i=0; i<(2-length);i++)
				strcat((char *)DataBuffer0,"0");
		else if(length>2)		
			strcpy(str,"00");

		strcat((char *)DataBuffer0, str);
		
		//level
		strcat((char *)DataBuffer0, " N=");
		utoa(n, str, 16);

		length=strlen(str);
		if(length<4)
			for(i=0; i<(4-length);i++)
				strcat((char *)DataBuffer0,"0");
		else if(length>4)		
			strcpy(str,"0000");

		strcat((char *)DataBuffer0, str);/**/
		strcat((char *)DataBuffer0, ".0\n\r");

		omnicomm_frame_size=strlen((char *)DataBuffer0);
		
		OmniComm_Send_frame_old_data();
	}
}

void Data_contin (unsigned int _type)
{
	if(_type == NEW_COMMAND_TYPE)
	{
		OmniComm_Reply_ok();
		
		omnicomm_command_type = NEW_COMMAND_TYPE;
		//Disable_auto_old_mode();
		report_on = 1;
		if(period_auto == 0)
			period_auto = 1;		
	}
	else
	{		
		if(eeprom_read_byte(&e2_report_on))
		{
			eeprom_busy_wait();
			eeprom_write_byte(&e2_report_on, 0);
		}	

		Enable_auto_old_mode();

		omnicomm_command_type = OLD_COMMAND_TYPE;
		report_on = 1;
		if(period_auto == 0)
			period_auto = 1;		
	}
}

void Fcurr_fmin (void)
{
	update_data();
	eeprom_busy_wait();
	eeprom_write_word(&e2_f_min_t, f_curr);
	f_min_t = eeprom_read_word(&e2_f_min_t);
	OmniComm_Reply_ok();
}

void Fcurr_fmax (void)
{
	update_data();
/*	eeprom_busy_wait();
	eeprom_write_word(&e2_f_max_t, f_curr);
	f_max_t = eeprom_read_word(&e2_f_max_t);*/
	OmniComm_Reply_ok();
}

void Tslope_t0 (void)
{
/*	k_t = (unsigned int)DataBuffer0[5];
	k_t <<= 8;
	k_t |= (unsigned int)DataBuffer0[4];
	k_t0 = DataBuffer0[3];
	eeprom_busy_wait();
	eeprom_write_word(&e2_k_t, k_t);
	eeprom_busy_wait();
	eeprom_write_byte(&e2_k_t0, k_t0);*/
	OmniComm_Reply_ok();
}

void Set_fmin (void)
{
	/*f_min_t = (unsigned int)DataBuffer0[4];
	f_min_t <<= 8;
	f_min_t |= (unsigned int)DataBuffer0[3];
	eeprom_busy_wait();
	eeprom_write_word(&e2_f_min_t, f_min_t);
	f_min_t = eeprom_read_word(&e2_f_min_t);*/
	OmniComm_Reply_ok();
}

void Set_fmax (void)
{
	/*f_max_t = (unsigned int)DataBuffer0[4];
	f_max_t <<= 8;
	f_max_t |= (unsigned int)DataBuffer0[3];
	eeprom_busy_wait();
	eeprom_write_word(&e2_f_max_t, f_max_t);
	f_max_t = eeprom_read_word(&e2_f_max_t);*/
	OmniComm_Reply_ok();
}

void Set_pwm_max (void)
{
	/*pwm_max = (unsigned int)DataBuffer0[4];
	pwm_max <<= 8;
	pwm_max |= (unsigned int)DataBuffer0[3];
	eeprom_busy_wait();
	eeprom_write_word(&e2_pwm_max, pwm_max);*/
	OmniComm_Reply_ok();
}

void Set_pwm_mod (void)
{
	/*pwm_mode = DataBuffer0[3];
	eeprom_busy_wait();
	eeprom_write_byte(&e2_pwm_mode, pwm_mode);*/
	OmniComm_Reply_ok();
}

void Set_trl (void)
{
	/*trl = (unsigned int)DataBuffer0[4];
	trl <<= 8;
	trl |= (unsigned int)DataBuffer0[3];
	eeprom_busy_wait();
	eeprom_write_word(&e2_trl, trl);*/
	OmniComm_Reply_ok();
}

void Set_filt_length (void)
{
	//if (eeprom_read_byte(&filt_length_accessed))
		{
	/*		filt_length = DataBuffer0[3];
			eeprom_busy_wait();
			eeprom_write_byte(&e2_filt_length, filt_length);
			eeprom_busy_wait();
			eeprom_write_byte(&filt_length_accessed, 0);*/
			OmniComm_Reply_ok();
		}
}

void Set_passw (void)
{
	/*unsigned char temp;
	unsigned long ph,pl;
	
	if((!eeprom_read_byte(&passw)) | (eeprom_read_byte(&filt_length_accessed)))
		return;
		else
		{
			password_hi = (unsigned long)DataBuffer0[6];
			password_hi <<= 8;
			password_hi |= (unsigned long)DataBuffer0[5];
			password_hi <<= 8;
			password_hi |= (unsigned long)DataBuffer0[4];
			password_hi <<= 8;
			password_hi |= (unsigned long)DataBuffer0[3];
			password_lo = (unsigned long)DataBuffer0[10];
			password_lo <<= 8;
			password_lo |= (unsigned long)DataBuffer0[9];
			password_lo <<= 8;
			password_lo |= (unsigned long)DataBuffer0[8];
			password_lo <<= 8;
			password_lo |= (unsigned long)DataBuffer0[7];
			eeprom_read_block(&ph, &e2_password_hi, 4);
			eeprom_read_block(&pl, &e2_password_lo, 4);
			if((password_hi == ph) & (password_lo == pl))
				{
					eeprom_write_byte(&passw, 3);
					eeprom_write_byte(&filt_length_accessed, 1);
				}
				else
					{
						temp = eeprom_read_byte(&passw);
						if (temp)
							{
								temp--;
								eeprom_write_byte(&passw, temp);
							}
					}
		}
	if(eeprom_read_byte(&passw) != 3)
		OmniComm_Reply_error();
		else*/
			OmniComm_Reply_ok();
}

void Set_period (void)
{
	period_auto = DataBuffer0[3];
	if(period_auto == 0)
		period_auto = 1;
	eeprom_busy_wait();
	eeprom_write_byte(&e2_period_auto, period_auto);
	OmniComm_Reply_ok();
}

void Filt_leng_read (void)
{
	filt_length = eeprom_read_byte(&e2_filt_length);
	DataBuffer0[3] = filt_length;
	omnicomm_frame_size = 5;
	OmniComm_Send_frame();
}

void Passw_read (void)
{
	DataBuffer0[3] = eeprom_read_byte(&e2_passw);
	omnicomm_frame_size = 5;
	OmniComm_Send_frame();
}

/*
автоматические режимы
	Может быть автоматический режим только для старых или новых команд, одноврменно быть не может.
	При включении автоматического режима для старых команд, автоматический режим для новых команд отключается.
*/

void Set_auto_mode (unsigned char _param)
{
	if(_param==0)
	{
		eeprom_busy_wait();
		eeprom_write_byte(&e2_report_on, 0);
		Disable_auto_old_mode();
	}
	else if(_param==1)
	{
		Disable_auto_old_mode();
		report_on=1;
		eeprom_busy_wait();
		eeprom_write_byte(&e2_report_on, report_on);		
	}
	else if(_param==2)
	{
		Enable_auto_old_mode();
		eeprom_busy_wait();
		eeprom_write_byte(&e2_report_on, 0);
	}
	OmniComm_Reply_ok();
}

void Enable_auto_old_mode (void)
{
	if(!eeprom_read_byte(&old_contin))
	{
		eeprom_busy_wait();
		eeprom_write_byte(&old_contin, 0x01);
	}	
}

void Disable_auto_old_mode (void)
{
	if(eeprom_read_byte(&old_contin))
	{
		eeprom_busy_wait();
		eeprom_write_byte(&old_contin, 0x00);
	}	
}

unsigned char Get_auto_old_mode (void)
{
	return(eeprom_read_byte(&old_contin));
}

