#ifndef FREQ_H
#define FREQ_H

#include <stdint.h>

/**
 * Measure frequency on ICP1.
 * Timer1 is used for capture
 */
void frequency_measurement_start();

void frequency_measurement_stop();

/**
 * 
 * @return frequency multiplied by 10.
 * 0xFFFF - overflow, 0 - no signal
 */
uint16_t get_frequency();

/**
 * Output 50% duty cycle square wave on OC1B
 * 
 * @param frequency - value in Hz
 */
void PWM_start(uint16_t frequency);

void PWM_stop();


/**
 * Output 50% duty cycle square wave on PWM_OUT_PIN
 * by software pin toggling
 * 
 * @param frequency - value in Hz
 */
void soft_PWM_start(uint16_t frequency);

void soft_PWM_stop();

#endif /* FREQ_H */
