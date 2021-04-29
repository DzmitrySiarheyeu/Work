#ifndef CONFIG_H
#define CONFIG_H

#include <avr/io.h>

#if defined (CONFIG_PROTO)
#include "config_proto.h"
#else
/* DC-DC converter */
#define BOOST_PORT          PORTB
#define BOOST_DR            DDRB
#define BOOST_PIN           PB1

/* Output pull-up */
#define PULLUP_PORT         PORTC
#define PULLUP_DR           DDRC
#define PULLUP_PIN          PC0

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
    ADC_VIN_CHANNEL = 7,
    ADC_12V_CHANNEL = 1,
    ADC_SHUNT_CHANNEL = 6,

    ADC_CHANNELS_COUNT
};
#endif

/* Inline functions */
static inline void boost_enable() {
    BOOST_PORT |= (1 << BOOST_PIN);
}

static inline void boost_disable() {
    BOOST_PORT &= ~(1 << BOOST_PIN);
}

static inline void pullup_disable() {
    PULLUP_PORT |= (1 << PULLUP_PIN);
}

static inline void pullup_enable() {
    PULLUP_PORT &= ~(1 << PULLUP_PIN);
}

static inline void rs485_tx_enable() {
    RS485_RE_PORT |= (1 << RS485_RE_PIN);
    RS485_DE_PORT |= (1 << RS485_DE_PIN);
}

static inline void rs485_rx_enable() {
    RS485_RE_PORT &= ~(1 << RS485_RE_PIN);
    RS485_DE_PORT &= ~(1 << RS485_DE_PIN);
}

static inline void rs485_disable() {
    RS485_RE_PORT |= (1 << RS485_RE_PIN);
    RS485_DE_PORT &= ~(1 << RS485_DE_PIN);
}

static inline void rs232_enable() {
    RS232_FF_PORT |= (1 << RS232_FF_PIN);
    RS232_EN_PORT &= ~(1 << RS232_EN_PIN);
}

static inline void rs232_disable() {
    RS232_FF_PORT &= ~(1 << RS232_FF_PIN);
    RS232_EN_PORT |= (1 << RS232_EN_PIN);
}

static inline void GPIO_init() {
    /* Reset ports to default state
     * in case bootloader messed things up */
    DDRB = DDRC = DDRD = 0x00;
    PORTB = PORTC = PORTD = 0x00;
    /* Buzzer */
    DDRD |= (1 << PD6);
    /* DC-DC converter EN */
    BOOST_DR |= (1 << BOOST_PIN);
    /* Pull-up */
    PULLUP_DR |= (1 << PULLUP_PIN);
    /* Interfaces */
    RS232_EN_DR |= (1 << RS232_EN_PIN);
    RS232_FF_DR |= (1 << RS232_FF_PIN);
    RS485_RE_DR |= (1 << RS485_RE_PIN);
    RS485_DE_DR |= (1 << RS485_DE_PIN);
}

#endif /* CONFIG_H */
