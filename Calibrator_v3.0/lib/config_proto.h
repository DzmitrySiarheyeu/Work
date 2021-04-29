/* 
 * File:   config_proto.h
 * Description: configuration used for early prototype boards.
 * Should NOT be used in production.
 *
 * Created on July 22, 2016, 11:52 AM
 */
#ifndef CONFIG_PROTO_H
#define CONFIG_PROTO_H

/* DC-DC converter */
#define BOOST_PORT          PORTB
#define BOOST_DR            DDRB
#define BOOST_PIN           PB1

/* Output pull-up */
#define PULLUP_PORT         PORTD
#define PULLUP_DR           DDRD
#define PULLUP_PIN          PD7

/* Interfaces */
#define RS232_EN_PIN        PD2
#define RS232_EN_DR         DDRD
#define RS232_EN_PORT       PORTD

#define RS232_FF_PIN        PD4
#define RS232_FF_DR         DDRD
#define RS232_FF_PORT       PORTD

#define RS485_RE_PIN        PC2
#define RS485_RE_DR         DDRC
#define RS485_RE_PORT       PORTC

#define RS485_DE_PIN        PC3
#define RS485_DE_DR         DDRC
#define RS485_DE_PORT       PORTC

/* Software PWM */
#define PWM_OUT_PORT        PORTB
#define PWM_OUT_DR          DDRB
#define PWM_OUT_PIN         PB2
#define PWM_OUT_PIN_REG     PINB

/* ADC channels */
enum ADC_CHANNEL {
    ADC_VIN_CHANNEL = 0,
    ADC_12V_CHANNEL = 1,
    ADC_SHUNT_CHANNEL = 7,

    ADC_CHANNELS_COUNT
};

/* ADC */
#define V_REF                   2.048
#define ADC_RESOLUTION          1024
#define SHUNT_RESISTOR          0.5
#define V_IN_DIVIDER            ((40.2 + 10)/10)
#define V_EXT_DIVIDER           ((40.2 + 2.1)/2.1)
#define EXT_VOLTAGE_THRESHOLD   248 //approx. 10V
#define BAT_VOLTAGE_LOW         302 //approx. 3V
#define BAT_VOLTAGE_CRITICAL    300

#endif /* CONFIG_PROTO_H */
