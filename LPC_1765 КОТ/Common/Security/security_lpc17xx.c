// 28.08.2014 - ��� ���������� ������ � ������ � ������, ��������� ����� ����� ������!!!
#include "Config.h"
#if(_SECURITY_ == OPTION_ON)
#include "Security.h"
#include "SetParametrs.h"
#include "User_Error.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/*********************************** ��� IAP*****************************/
static unsigned param_table[5];
static unsigned result_table[5];

static
void iap_entry(unsigned param_tab[],unsigned result_tab[])
{
    void (*iap)(unsigned [],unsigned []);

    iap = (void (*)(unsigned [],unsigned []))IAP_ADDRESS;
    iap(param_tab,result_tab);
}


void get_ID(void)
{
    param_table[0] = READ_SERIAL_NUM;
    iap_entry(param_table,result_table);
}

/************************************************************************/



static SECURITY_INFO sec_info = {.auth_list = 0, .phone_num_list = 0};




/*********************���������� ���������� ������***********************/

uint32_t calc_hash(void)
{
	uint32_t hash = FNV_32_INIT_VAL;
	int i = 0;

	get_ID();
	
	for(i = 1; i < 5; i++)
	{
		hash ^= result_table[i];
      	hash *= FNV_32_PRIME;
	}
	
	return 	hash;
}

/************************************************************************/




/****************������� ����������� ������������� ������� ������ � ��������� �����������****************/

int NetAccessRequest(char *login, char *password)
{
	int i = 0;
	int l_len = strlen(login);

	//  ������� �������� �� ��������� �� ������ ������ � ������ �������� ��������
	if(l_len > MAX_LOGIN_LENGTH || strlen(password) != MAX_PASS_LENGTH) return 0;

	//  		���� ������ ������ ������� � ������� ���������� ������
	//          ����� ���� ��� ��������� ������
	if(l_len == ADMIN_LOGIN_LENGTH())
	{
		if(strncmp(login, ADMIN_LOGIN, l_len) == 0 && strncmp(password, sec_info.admin_pas, MAX_PASS_LENGTH) == 0) return 1;
	}

	if(sec_info.auth_list == NULL) return 1; //������ ��� 0!!!!! ���� � ����������� ���� ��������, ��������� ������ ��������� ������


	// ��������� ��������� ���� ������� �������
	for(i = 0; i < sec_info.number_auth_records; i++)
	{
		 if(strncmp(login, (sec_info.auth_list + i)->login, l_len) == 0 && strncmp(password, (sec_info.auth_list + i)->password, MAX_PASS_LENGTH) == 0) return 1;
	}

	// ������� ������ � ������ ����������� �� �������
	return 0;

}

/************************������� �������� ���������� ������� ��� ���������******************************/

int SmsAccessRequest(char *num, char *password)
{
	int i = 0;

	//  ������� �������� ���������� ����� ������ � ����������� ������
	if(strlen(num) != AUTH_PHONE_NUM_LENGTH || strlen(password) != MAX_PASS_LENGTH) return 0;

	//  ������ ������ ����������� � ������ ������ ������
	if(strncmp(password, sec_info.admin_pas, MAX_PASS_LENGTH) == 0) return 1;

	if(sec_info.auth_list == NULL) return 0; // ���� � ����������� ���� ��������, ��������� ������ ��������� ������

	// ���� ������ �������� ����� ����� � �������
	if(strncmp(password, sec_info.sms_pas, MAX_PASS_LENGTH) != 0) return 0;

	//  ���� ������ ������, ������� ����� �� ����������� ������ ���������

	if(sec_info.numder_sms_num > 0) //  ���� �������� ���������� �� ���������� �������
	{								 //  ����������� �� ����� ������ �������, ���� ������� - ������ ������
		 for(i = 0; i < sec_info.numder_sms_num; i++)
		 {
		 	  if(strncmp(num, (sec_info.phone_num_list + i*AUTH_PHONE_NUM_LENGTH), AUTH_PHONE_NUM_LENGTH) == 0) return 1;
		 }

		 // ���� ��� ������ � ����������������� ������ - ������ ������
		 return 0;
	}

	// ���� ���������� �� ������� �� ����� - ������ ������
	return 1;
}

/*******************************************************************************************************/



/********************************������� �������� ��������� ������**************************************/

void GetPassword(char *password, uint16_t num)
{
	if(password == NULL) return;
	if(num > sec_info.number_auth_records) return;

	memcpy(password, (sec_info.auth_list + num)->password, MAX_PASS_LENGTH);
}

/*******************************************************************************************************/



/*************************������� �������� � ���������� ���� ID ����������******************************/

void GetProcessorID(char *id)
{
	int i = 0;

	get_ID();

	for(i = 0; i < 4; i++)
	{
		sprintf((id + i*8), "%08x", result_table[i + 1]);
	}
}

/*******************************************************************************************************/

																			   
/********************************������� ������������� ������ ������************************************/

void default_security_init(void)
{
	 sec_info.auth_list = NULL;
	 sec_info.number_auth_records = 0;
	 sec_info.numder_sms_num = 0;
	 sec_info.phone_num_list = NULL;
}

void SecurityInit(void)
{
	AUTH_INFO *pAuth = NULL;
	char *p_data = NULL;
	uint16_t temp = 0;
	int i = 0, k = 0;
	uint32_t hesh = 0;
	
	hesh = calc_hash();
	sprintf((char *)&sec_info.admin_pas, "%08x", hesh);	

	if(GetSettingsFileStatus())
    {
		pAuth = (AUTH_INFO *)pvPortMalloc((size_t) sizeof(AUTH_INFO) * 1);	  //  �������� ������ ��� ��������� ������� �������
	    if( pAuth == NULL )												      //  ���� ������������� ������ ���� ������� ������
	    {
			default_security_init();
	        AddError(NULL);
	        return;
	    }


		p_data = (char *)GetPointToElement("SECURITY", "Login");		//  ����� ����� �� ����� ��������
        if(p_data == 0 || strlen(p_data) > MAX_LOGIN_LENGTH)		// 	���� ������ ������ �� �������� ���������� �������� - ������ � �� ������
        {
            memcpy(pAuth->login, "ets", 4);
        }
        else { memcpy(pAuth->login, (char *)p_data, MAX_LOGIN_LENGTH); }

		p_data = (char *)GetPointToElement("SECURITY", "Password");		//  ����� ����� �� ����� ��������
        if(p_data == 0 || strlen(p_data) > MAX_PASS_LENGTH)		// 	���� ������ ������ �� �������� ���������� �������� - ������ � �� ������
        {
            default_security_init();
	        AddError(NULL);
	        return;
        }
        else { memcpy(pAuth->password, (char *)p_data, MAX_PASS_LENGTH);	memcpy(sec_info.sms_pas, (char *)p_data, MAX_PASS_LENGTH); }

		sec_info.auth_list = pAuth;				   //  ������������� ��������� �� ���� ���������
		sec_info.number_auth_records = 1;

		p_data = (char *)GetPointToElement("SECURITY", "Quantity_phone");		    //  ���������� ���������� ������� ��� ������� ��� ��������� (���� == 0, �� ������ �� ������� ��������)
        if(p_data == 0 || (temp = (uint16_t)atoi(p_data)) > PHONE_NUM_QUANTITY)	  
        {
        	default_security_init();
	        AddError(NULL);
	        return;
        }
        else sec_info.numder_sms_num = temp;

		if(sec_info.numder_sms_num > 0)
		{

			sec_info.phone_num_list = (char *)pvPortMalloc((size_t)(AUTH_PHONE_NUM_LENGTH * sec_info.numder_sms_num));   //  �������� ������ ��� ������ ��������� ��� �������
			if( sec_info.phone_num_list == NULL )
		    {
		        default_security_init();
		        AddError(NULL);
		        return;
		    }
	
	
			p_data = (char *)GetPointToElement("SECURITY", "Phones");  // ����������� ������ � ����������� ��������
			if(p_data == NULL)
	        {
	            default_security_init();
		        AddError(NULL);
		        return;
	        }
			else
			{
				
				for(i = 0; i < sec_info.numder_sms_num; i++ )	 // �������� ������ �� ������ � ��� ������
				{
					memcpy((sec_info.phone_num_list + i*AUTH_PHONE_NUM_LENGTH), (p_data + k + i*AUTH_PHONE_NUM_LENGTH), AUTH_PHONE_NUM_LENGTH);     //  ��� ��� ������ ��������� � ������ ���������� ���� 
					k++;	   																										 //   �� ����� �������, ���������� �� ���������� 
				}			  																										//  ��� ����� ������������ ���������� �������� k
			}
		}

	}
	else
	{
		default_security_init();
	    AddError(NULL);
	    return;
	}
}

/*******************************************************************************************************/

void DelAllSecurity(void)
{
	if(sec_info.auth_list != 0)
		{vPortFree(sec_info.auth_list); sec_info.auth_list = 0;}
	if(sec_info.phone_num_list != 0)
		{vPortFree(sec_info.phone_num_list); sec_info.phone_num_list = 0;}
}

#else

//  ������ �������

int NetAccessRequest(char *login, char *password){ return 1;}
int SmsAccessRequest(char *num, char *password){ return 1;}
void GetPassword(char *password, uint16_t num){}
void GetProcessorID(char *id){}
void SecurityInit(void){}
void DelAllSecurity(void){}


#endif   //  _SECURITY_

