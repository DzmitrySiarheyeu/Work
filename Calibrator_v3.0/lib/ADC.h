#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include "config.h"

#if defined (CONFIG_PROTO)
#include "config_proto.h"
#else
#define V_REF                   2.048
#define ADC_RESOLUTION          1024
#define SHUNT_RESISTOR          0.32
#define V_IN_DIVIDER            ((40.2 + 10) / 10)   // (R10 + R13)/R13
#define V_EXT_DIVIDER           ((40.2 + 2.1) / 2.1) // (R16 + R17)/R17
#define EXT_VOLTAGE_THRESHOLD   ((uint16_t)(10 * (ADC_RESOLUTION / V_REF) / V_EXT_DIVIDER)) // 10V
#define BAT_VOLTAGE_LOW         302 //approx. 3V
#define BAT_VOLTAGE_CRITICAL    300
#endif

struct ADC_result {
    uint16_t channel_vin;
    uint16_t channel_12v;
    uint16_t channel_shunt;
};

/**
 * Initialize ADC with following setting: 
 *      Division Factor = 16, 
 *      AREF pin as voltage reference.
 * @param res structure for storing conversion results
 */
void ADC_enable(struct ADC_result *res);

void ADC_disable();

/**
 * Non-blocking conversion of 3 ADC channels. Checks if ADC is busy and starts
 * conversion of the next channels. Once all channels are sampled, results are
 * stored in ADC_result struct.
 * @return 1 - all channels were sampled;
 *         0 - conversion still in progress.
 */
uint8_t ADC_ready();

/**
 * Blocking ADC read
 * @param channel ADC channel
 * @return raw 10-bit value
 */
uint16_t ADC_read(enum ADC_CHANNEL channel);

/**
 * @return sensor voltage multiplied by 100
 * @param val raw ADC reading
 */
uint16_t ADC_convert_sensor_voltage(uint16_t val);

uint16_t ADC_convert_external_voltage(uint16_t val);

/**
 * @return supplied current in mA.
 */
uint16_t ADC_convert_supply_current(uint16_t val);

#endif /* ADC_H */
