#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "i2c.h"

#define I2C_TIMEOUT 100000U

static void i2c_busy_wait() {
    uint32_t timeout = 0;
    wdt_reset();
    while (!(TWCR & (1 << TWINT)) && TWSR != 0) {
        if (timeout++ >= I2C_TIMEOUT) {
            i2c_disable();
            i2c_init();
            return;
        }
    }
}

void i2c_init() {
    /*
     * scl_freq = f_cpu/(16 + 2*TWBR*prescaler)
     * f_cpu/scl_freq - 16 = 2*twbr*presc
     * twbr = (F_CPU/SCL - 16)/(2*prescaler)
     */
    TWSR = 0x00; // prescaler = 0
    TWBR = (F_CPU / F_SCL - 16) / 2;
    TWCR = (1 << TWEN);
}

void i2c_disable() {
    TWCR &= ~((1 << TWSTO) | (1 << TWEN));
}

void i2c_start() {
    TWCR = ((1 << TWINT) | (1 << TWSTA) | (1 << TWEN));
    i2c_busy_wait();
}

void i2c_stop() {
    TWCR = ((1 << TWINT) | (1 << TWEN) | (1 << TWSTO));
    _delay_us(10); //wait for a short time
}

void i2c_write(uint8_t word) {
    TWDR = word;
    TWCR = ((1 << TWINT) | (1 << TWEN));
    i2c_busy_wait();
}

unsigned char i2c_read(uint8_t ack) {
    TWCR = ((1 << TWINT) | (1 << TWEN) | (ack << TWEA));
    i2c_busy_wait();
    return TWDR;
}
