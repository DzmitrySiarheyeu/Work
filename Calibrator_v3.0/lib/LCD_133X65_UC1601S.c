#include "LCD_133X65_UC1601S.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "Font.h"
#include "i2c.h"

void LCD_init(void);
void LCD_data_send(uint8_t data);
void LCD_data_send_buf(uint8_t *data, uint16_t count);
void LCD_goto(uint8_t row, uint8_t col);
void LCD_clear(void);

static void LCD_set_row(uint8_t Row);
static void LCD_set_column(uint8_t Column);

static int _LCD_putchar(char c, FILE *stream);
static FILE LCD_stdout = FDEV_SETUP_STREAM(_LCD_putchar, NULL, _FDEV_SETUP_WRITE);

static void LCD_write_reg(uint8_t reg) {
    i2c_start();
    i2c_write(LCD_ADDR_CMD);
    i2c_write(reg);
    i2c_stop();
}

void LCD_disable() {
    LCD_write_reg(UC1601S_DISABLE);
}

static uint8_t LCD_get_char_index(uint8_t ch) {
    uint8_t index = 0;

    //    //обрабытываем символ перехода на новую строку
    //    if (ch == 0x0A) {
    //        LCD_setXY(0, CurrentPos.Y + 8);
    //        return;
    //    }
    //
    //    //обрабытываем символ перевода каретки
    //    if (ch == 0x0D) {
    //        LCD_setXY(2, CurrentPos.Y + 9);
    //        return;
    //    }

    //символы и латинские буквы начинаются с позиции 0 в массиве
    //поэтомы позиция = код - 0x20
    if ((int) ch >= ' ' && (int) ch <= '~') {
        index = ch - ' ';
    }//русские буквы начинаются с позиции 95 в массиве, код "А" - 192,
        //поэтому позиция = код - 192 + 95
    else if ((int) ch >= (0x000000FF & 'А') && (int) ch <= (0x000000FF & 'я')) {
        index = ch - 'А' + 95;
    }
    return index;
}

/* Used for printf */
static int _LCD_putchar(char c, FILE *stream) {
    LCD_putchar(c, false);
    return 0;
}

void LCD_putchar(uint8_t ch, bool inverse) {
    uint8_t index = LCD_get_char_index(ch);

    LCD_open();

    for (uint8_t i = 0; i < 5; i++) {
        uint8_t buffer = pgm_read_word(&(font[index * 5 + i]));
        i2c_write(inverse ? ~buffer : buffer);
    }

    i2c_write(inverse ? ~0x00 : 0x00);

    LCD_close();
}

void LCD_open() {
    i2c_start();
    i2c_write(LCD_ADDR_DATA);
}

void LCD_close() {
    i2c_stop();
}

static void LCD_put_large_digit(uint8_t d, uint8_t row, uint8_t column) {
    LCD_goto(row, column);

    const uint8_t *p = &font11x15[d * 11 * 2];
    const uint8_t *end = p + 11;

    LCD_open();
    while (p < end)
        i2c_write(pgm_read_word(p++));
    LCD_close();

    LCD_goto(row + 1, column);
    end += 11;

    LCD_open();

    while (p < end)
        i2c_write(pgm_read_word(p++));
    LCD_close();
}

void LCD_put_hz(uint32_t n, uint8_t dec, uint8_t row, uint8_t column) {
    uint8_t pos = LCD_put_large_int(n, dec, 1, row, column, true);
    /* Place 'Hz' */
    LCD_put_large_digit(11, row, pos += 11);
    LCD_put_large_digit(12, row, pos += 11);
}

void LCD_put_volts(uint32_t n, uint8_t dec, uint8_t row, uint8_t column) {
    uint8_t pos = LCD_put_large_int(n, dec, 3, row, column, true);
    /* Place 'V' */
    LCD_put_large_digit(13, row, pos += 11);
}

uint8_t LCD_put_large_int(uint32_t n, uint8_t dec, uint8_t min_sz, uint8_t row, uint8_t column, bool align_right) {
    int8_t idx = 0;

    uint8_t buf[6] = {0};

    if (n == 0) idx = dec + 1;

    /* Extract digits (slow) */
    while (n > 0) {
        buf[idx++] = n % 10;
        n /= 10;
    };

    while (idx < min_sz)
        buf[idx++] = 0;

    /* Align text */
    uint8_t pos = column;
    pos += (align_right) ? (6 - idx)*11 : 0;

    /* @TODO: this should be a separate function */
    LCD_goto(row, 0);
    LCD_open();
    for (uint8_t i = 0; i < pos; i++)
        i2c_write(0x00);
    LCD_close();

    LCD_goto(row + 1, 0);
    LCD_open();
    for (uint8_t i = 0; i < pos; i++)
        i2c_write(0x00);
    LCD_close();
    /*********************************************/

    /* Place all digits preceding delimiter */
    while (idx-- > dec) {
        LCD_put_large_digit(buf[idx], row, pos);
        pos += 11;
    }

    /* Place delimiter */
    if (dec) LCD_put_large_digit(10, row, pos);

    /* Place remaining digits */
    while (idx >= 0)
        LCD_put_large_digit(buf[idx--], row, pos += 11);

    return pos;
}

void LCD_init(void) {
    _delay_ms(10);
    LCD_write_reg(UC1601S_RESET);
    _delay_ms(10);

    i2c_start();
    i2c_write(LCD_ADDR_CMD);
    i2c_write(UC1601S_BIAS_RATIO | 3);
    i2c_write(UC1601S_VBIAS);
    i2c_write(110);
    i2c_write(0b11000110); // top-bottom, left-right
    i2c_write(UC1601S_ENABLE);
    i2c_stop();

    LCD_clear();

    stdout = &LCD_stdout;
}

void LCD_set_inverse_mode(uint8_t mode) {
    LCD_write_reg(UC1601S_INVERSE + mode);
}

static void LCD_set_column(uint8_t Column) {
    i2c_start();
    i2c_write(LCD_ADDR_CMD);
    i2c_write(UC1601S_CLMN_LSB & Column);
    i2c_write((Column >> 4) | UC1601S_CLMN_MSB);
    i2c_stop();
}

static void LCD_set_row(uint8_t Row) {
    LCD_write_reg(UC1601S_ROW | Row);
}

void LCD_goto(uint8_t row, uint8_t col) {
    LCD_set_row(row);
    LCD_set_column(col);
}

void LCD_clear(void) {
    i2c_start();
    i2c_write(LCD_ADDR_CMD);
    /* Page 0 */
    i2c_write(0b10110000);
    /* Column 0 */
    i2c_write(0b00000000);
    i2c_write(0b00010000);
    i2c_stop();

    LCD_open();

    for (uint16_t i = 1056; i > 0; i--)
        i2c_write(0x00);

    LCD_close();
}
