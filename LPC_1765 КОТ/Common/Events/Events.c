#include "Config.h"
#if(_EVENTS_)

#include "Events.h"
#include "User_Error.h"
#include "SetParametrs.h"
#include <string.h>
#include <stdio.h>

EVENT_HANDLE *pFistEvent = NULL;
uint8_t *pFistEventMem = NULL;
xQueueHandle events_queue = NULL;
char * opz_form(char *buf, int len);
float opz_calc(char *opz);

uint8_t EventID(char *name)
{
    EVENT_HANDLE *pEvent;
    pEvent = pFistEvent;
    while(pEvent != NULL)
    {
        if(strcmp((char *)pEvent->name,(char *) name) == 0)
        {
            return pEvent->ID;
        }
        pEvent = pEvent->pNext;
    }
    return 0xFF;
}

int PutEvenToQueue(uint8_t id, uint32_t data, char *name, int timeout)
{
    EVENTS_MESSAGE message;

    message.ID = id;
    message.data = data;
	message.name = name;
    return xQueueSend(events_queue, ( void * )&message, (portTickType)timeout);
}


void add_event(EVENT_HANDLE *new_event)
{
    if(pFistEvent == NULL) //Если добавляем первое событие
    {
        pFistEvent = new_event;
        pFistEvent->pNext = NULL;
    }
    else 
    {
        new_event->pNext = pFistEvent;
        pFistEvent = new_event;
    }
}

EVENT_HANDLE * get_event_handle(uint8_t id)
{
    EVENT_HANDLE *pEvent;
    pEvent = pFistEvent;
    while(pEvent != NULL)
    {
        if(pEvent->ID == id)
        {
            return pEvent;
        }
        pEvent = pEvent->pNext;
    }
    return 0;
}

char * Get_event_name(uint8_t id)
{
    EVENT_HANDLE *pEvent;
    pEvent = pFistEvent;
    while(pEvent != NULL)
    {
        if(pEvent->ID == id)
        {
            return pEvent->name;
        }
        pEvent = pEvent->pNext;
    }
    return 0;
}

uint8_t GetEventExistFlag(uint8_t id)
{
    EVENT_HANDLE *pEvent = get_event_handle(id);
    return pEvent->exist_flag;
}

void SetEventExistFlag(uint8_t id)
{
    EVENT_HANDLE *pEvent = get_event_handle(id);
    pEvent->exist_flag = 1;
}

void ClrEventExistFlag(uint8_t id)
{
    EVENT_HANDLE *pEvent = get_event_handle(id);
    pEvent->exist_flag = 0;
}

/****************************************************************************************************************************************************/

EVENT_INFO *pFirstCheck;


void add_event_to_list(EVENT_INFO *pCheck)
{
    if(pFirstCheck == NULL)
    {
        pFirstCheck = pCheck;
        pCheck->pNext = NULL;
    }
    else
    {
        pCheck->pNext = pFirstCheck;
        pFirstCheck = pCheck;
    }
}  



void CheckEvents(void)
{
    EVENT_INFO *pCheck = pFirstCheck;

    while(pCheck != 0)  // Пока не проверим все заданные события
    {

		pCheck->status =   opz_calc(pCheck->pOPZ);		 

		if(pCheck->status == 1)			  // Если условия для наступления события выполняются
		{
			if(pCheck->multiplicity_flag == 0)	  // Смотрим - однократноя или многократная фиксация событий
			{
				if(pCheck->event_exist_flag == 0)
				{	
					PutEvenToQueue(pCheck->event_ID, (uint32_t)pCheck->mes_num, pCheck->name, 100);  //  Отправляем в очередь событий сообщение
					pCheck->event_exist_flag = 1;
				} 
			}
			else																    
			{
				PutEvenToQueue(pCheck->event_ID, (uint32_t)pCheck->mes_num, pCheck->name, 0);  //  Отправляем в очередь событий сообщение
			}
		}
		else 
		{
			pCheck->event_exist_flag = 0;	   // сбрасываем флаг для выполнения события
		}
		pCheck = pCheck->pNext;
    }
}



// Инициализируем функции проверки и формирования событий 
void InitEventCheck(void)
{
    EVENT_INFO *pCheck, *pCheckPrev = 0;

    int count, i = 0;
    char *p;
	int len = 0, err = 0;
    char num[4];
	int correctEventsCounter = 0;

    if(GetSettingsFileStatus())
    {
        p = GetPointToElement("Number", "val");		 //  Число функций проверок
        if(p == 0)
        {
             AddError(ERR_INCORRECT_IVENT_FILE_STR);
             return;
        }
        count = atoi((char *)p);   //  Число функций проверок
        if(count > MAX_EVENTS_IN_FILE)
        {
            AddError(ERR_INCORRECT_IVENT_FILE_STR);
            return;
        }
        if(count == 0)
        {
            pCheck = 0;
            pFirstCheck = pCheck;
            return;
        }

        pCheck = (EVENT_INFO *)pvPortMalloc((size_t)(sizeof(EVENT_INFO) * count));  // Выделяем необходимое количество памяти для параметров каждой функции
        if(pCheck == NULL)
        {
             AddError(ERR_MEM_IVENT_FILE_STR);
             return;
        }
	
		memset((char *)pCheck, 0, sizeof(EVENT_INFO) * count);

		if(pFirstCheck == 0)
        	pFirstCheck = pCheck;   //  Глобальный указатель
		else pFirstCheck->pNext = pCheck;  
		pFistEventMem = (uint8_t *)pCheck;

        for(i = 0; i < count; i++)
        {
			err = 0;
            sprintf(num, "%lu", i);

            p = GetPointToElement(num, "Con");		 //Первое значение или датчик
            if(p == 0)
            {
                AddError(ERR_INCORRECT_IVENT_FILE_STR);
				err = 1;
                continue;
            }

			pCheck->pOPZ = opz_form((char *)p, strlen((char *)p));
			if(pCheck->pOPZ == NULL)
			{
				AddError(ERR_INCORRECT_IVENT_FILE_STR);
				err = 1;
                continue;
			}
			
            p = GetPointToElement(num, "M");	   //  Многократность повторения события
            if(p == 0)
            {
                AddError(ERR_INCORRECT_IVENT_FILE_STR);
				err = 1;
                continue;
            }
            pCheck->multiplicity_flag = atoi((char *)p);

            p = GetPointToElement(num, "E");		 // Событие для выполнения условия
            if(p != 0)
            	pCheck->event_ID = EventID(p);
			else pCheck->event_ID = 0xFF;

			memset(pCheck->mes_num, 0, 4);
			p = GetPointToElement(num, "Mes");		 // Сообщение для выполнения условия
            if(p != 0)
			{
				len = strlen((char *)p);
				if(len > 0 && len < 4) 
					strncpy(pCheck->mes_num, (char *)p, len);
			}

			p = GetPointToElement(num, "N");		 // Человеческое название события
            if(p != 0)
			{
				len = strlen((char *)p);
				if(len > 0 && len < 30) 
				{
					pCheck->name = (char *)pvPortMalloc((size_t)(len + 1));
					if(pCheck->name != NULL)
						strncpy(pCheck->name, (char *)p, (len + 1));
				}
			}else pCheck->name = NULL;


            pCheck->event_exist_flag = 0;
			pCheck->status = 0;

			if(i != 0)
				pCheckPrev->pNext = pCheck;  //  При первом разе писать не нужно (указатель указывает на ноль)
			pCheckPrev = pCheck;
			pCheckPrev->pNext = 0;
			pCheck = (EVENT_INFO *)(((EVENT_INFO *)pCheck) + 1);

			correctEventsCounter++;
        }
		if(err && !correctEventsCounter)
			pFirstCheck = 0;
		else 	
        return;
    }
    AddError(ERR_INCORRECT_IVENT_FILE_STR);

}

void DelAllEvents(void)
{
	EVENT_INFO *pCheck = NULL;
	if(pFistEventMem != 0)
	{
		pCheck = (EVENT_INFO *)pFistEventMem;
		while(pCheck)
		{
			if(pCheck->pOPZ != NULL)
				vPortFree(pCheck->pOPZ);
			if(pCheck->name != NULL)
				vPortFree(pCheck->name);
			pCheck = pCheck->pNext;	
		}
		vPortFree(pFistEventMem);
	}
	pFirstCheck = 0;
	pFistEventMem = 0;
}


/*Вычисление приоритета операции*/
int prior(char c) 
{      
    switch(c)
	{
        case '(': return 1;
        case '+': case '-': return 4;
        case '*': case '/': return 5;
		case '>': case '<': case '=': case '{': case '}': return 3;
		case '&': case '|': return 2;
        default: return 0;
    }
}

/*построение обратной польской записи*/
char * opz_form(char *buf, int len)
{
	#define STACK_LEN		30
	int was_op = 0, np = 0;		// Флаг того что последним символом была операция и счетчик скобок
	int sp = 0;					// указатель стека
	char stack[STACK_LEN], ch;		// стэк
	int  i = 0, j = 1;
	char *opz = pvPortMalloc((size_t)(len * 2));  //  *2 т.к разделяем каждый элемент в записи нулем
	
	if(buf == NULL || len == 0)
		goto fail;
	if(buf[0] == '+' || buf[0] == '-' || buf[0] == '*' || buf[0] == '/')  // Первый элемент в записе не может быть ареф знаком!!!
		goto fail;

	memset(stack, 0, STACK_LEN);

	while(buf[i] && i < len)	   // До конца строки или пока позволяет заданная длинна длинна
	{
		switch(buf[i])	       
		{
			case '(':
				stack[sp++] = buf[i++];			  //   проталкиваем скобку в стэк
				if(sp == STACK_LEN) goto fail;	  //   отслеживаем переполнение стэка
				np++;							  //   увеличиваем счетчик открытых скобок
				was_op = 0; 					  //   помечаем что это была не операция
				break;
			case '*':
			case '/':
			case '-':
			case '+':
			case '>':
			case '<':
			case '=':
			case '}':
			case '{':
			case '|':
			case '&':
				if(i == (len - 1)) goto fail;   //  последний символ не может быть операцией
				if(was_op == 1) goto fail;      //  две операции подряд идти не могут
				was_op = 1;					    //  запоминаем что была операция
				opz[j++] = 0;
				while(prior(buf[i]) <= prior(stack[sp-1]) && sp > 0)	// Пока приоритет текущей операции ниже или равен последний операции в стеке
				{										// помещаем операцию из стека в выходную строку
			    	opz[j++] = stack[--sp];
					opz[j++] = 0;						//  в выходной строке элемениты друг от друга отделяются нулями
					if(sp < 0) goto fail;			   //  проверка стэка
				}
                if(prior(buf[i]) > prior(stack[sp-1]) || sp == 0)	   // если приоритет операции выше последний операции в стэке
				{
					stack[sp++] = buf[i++];			  //   заталкиваем текущую операцию в стэк
					if(sp == STACK_LEN) goto fail;	  //   отслеживаем переполнение стэка
				}
				break;
			case ')':
				if(was_op) goto fail;    //  Закрывающая скобка сразу после операции идти не может
				if(np == 0) goto fail;   //  Открывающей скобки не было - это ошибка
				opz[j++] = 0;
				while((ch = stack[--sp]) != '(')	// пока не встретим в стэке открывающую скобку
				{
					   if(sp < 0) goto fail;	
				       opz[j++] = ch;
				} 
				if(sp < 0) goto fail;
				i++;				 // переходим на следующий символ в страке
			    np--;				 // Уменьшаем счетчик открытых скобок
				break;
			default:
				opz[j++] = buf[i++];		  //  Если идут просто символы кидаем их в выходной массив
				was_op = 0;
				break;
		}	
	}
	opz[j++] = 0;
	while(sp != 0)		   //  Выгружаем все оставшиеся операции из стека в выходныю страку
	{
		opz[j++] = stack[--sp];
		opz[j++] = 0;
	}
	opz[--j] = 0; // добовляем нуль терминатор
	opz[0] = j;   // в самом начале храним длинну выражения
	if(np) goto fail;   // Если остались незакрытые скобки - ошибка

	return opz;
	
fail:
	vPortFree(opz);	
	return NULL;

	 
}

float opz_calc(char *opz)
{
	SENSOR_HANDLE pSensor;
	float stack[10];
    float res = 0, operand = 0;
    int sp = 0, i = 1;
	int len = opz[0];

	while(opz[i] != 0 && i < len)
	{
		if(prior(opz[i]) == 0)
		{
			pSensor = OpenSensorProc((char *)&opz[i]);
			if(pSensor != NULL)
			{
				ReadSensor(pSensor , (uint8_t *)&operand, 1);
				if(*((uint32_t *)&operand) == QNaN){
					SET_QNaN(&res);
					return res;
				}
			}
			else  operand = atof((char *)&opz[i]);

			stack[sp++] = operand;
			i += (strlen((char *)&opz[i]) + 1);
			continue;
		}

		switch(opz[i])
		{
			case '+':
				res = stack[sp - 2] + stack[sp - 1];
				break;
			case '-':
				res = stack[sp - 2] - stack[sp - 1];
				break;
			case '*':
				res = stack[sp - 2] * stack[sp - 1];
				break;
			case '/':
				res = stack[sp - 2] / stack[sp - 1];
				break;
			case '>':
				res = (stack[sp - 2] > stack[sp - 1]) ? 1 : 0;
				break;
			case '<':
				res = (stack[sp - 2] < stack[sp - 1]) ? 1 : 0;
				break;
			case '=':
				res = (stack[sp - 2] == stack[sp - 1]) ? 1 : 0;
				break;
			case '{':
				res = (stack[sp - 2] <= stack[sp - 1]) ? 1 : 0;
				break;
			case '}':
				res = (stack[sp - 2] >= stack[sp - 1]) ? 1 : 0;
				break;
			case '&':
				res = ((uint32_t)stack[sp - 2] & (uint32_t)stack[sp - 1]) ? 1 : 0;
				break;
			case '|':
				res = ((uint32_t)stack[sp - 2] | (uint32_t)stack[sp - 1]) ? 1 : 0;
				break;
		}
		stack[sp - 2] = res;
        sp--;
		i += 2;   // переходим на следующий элемент в выражении
	}	 
    return stack[--sp];
}

#endif


