#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "mbfanction0.h"
#include "settings.h"
#include "reply.h"
#include "dot.h"
#include "uart.h"

OMNI_PARAM  omn_param;

static int nomnicomm_command_type = 0;

void update_data(void);


void update_data(void)
{
	if(omnicomm_addr == MBClientGetHostID0()){
		omn_param.e2_f_curr = (unsigned short)(1000.0 * VolumePercent_addr_99);
		omn_param.e2_n = 1023.0 * VolumePercent_addr_99;
		omn_param.e2_t = 0;
	}
	else if(omnicomm_addr < 5){
		omn_param.e2_n = Reverse4(RegFile.mb_struct.dot_info.DOT[omnicomm_addr-1].frequency);
		omn_param.e2_f_curr = (unsigned short)(1000.0 * omn_param.e2_n);
		omn_param.e2_t = 0;
	}
	else if(omnicomm_addr == 5 || omnicomm_addr == 255){
		omnicomm_addr = 5;
		float freq1 = Reverse4(RegFile.mb_struct.dot_info.DOT[omnicomm_addr - 5].frequency);
		float freq2 = Reverse4(RegFile.mb_struct.dot_info.DOT[omnicomm_addr - 4].frequency);
		omn_param.e2_n = freq1;
		omn_param.e2_f_curr = (unsigned short)(1000.0 * freq1);
		if(freq2 < 450){
			if((uint16_t)freq2 == 0)
				omn_param.e2_t = 251;
			else if((uint16_t)freq2 == 340)
				omn_param.e2_t = 252;
			else if((uint16_t)freq2 == 400)
				omn_param.e2_t = 253;
			else if((uint16_t)freq2 == 420)
				omn_param.e2_t = 254;
			else
				omn_param.e2_t = 255;	
		}
		else{
			if(freq2 < 500) freq2 = 500;
			if(freq2 > 1500) freq2 = 1500;
			float temp = 0;
			temp  = (250.0) * (freq2 - 500.0);
    		temp /= (1500.0 - 500.0);
    		omn_param.e2_t = (uint16_t)temp;
		}
	}
	else if(omnicomm_addr == 6)
	{
		omn_param.e2_f_curr = (unsigned short)(1000.0 * VolumePercent + 500.5);
		omn_param.e2_n = omn_param.e2_f_curr;
		omn_param.e2_t = 0;
	}
	
}

void Read_parameters (void)
{
	if(omn_param.e2_report_on)
	{
		omn_param.e2_report_on = 1;
		omnicomm_command_type = NEW_COMMAND_TYPE;
	}
	else if(omn_param.e2_old_contin)
	{
		omn_param.e2_report_on = 1;
		omnicomm_command_type = OLD_COMMAND_TYPE;
	}
	else
		omn_param.e2_report_on = 0;


	if(omn_param.e2_period_auto == 0)
		omn_param.e2_period_auto = 1;

}

void Set_id (void)
{
//	if (eeprom_read_byte(&id_was_set))//id уже записали?
//		return;
//		else
//		{
//			id = (unsigned long)UartBuff[6];
//			id <<= 8;
//			id |= (unsigned long)UartBuff[5];
//			id <<= 8;
//			id |= (unsigned long)UartBuff[4];
//			id <<= 8;
//			id |= (unsigned long)UartBuff[3];
//			eeprom_busy_wait();
//			eeprom_write_block(&id, &e2_id, 4);
//			eeprom_busy_wait();
//			eeprom_write_byte(&id_was_set, 1);//признак того, что id уже записан
//			OmniComm_Reply_ok();
//		}
}

void Id_read (void)
{
	UartBuff[6] = (unsigned char)(omn_param.e2_id >> 24);
	UartBuff[5] = (unsigned char)(omn_param.e2_id >> 16);
	UartBuff[4] = (unsigned char)(omn_param.e2_id >> 8);
	UartBuff[3] = (unsigned char)omn_param.e2_id;
	omnicomm_frame_size = 8;
	OmniComm_Send_frame();
}

void Set_addr (void)
{
	if(omnicomm_addr != MBClientGetHostID0()) return;
	unsigned char net_addr;
	FL_REGISTER_STRUCT *pROMFile = GetFlashActualData();
	net_addr = UartBuff[3];
	pROMFile->ID = net_addr;
	WriteFlash(pROMFile);
	MBClientSetHostID0(net_addr);
	OmniComm_Reply_ok();
}

void Set_net_mode (void)
{
	if(omnicomm_addr != MBClientGetHostID0()) return;
	FL_REGISTER_STRUCT *pROMFile = GetFlashActualData();
	omn_param.e2_net_mode = UartBuff[3];
	pROMFile->omn_param.e2_net_mode = omn_param.e2_net_mode;
	WriteFlash(pROMFile);
	OmniComm_Reply_ok();
}

void Config_read (void)
{
	UartBuff[6] = (unsigned char)(omn_param.e2_id >> 24);
	UartBuff[5] = (unsigned char)(omn_param.e2_id >> 16);
	UartBuff[4] = (unsigned char)(omn_param.e2_id >> 8);
	UartBuff[3] = (unsigned char)omn_param.e2_id;
	UartBuff[8] = (unsigned char)0;
	UartBuff[7] = (unsigned char)0;
	UartBuff[10] = (unsigned char)0;
	UartBuff[9] = (unsigned char)0;
	UartBuff[12] = (unsigned char)0;
	UartBuff[11] = (unsigned char)0;
	UartBuff[13] = 0;
	UartBuff[15] = (unsigned char)0;
	UartBuff[14] = (unsigned char)0;
	UartBuff[17] = (unsigned char)0;
	UartBuff[16] = (unsigned char)0;
	if(omnicomm_addr == 255)
		UartBuff[18] = 	5;
	else
		UartBuff[18] = omnicomm_addr;//MBClientGetHostID0();
	UartBuff[19] = omn_param.e2_net_mode;
	UartBuff[20] = 0;
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
		UartBuff[3] = omn_param.e2_t;
		UartBuff[4] = (unsigned char)omn_param.e2_n;
		UartBuff[5] = (unsigned char)(omn_param.e2_n >> 8);
		UartBuff[6] = (unsigned char)omn_param.e2_f_curr;
		UartBuff[7] = (unsigned char)(omn_param.e2_f_curr >> 8);
		omnicomm_frame_size = 9;
		OmniComm_Send_frame();
	}
	else
	{
		
		//omnicomm_frame_size=0;
		update_data();
		strcpy((char *)UartBuff, "");
		
		//frequency
		strcat((char *)UartBuff, "F=");
		sprintf(str, "%X", omn_param.e2_f_curr);
		//itoa(omn_param.e2_f_curr, str, 16);

		length=strlen(str);
		if(length<4)
		{
			for(i=0; i<(4-length);i++)
			{
				strcat((char *)UartBuff,"0");
			}
		}
		else
		{
			if(length>4)
				strcpy(str,"0000");
		}
		strcat((char *)UartBuff, str);
		
		//temperature
		strcat((char *)UartBuff, " t=");
		sprintf(str, "%X", omn_param.e2_t);

		length=strlen(str);
		if(length<2)
			for(i=0; i<(2-length);i++)
				strcat((char *)UartBuff,"0");
		else if(length>2)		
			strcpy(str,"00");

		strcat((char *)UartBuff, str);
		
		//level
		strcat((char *)UartBuff, " N=");
		sprintf(str, "%X", omn_param.e2_n);

		length=strlen(str);
		if(length<4)
			for(i=0; i<(4-length);i++)
				strcat((char *)UartBuff,"0");
		else if(length>4)		
			strcpy(str,"0000");

		strcat((char *)UartBuff, str);/**/
		strcat((char *)UartBuff, ".0\n\r");

		omnicomm_frame_size=strlen((char *)UartBuff);
		
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
		omn_param.e2_report_on = 1;
		if(omn_param.e2_period_auto == 0)
			omn_param.e2_period_auto = 1;		
	}
	else
	{		
		if(pFlashRegFile->omn_param.e2_report_on)
		{
			FL_REGISTER_STRUCT *pROMFile = GetFlashActualData();
			pROMFile->omn_param.e2_report_on = 0;
			WriteFlash(pROMFile);
		}	

		Enable_auto_old_mode();

		omnicomm_command_type = OLD_COMMAND_TYPE;
		omn_param.e2_report_on = 1;
		if(omn_param.e2_period_auto == 0)
			omn_param.e2_period_auto = 1;		
	}
}

void Fcurr_fmin (void)
{
	update_data();
	OmniComm_Reply_ok();
}

void Fcurr_fmax (void)
{
	update_data();
	OmniComm_Reply_ok();
}

void Tslope_t0 (void)
{
	OmniComm_Reply_ok();
}

void Set_fmin (void)
{
	OmniComm_Reply_ok();
}

void Set_fmax (void)
{
	OmniComm_Reply_ok();
}

void Set_pwm_max (void)
{
	OmniComm_Reply_ok();
}

void Set_pwm_mod (void)
{
	OmniComm_Reply_ok();
}

void Set_trl (void)
{
	OmniComm_Reply_ok();
}

void Set_filt_length (void)
{
	OmniComm_Reply_ok();
}

void Set_passw (void)
{
	OmniComm_Reply_ok();
}

void Set_period (void)
{
	if(omnicomm_addr != MBClientGetHostID0())
	{
		OmniComm_Reply_ok();
		return;
	}

	omn_param.e2_period_auto = UartBuff[3];
	if(omn_param.e2_period_auto == 0)
		omn_param.e2_period_auto = 1;

	FL_REGISTER_STRUCT *pROMFile = GetFlashActualData();
	pROMFile->omn_param.e2_period_auto = omn_param.e2_period_auto;
	WriteFlash(pROMFile);
	OmniComm_Reply_ok();
}

void Filt_leng_read (void)
{
	UartBuff[3] = 0;
	omnicomm_frame_size = 5;
	OmniComm_Send_frame();
}

void Passw_read (void)
{
	UartBuff[3] = 0;
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
	if(omnicomm_addr != MBClientGetHostID0())
	{
		OmniComm_Reply_ok();
		return;
	}

	FL_REGISTER_STRUCT *pROMFile = GetFlashActualData();

	if(_param==0)
	{
		pROMFile->omn_param.e2_report_on = 0;
		WriteFlash(pROMFile);
		Disable_auto_old_mode();
	}
	else if(_param==1)
	{
		Disable_auto_old_mode();
		omn_param.e2_report_on = 1;
		pROMFile->omn_param.e2_report_on = omn_param.e2_report_on;
		WriteFlash(pROMFile);		
	}
	else if(_param==2)
	{
		Enable_auto_old_mode();
		pROMFile->omn_param.e2_report_on = 0;
		WriteFlash(pROMFile);
	}
	OmniComm_Reply_ok();
}

void Enable_auto_old_mode (void)
{
	if(omnicomm_addr != MBClientGetHostID0()) return;

	if(!pFlashRegFile->omn_param.e2_old_contin)
	{
		FL_REGISTER_STRUCT *pROMFile = GetFlashActualData();
		pROMFile->omn_param.e2_old_contin = 1;
		WriteFlash(pROMFile);
	}	
}

void Disable_auto_old_mode (void)
{
	if(omnicomm_addr != MBClientGetHostID0()) return;

	if(pFlashRegFile->omn_param.e2_old_contin)
	{
		FL_REGISTER_STRUCT *pROMFile = GetFlashActualData();
		pROMFile->omn_param.e2_old_contin = 0;
		WriteFlash(pROMFile);
	}	
}

unsigned char Get_auto_old_mode (void)
{
	return(pFlashRegFile->omn_param.e2_old_contin);
}

