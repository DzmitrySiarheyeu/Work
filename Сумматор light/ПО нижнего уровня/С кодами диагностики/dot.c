#include "dot.h"

double VolumePercent = 0;      // Volume from all tanks

// вычисление процента заполнения датчика
double GetDotVolume(int dotIndex)
{
	byte i;
	double  temp;
	double  x, x1, x2, y1, y2;  // Переменные используемы для вычисления по уравнению прямой промежуточного значения между двумя табличными значениями АЦП
	DOT_PARAM * dotParam = &(RegFile.mb_struct.dot_info.DOT[dotIndex]);


	x =	Reverse4(dotParam->frequency);

	// Находим максимально допустимую частоту датчика
	// Вообще то она долджна быть DEFAULT_DOT_MAX_FREQ, но если она вдруг задана выше то мы должны это учесть
	temp = get_high_frequency(dotIndex);
	if(temp < DEFAULT_DOT_MAX_FREQ)
		temp = DEFAULT_DOT_MAX_FREQ;

	if (x >= (get_low_frequency(dotIndex) - DOT_DELTA)  && x <= (temp + DOT_DELTA))			
	{
		if(x < get_low_frequency(dotIndex))
			x = get_low_frequency(dotIndex);
	
		if(x > get_high_frequency(dotIndex))
			x = get_high_frequency(dotIndex);
	}
	else
	{
		return 0;
	}
																									    	
	i = Reverse2(dotParam->N) - 2;
	while((x < get_double_P(&(ProgramDotTable[dotIndex].Ftable[i]))) && i > 0) i--; // Находим после какой точки в таблице находится потдерживаемое значение АЦП
	// Далее вычисляем по уравнению кривой промежуточное знаение между двумя табличными
	// Такой подход дает вычисление конкретного значения АЦП приконкретном уровне топлива. Тем самым мы получаем не дискретную систему а непрерывную.
	// Таким образом мы избегаем колебаниие стрелки между двумя табличными значениями
	
    x1 = (get_double_P(&(ProgramDotTable[dotIndex].Ftable[i]))); 
	x2 = (get_double_P(&(ProgramDotTable[dotIndex].Ftable[i+1])));
	
    y1 = (get_double_P(&(ProgramDotTable[dotIndex].Vtable[i])));
	y2 = (get_double_P(&(ProgramDotTable[dotIndex].Vtable[i+1])));
	
    temp  = (y2 - y1) * (x - x1);
	temp /= (x2 - x1);
	temp += (y1);
		
	if(temp > -1.0 && temp < 100000)
		return temp;
	return 0;
}

double GetAllDotsVolume()
{
    byte i;
    double sum = 0;
    for (i = 0; i < Reverse2(RegFile.mb_struct.dot_info.N); i++)
    {
        sum += GetDotVolume(i);
    }
    
    return sum;
}

double GetFullTanksVolume()
{
    byte i;
    double sum = 0; // чтобы, если у нас нет датчиков небыло деления на ноль
    for (i = 0; i < Reverse2(RegFile.mb_struct.dot_info.N); i++)
    {
        unsigned short lastVolume = Reverse2(RegFile.mb_struct.dot_info.DOT[i].N) - 1;
        sum += (get_double_P(&(ProgramDotTable[i].Vtable[lastVolume])));
    }
    
    return sum;
}

inline double GetFuelPercent(void)
{
	double temp = GetAllDotsVolume() / GetFullTanksVolume() ;
	if(temp > -1.0 && temp < 2)
		return temp;
	return 0;

}

static unsigned char dotState[MAX_DOT_COUNT];

void set_Freq(double freq);

inline unsigned char  DotsProcess()
{
   	
	unsigned short dotIndex;
	unsigned short dotCount = Reverse2(RegFile.mb_struct.dot_info.N);
	double generatorFrequency;

#ifdef FREQUENCY_INPUT		
	FrequancyMeasure();
#endif

#ifdef ANALOG_INPUT
	AnalogDotMeasure();
#endif

    for (dotIndex = 0; dotIndex < dotCount; dotIndex ++)
    {
		dotState[dotIndex] = ERROR_OK_LED;
		generatorFrequency = Reverse4(RegFile.mb_struct.dot_info.DOT[dotIndex].frequency);
#ifdef FREQUENCY_INPUT
		if(generatorFrequency < 450)
		{
			dotState[dotIndex] = RS_DOT_STATE_TIMEOUT;
			if(generatorFrequency > 200)
				VolumePercent = (500 - generatorFrequency)/(-1000.0);
			else
				VolumePercent = -0.28 + dotIndex * 0.02;
			return ERROR_DOT + dotIndex;
		}
		
#endif				
    }

	VolumePercent = GetFuelPercent();
	RegFile.mb_struct.VolumePercent = Reverse4(VolumePercent);

/*	for (dotIndex = 0; dotIndex < dotCount; dotIndex ++)
	{
		if (dotState[dotIndex] == RS_DOT_STATE_TIMEOUT)
					return ERROR_DOT + dotIndex;
	}
*/
		
	return ERROR_OK_LED;
}
