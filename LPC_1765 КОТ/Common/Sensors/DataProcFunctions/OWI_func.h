#include "Config.h"
#if(_SENSORS_)
#ifndef OWI_FUNC_H
#define OWI_FUNC_H


typedef struct tagOWISensInfo
{
	SENSOR 		*pSInfo[5];   //  ������ �������� OWI
	int			pass_count;	   //  ������� ����������� ����� ��������
	int 		pass_lim;	   //  ������� ����� ����� ���������� ����� ������ ���� Owi ������ ��� � 2 ���
	int			cur_sen;	   //  ����� �������� �������
	int			num_sens;	   //  ������� ����� ����������������� �������� owi
}OWI_SENS_INFO;

#endif
#endif

