#include "freq.h"
#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define MAX_FREQ_COUNTS 64 //def:250

volatile uint16_t result_val;
volatile uint16_t result_timer_ovf_count;

volatile uint8_t timer_ovf_count;
volatile uint8_t capture_count;

void PWM_start(uint16_t frequency) {
    PWM_OUT_DR |= 1 << PWM_OUT_PIN;
    TCCR1A = (1 << COM1B0);
    TCCR1B = (1 << WGM12) | (1 << CS10);
    OCR1A = F_CPU / (frequency * 2) - 1;
}

void PWM_stop() {
    PWM_OUT_PORT &= ~(1 << PWM_OUT_PIN);
    TCCR1A = 0;
    TCCR1B = 0;
}

void soft_PWM_start(uint16_t frequency) {
    PWM_OUT_DR |= 1 << PWM_OUT_PIN;
    /* CTC mode */
    TCCR2A = (1 << WGM21);
    /* Prescaler = 128 */
    TCCR2B = (1 << CS22) | (1 << CS20);
    /* Timer2 CompareA interrupt enable */
    TIMSK2 |= 1 << OCIE2A;
    /* Preload for given frequency */
    OCR2A = (F_CPU / (2 * 128 * (uint32_t) frequency)) - 1;
}

void soft_PWM_stop() {
    //PWM_OUT_DR |= 1 << PWM_OUT_PIN;
    PWM_OUT_PORT &= ~(1 << PWM_OUT_PIN);
    TCCR2A = 0;
    TCCR2B = 0;
    TIMSK2 &= ~(1 << OCIE2A);
}

ISR(TIMER2_COMPA_vect, ISR_NAKED) {
    /* Toggle output pin */
    PWM_OUT_PIN_REG |= 1 << PWM_OUT_PIN;
    reti();
}

void frequency_measurement_start() {
    TCCR1A = 0;
    TCCR1B = 0;

    timer_ovf_count = 0;
    capture_count = 0;

    /* Prescaler = 1, capture on rising edge */
    TCCR1B = (1 << CS10) | (1 << ICES1);
    /* Input capture noise cancel */
    TCCR1B |= (1 << ICNC1);
    /* Enable input capture and timer overflow interrupts */
    TIMSK1 |= (1 << ICIE1) | (1 << TOIE1);
}

void frequency_measurement_stop() {
    TCCR1A = 0;
    TCCR1B = 0;
    TIMSK1 &= (1 << ICIE1) | (1 << TOIE1);
}

uint16_t get_frequency() {
    uint32_t result = 0;

    /* @note: \result_timer_ovf_count and \result_val 
     * should be accessed atomically
     */
    if (result_timer_ovf_count > 0) {
        result = (result_timer_ovf_count * 65536UL + result_val) / (MAX_FREQ_COUNTS + 1); // why +1??
    } else {
        result = result_val / (MAX_FREQ_COUNTS + 1);
    }
    result = F_CPU * 10 / result;
    return result;
}

ISR(TIMER1_CAPT_vect) {
    uint16_t val = ICR1;

    if (capture_count++ >= MAX_FREQ_COUNTS) {
        result_val = val;
        result_timer_ovf_count = timer_ovf_count;
        timer_ovf_count = 0;
        capture_count = 0;
        TCNT1 = 0; // restart timer
    }
}

ISR(TIMER1_OVF_vect) {
    timer_ovf_count++;
    if (timer_ovf_count > 254) {
        result_timer_ovf_count = 0;
        result_val = 0;
    }
}
