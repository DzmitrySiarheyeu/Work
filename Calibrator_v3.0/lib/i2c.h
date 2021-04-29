#ifndef I2C_H
#define I2C_H

#define F_SCL 400000UL

#define I2C_READ 0x01
#define I2C_WRITE 0x00

void i2c_init();

void i2c_disable();

void i2c_start();

void i2c_stop();

/**
 * Send one byte over i2c.
 * I2C_READ or I2C_WRITE should be added accordingly
 *
 * @param word 8bit data to be sent.
 */
void i2c_write(uint8_t word);

unsigned char i2c_read(uint8_t ack);

#endif /* I2C_H */
