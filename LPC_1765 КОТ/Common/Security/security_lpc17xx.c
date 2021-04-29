// 28.08.2014 - При отсутствии данных о логине и парале, действует любой логин пароль!!!
#include "Config.h"
#if(_SECURITY_ == OPTION_ON)
#include "Security.h"
#include "SetParametrs.h"
#include "User_Error.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/*********************************** Для IAP*****************************/
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




/*********************Вычисление админского пароля***********************/

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




/****************Функция определения существования учетной записи с заддыными риквизитами****************/

int NetAccessRequest(char *login, char *password)
{
	int i = 0;
	int l_len = strlen(login);

	//  Сначало проверим не превышает ли длинна логина и пороля заданной величины
	if(l_len > MAX_LOGIN_LENGTH || strlen(password) != MAX_PASS_LENGTH) return 0;

	//  		Если длинна логина совпала с длинной админского логина
	//          может быть это реквизиты админа
	if(l_len == ADMIN_LOGIN_LENGTH())
	{
		if(strncmp(login, ADMIN_LOGIN, l_len) == 0 && strncmp(password, sec_info.admin_pas, MAX_PASS_LENGTH) == 0) return 1;
	}

	if(sec_info.auth_list == NULL) return 1; //Раньше был 0!!!!! Если с настройками были проблемы, действует только админская запись


	// Проверяем реквизиты всех учетных записей
	for(i = 0; i < sec_info.number_auth_records; i++)
	{
		 if(strncmp(login, (sec_info.auth_list + i)->login, l_len) == 0 && strncmp(password, (sec_info.auth_list + i)->password, MAX_PASS_LENGTH) == 0) return 1;
	}

	// Учетной записи с такими риквизитами не найдено
	return 0;

}

/************************Функция проверки реквизитов доступа смс сообщения******************************/

int SmsAccessRequest(char *num, char *password)
{
	int i = 0;

	//  Сначало проверим совпадение длины пароля и телефонного номера
	if(strlen(num) != AUTH_PHONE_NUM_LENGTH || strlen(password) != MAX_PASS_LENGTH) return 0;

	//  пароль админа принимается с любого номера всегда
	if(strncmp(password, sec_info.admin_pas, MAX_PASS_LENGTH) == 0) return 1;

	if(sec_info.auth_list == NULL) return 0; // Если с настройками были проблемы, действует только админская запись

	// если пороль неверный сразу отказ в доступе
	if(strncmp(password, sec_info.sms_pas, MAX_PASS_LENGTH) != 0) return 0;

	//  если пороль верный, смотрим нужно ли фильтровать номера телефонов

	if(sec_info.numder_sms_num > 0) //  если включена фильтрация по телефонным намерам
	{								 //  пробигаемся по всему списку номеров, если находим - доступ открыт
		 for(i = 0; i < sec_info.numder_sms_num; i++)
		 {
		 	  if(strncmp(num, (sec_info.phone_num_list + i*AUTH_PHONE_NUM_LENGTH), AUTH_PHONE_NUM_LENGTH) == 0) return 1;
		 }

		 // Если смс пришло с неподдерживаемого номкра - доступ закрыт
		 return 0;
	}

	// если фильтрация по номерам не стоит - доступ открыт
	return 1;
}

/*******************************************************************************************************/



/********************************Функция возврата заданного пороля**************************************/

void GetPassword(char *password, uint16_t num)
{
	if(password == NULL) return;
	if(num > sec_info.number_auth_records) return;

	memcpy(password, (sec_info.auth_list + num)->password, MAX_PASS_LENGTH);
}

/*******************************************************************************************************/



/*************************Функция возврата в символьном виде ID процессора******************************/

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

																			   
/********************************Функция инициализации модуля защиты************************************/

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
		pAuth = (AUTH_INFO *)pvPortMalloc((size_t) sizeof(AUTH_INFO) * 1);	  //  Выделяем память под реквизиты учетных записей
	    if( pAuth == NULL )												      //  пока предусмотрена только одна учетная запись
	    {
			default_security_init();
	        AddError(NULL);
	        return;
	    }


		p_data = (char *)GetPointToElement("SECURITY", "Login");		//  порль берем из файла настроек
        if(p_data == 0 || strlen(p_data) > MAX_LOGIN_LENGTH)		// 	если длинна пароля не заданное количество символов - ерунда а не пороль
        {
            memcpy(pAuth->login, "ets", 4);
        }
        else { memcpy(pAuth->login, (char *)p_data, MAX_LOGIN_LENGTH); }

		p_data = (char *)GetPointToElement("SECURITY", "Password");		//  порль берем из файла настроек
        if(p_data == 0 || strlen(p_data) > MAX_PASS_LENGTH)		// 	если длинна пароля не заданное количество символов - ерунда а не пороль
        {
            default_security_init();
	        AddError(NULL);
	        return;
        }
        else { memcpy(pAuth->password, (char *)p_data, MAX_PASS_LENGTH);	memcpy(sec_info.sms_pas, (char *)p_data, MAX_PASS_LENGTH); }

		sec_info.auth_list = pAuth;				   //  устанавливаем указатель на наши риквизиты
		sec_info.number_auth_records = 1;

		p_data = (char *)GetPointToElement("SECURITY", "Quantity_phone");		    //  Количество телефонных номеров для фильтра смс сообщений (если == 0, то фильтр по номерам отключен)
        if(p_data == 0 || (temp = (uint16_t)atoi(p_data)) > PHONE_NUM_QUANTITY)	  
        {
        	default_security_init();
	        AddError(NULL);
	        return;
        }
        else sec_info.numder_sms_num = temp;

		if(sec_info.numder_sms_num > 0)
		{

			sec_info.phone_num_list = (char *)pvPortMalloc((size_t)(AUTH_PHONE_NUM_LENGTH * sec_info.numder_sms_num));   //  Выделяем память под список доступных тел номеров
			if( sec_info.phone_num_list == NULL )
		    {
		        default_security_init();
		        AddError(NULL);
		        return;
		    }
	
	
			p_data = (char *)GetPointToElement("SECURITY", "Phones");  // Вытаскиваем строку с телефонными номерами
			if(p_data == NULL)
	        {
	            default_security_init();
		        AddError(NULL);
		        return;
	        }
			else
			{
				
				for(i = 0; i < sec_info.numder_sms_num; i++ )	 // копируем номера из строки в наш список
				{
					memcpy((sec_info.phone_num_list + i*AUTH_PHONE_NUM_LENGTH), (p_data + k + i*AUTH_PHONE_NUM_LENGTH), AUTH_PHONE_NUM_LENGTH);     //  так как номера телефонов в строке отделяются друг 
					k++;	   																										 //   от друга запятой, необходимо ее пропускать 
				}			  																										//  для этого используется переменная смещения k
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

//  Пустые функции

int NetAccessRequest(char *login, char *password){ return 1;}
int SmsAccessRequest(char *num, char *password){ return 1;}
void GetPassword(char *password, uint16_t num){}
void GetProcessorID(char *id){}
void SecurityInit(void){}
void DelAllSecurity(void){}


#endif   //  _SECURITY_

