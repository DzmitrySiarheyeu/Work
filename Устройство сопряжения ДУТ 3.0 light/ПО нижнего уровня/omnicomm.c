#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "base_type.h"
#include <string.h>
#include "mbfanction0.h"
#include "iit_boot.h"
#include "omnicomm.h"

//omnicomm variables
unsigned char	omnicomm_addr, omnicomm_cmd;

volatile unsigned char omnicomm_status, omnicomm_frame_size, omnicomm_command_type;

volatile unsigned long omnicomm_timer;

unsigned char is_frame_ok(void)
{
	//if(omnicomm_status != OMNICOMM_FRAME_OK) // ����� � ������� �� ������� ������ ������� ������������� ������
	//	return 0;
	if(BufferSize0 < 3) // ����� � ������� �� ������� ������ ������� ������������� ������
		return 0;
    if(OmniComm_Check_old_frame()==OMNICOMM_FRAME_OK)
		return 1;
	if(OmniComm_Check_frame() != OMNICOMM_FRAME_OK)
	{
		omnicomm_status = OMNICOMM_IDLE;
		return 0;
	}
	if(net_mode == INDEPENDENT_MODE && omnicomm_addr == 0xFF)
		return 1;
	if(omnicomm_addr == MBClientGetHostID0())
		return 1;
	omnicomm_status = OMNICOMM_IDLE;
	return 0;
}

void do_omnicom(void)
{
	if(report_on)
	{
		if(omnicomm_timer > ((unsigned long)(F_CPU/TIMER0_CLOCK/256)) * period_auto)
		{
			if(omnicomm_command_type == NEW_COMMAND_TYPE)
			{
				omnicomm_addr = MBClientGetHostID0();
				omnicomm_cmd = DATA_CONTIN;
				Data_read(NEW_COMMAND_TYPE);
			}
			else
				Data_read(OLD_COMMAND_TYPE);
			omnicomm_timer = 0;
		}
	}
	if (is_frame_ok())
	{
		report_on = 0;
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
								}//�� ��������, �.�. ���� ���������� �� ������. ������ �������������� ������ ������� 0x44 0xFF - ��� ����������, ��� ��������� � ��������� ���������
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
			case SET_AUTO_MODE:	{Set_auto_mode(DataBuffer0[3]); break;}//������� ������� � �������� DATA_CONTIN, ������������� ����� ������ ������ ����� ������ �����������, �.�. ���������� ����� ����� ������� ������ "����" ������ ������ �� RS485
			default: OmniComm_Reply_ok();
		}
	}
}


unsigned char OmniComm_Do_crc (unsigned char frame_size)
{
	unsigned char i;
	unsigned char omnicomm_crc = 0;

	for(i = 0; i < frame_size - 1; i++)
	{
		omnicomm_crc = crc8(DataBuffer0[i], omnicomm_crc);
	}
	return(omnicomm_crc);
}


void OmniComm_Send_frame (void)	//����� ������� ���� ������� � omnicomm_frame_size ����� �������� ������ ������ (���������� ���� � ������,
{								//�������� ������� � crc)
	
	omnicomm_status = OMNICOMM_BUSY;//"������" ��������
	SET_DE485_0();//�������� ����� ��������
	DataBuffer0[0] = OMNICOMM_SLAVE_PREFIX;
	DataBuffer0[1] = omnicomm_addr;
	DataBuffer0[2] = omnicomm_cmd;
	DataBuffer0[omnicomm_frame_size - 1] = OmniComm_Do_crc(omnicomm_frame_size);
	pBuf0 = DataBuffer0;
	BufferSize0 = omnicomm_frame_size;
	UCSR0B |= (1 << UDRIE0);		//����� ��������. ���������� �������� �����
								//����������� �� ���������� USART_RX_vect
	while (!(UCSR0B & (1 << RXEN0)));			//���� �� ��������� �����.
}

void OmniComm_Send_frame_old_data (void)	//����� ������� ���� ������� � omnicomm_frame_size ����� �������� ������ ������ (���������� ���� � ������,
{								//�������� ������� � crc)
	
	omnicomm_status = OMNICOMM_BUSY;//"������" ��������
	SET_DE485_0();//�������� ����� ��������

	pBuf0 = DataBuffer0;
	BufferSize0 = omnicomm_frame_size;
	UCSR0B |= (1 << UDRIE0);		//����� ��������. ���������� �������� �����
								//����������� �� ���������� USART_RX_vect
	while (!(UCSR0B & (1 << RXEN0)));			//���� �� ��������� �����.
}

void OmniComm_Reply_error (void)//������� ������ ������� �� ������
{
	DataBuffer0[3] = OMNICOMM_ERROR;
	omnicomm_frame_size = 5;
	OmniComm_Send_frame();
}


void OmniComm_Reply_ok (void)//������� ������ ������� �� ���������� ������
{
	DataBuffer0[3] = OMNICOMM_NO_ERRORS;
	omnicomm_frame_size = 5;
	OmniComm_Send_frame();
}

unsigned char OmniComm_Check_frame (void)	//������� ��������� ����� � ����������
{											//������� ������� ��� ���������� ������
	omnicomm_frame_size = BufferSize0;
	if (DataBuffer0[ 0 ] == OMNICOMM_MASTER_PREFIX && BufferSize0 > 3)	//���� ������ ������� �������
	{
		if (OmniComm_Do_crc(BufferSize0) == DataBuffer0[BufferSize0 - 1])
		{
			omnicomm_addr = DataBuffer0[ 1 ];
			omnicomm_cmd = DataBuffer0[ 2 ];
			return(OMNICOMM_FRAME_OK);
		}
		else
		{
			omnicomm_addr = DataBuffer0[ 1 ];
			omnicomm_cmd = DataBuffer0[ 2 ];
			return(OMNICOMM_FRAME_ERROR);
		}
		
	}
	return(OMNICOMM_FRAME_ERROR);
}

unsigned char OmniComm_Check_old_frame (void)	//������� ��������� ����� � ����������
{				
	if(DataBuffer0[0]==OMNICOMM_OLD_PREFIX)
	{
		if(DataBuffer0[1] == 0x4F)
		{
			omnicomm_cmd = DATA_READ_OLD;
	 		omnicomm_addr = MBClientGetHostID0();
			return(OMNICOMM_FRAME_OK);	
		}
		if(DataBuffer0[1] == 0x50)
		{
			omnicomm_cmd = DATA_CONTIN_OLD;
			omnicomm_addr = MBClientGetHostID0();
			return(OMNICOMM_FRAME_OK);
		}	
	}
	return(OMNICOMM_FRAME_ERROR);
}
 
