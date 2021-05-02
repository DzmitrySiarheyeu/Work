



// ADC Clock = CLK / DIVIDER
#define ADC_CLOCK_DIVIDER_2    1
#define ADC_CLOCK_DIVIDER_4    2
#define ADC_CLOCK_DIVIDER_8    3
#define ADC_CLOCK_DIVIDER_16   4
#define ADC_CLOCK_DIVIDER_32   5
#define ADC_CLOCK_DIVIDER_64   6
#define ADC_CLOCK_DIVIDER_128  7

//-------------SET--------------------

#define ADC_CLOCK_DIVIDER ADC_CLOCK_DIVIDER_64
#define ADC_ROUND_COUNT	16
#define CHANGE_REF_DELAY_MS	1
//-------------SET--------------------

#define ADC_SET_REF_VCC cbi(ADMUX, REFS1); sbi(ADMUX, REFS0);  
#define ADC_SET_REF_1_1 sbi(ADMUX, REFS1); sbi(ADMUX, REFS0); 

//Размер буфера драйвера adc
#define ADC_BUF_MAX					16

typedef struct AdcReg
{
    uint8_t count;
	uint32_t Acum;
	uint16_t	buf[ADC_BUF_MAX];
	uint16_t	proc_value;
	uint8_t		in;				// указатель на входной элемент буфера
	uint8_t		out;			// указатель на выходной элемент буфера
} ADC_BUF;


void ADCInit(void);
unsigned short ADCRead(unsigned char channel);
void ADCSetChannel(unsigned char channel);
double ADCGetRoundedValue(unsigned char channel);
void IsrTimeOutADC(void);
