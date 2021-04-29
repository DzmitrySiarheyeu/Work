#include "ADC.h"
#include "config.h"
#include <avr/io.h>
#include <stddef.h>

static enum ADC_CHANNEL adc_channel;

static struct ADC_result *result;

static void ADC_start_conversion(uint8_t channel);

static inline uint8_t ADC_is_busy() {
    return (ADCSRA & (1 << ADSC));
}

uint8_t ADC_ready() {
    uint8_t ready = 0;
    if (!ADC_is_busy() && result != NULL) {
        switch (adc_channel) {
            case (ADC_VIN_CHANNEL):
                result->channel_vin = ADC;
                adc_channel = ADC_12V_CHANNEL;
                break;
            case (ADC_12V_CHANNEL):
                result->channel_12v = ADC;
                adc_channel = ADC_SHUNT_CHANNEL;
                break;
            case (ADC_SHUNT_CHANNEL):
                result->channel_shunt = ADC;
                adc_channel = ADC_VIN_CHANNEL;
                ready = 1;
                break;
            default:
                adc_channel = ADC_VIN_CHANNEL;
                break;
        }
        ADC_start_conversion(adc_channel);
    }
    return ready;
}

void ADC_enable(struct ADC_result *res) {
    result = res;
    adc_channel = 0;
    ADMUX = 0x00; // AREF, internal Vref turned off
    ADCSRA = (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2) | // Division Factor = 16
            (1 << ADEN); // Enable ADC
}

static void ADC_start_conversion(uint8_t channel) {
    if (channel > 7) return;
    /* Clear last 3 bits and set ADC channel */
    ADMUX = (ADMUX & 0xF8) | channel;
    /* Start conversion */
    ADCSRA |= (1 << ADSC);
}

uint16_t ADC_convert_sensor_voltage(uint16_t val) {
    //double d = (double) V_REF * (val / ADC_RESOLUTION) * 100 * V_DIVIDER;
    return ((double) (V_REF / ADC_RESOLUTION) * V_IN_DIVIDER * val * 100);
}

uint16_t ADC_convert_external_voltage(uint16_t val) {
    return ((double) (V_REF / ADC_RESOLUTION) * V_EXT_DIVIDER * val);
}

uint16_t ADC_convert_supply_current(uint16_t val) {
    return ((double) (val * V_REF * 10 / ADC_RESOLUTION) / (SHUNT_RESISTOR) + 0.5);
}

uint16_t ADC_read(uint8_t channel) {
    ADC_start_conversion(channel);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

void ADC_disable() {
    result = NULL;
    ADMUX &= 0xF8;
    ADCSRA = 0x00;
}
