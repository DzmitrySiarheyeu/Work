#include "Config.h"
#if(_SENSORS_)
#ifndef OWI_FUNC_H
#define OWI_FUNC_H


typedef struct tagOWISensInfo
{
	SENSOR 		*pSInfo[5];   //  Массив датчиков OWI
	int			pass_count;	   //  Счетчик пропущенных пулов датчиков
	int 		pass_lim;	   //  Сколько пулов нужно пропустить чтобы пулить один Owi датчик раз в 2 сек
	int			cur_sen;	   //  Номер текущего датчика
	int			num_sens;	   //  Сколько всего зарегистрированно датчиков owi
}OWI_SENS_INFO;

#endif
#endif

