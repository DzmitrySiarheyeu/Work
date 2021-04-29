#pragma once

#include "util.h"

//-----------GREEN LED--------------------
#define LED_PORT    PORTC   
#define LED_DDR     DDRC
#define LED_PIN     PC3

#define LED_INIT    sbi(LED_PORT, LED_PIN); sbi(LED_DDR, LED_PIN);

#define LED_ON      cbi(LED_PORT, LED_PIN);
#define LED_OFF     sbi(LED_PORT, LED_PIN);
#define LED_TOGGLE  LED_PORT ^= (1 << LED_PIN);

//------------OC1B------------------------
#define OC1B_DDR    DDRB
#define OC1B_PIN    PB2

//------------OC0A------------------------
#define OC0A_DDR    DDRD
#define OC0A_PIN    PD6

//------------SPI-------------------------
#define MOSI_PORT   PORTB
#define MOSI_DDR    DDRB
#define MOSI_PIN    PB3   

#define MISO_PORT   PORTB
#define MISO_DDR    DDRB
#define MISO_PIN    PB4   

#define SCK_PORT    PORTB
#define SCK_DDR     DDRB
#define SCK_PIN     PB5   

// Hardware SS Should be output for Master mode
#define SS_PORT     PORTB
#define SS_DDR      DDRB
#define SS_PIN      PB2   

// Potentiometer CS
#define POT_SS_PORT  PORTD
#define POT_SS_DDR   DDRD
#define POT_SS_PIN   PD7

#define SELECT_POT   cbi(POT_SS_PORT, POT_SS_PIN);
#define UNSELECT_POT sbi(POT_SS_PORT, POT_SS_PIN);  

//------------ADC-------------------------
#define AIN_U_CHANNEL   6
#define AIN_I_CHANNEL   7
#define REF_1_1_CHANNEL	14
#define AIN_DOT_CHANNEL 2

#define DIDR_AIN_U      ADC4D		// просто чтобы не удалять, так как 6 и 7 нету
#define DIDR_AIN_I      ADC5D

//------------FREQ_DOT--------------------

#define FREQ_PORT    PORTB   
#define FREQ_DDR     DDRB
#define FREQ_PIN     PB0

#define FREQ_CONTROL_PORT 	PORTC
#define FREQ_CONTROL_PIN_S0	4
#define FREQ_CONTROL_PIN_S1	5
#define FREQ_CONTROL_DDR 	DDRC

//------------ RESET ----------------------

#define RESET_PORT      PORTC
#define RESET_DDR       DDRC
#define RESET_READ_PIN  PINC
#define RESET_PIN       PC2

//-------------RESERV----------------------
#define RESERV_PORT     PORTD
#define RESERV_DDR      DDRD
#define RESERV_PIN      PD5

#define RESERV_INIT    cbi(RESERV_PORT, RESERV_PIN); sbi(RESERV_DDR, RESERV_PIN);

#define RESERV_ON      sbi(RESERV_PORT, RESERV_PIN);
#define RESERV_OFF     cbi(RESERV_PORT, RESERV_PIN);

//Выбор порта микроконтроллера при помощи которого формируется сигнал "Сигнал ДОТ"
#define FREQ_OUT_DDR	DDRD
#define FREQ_OUT_PORT	PORTD
#define FREQ_OUT_PIN	PD3                  




