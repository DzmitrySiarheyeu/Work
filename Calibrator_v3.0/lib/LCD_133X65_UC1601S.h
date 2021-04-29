#ifndef __LCD_133X65_H__
#define __LCD_133X65_H__

#include <stdint.h>
#include <stdbool.h>
#include "i2c.h"

/* @note: characters '#' and '$' are replaced with 
 * battery and power icons in character table.
 */
#define BATTERY_CHAR          '#'
#define EXT_PWR_CHAR          '$'

#define LCD_ADDR              (0x70 >> 1)
#define LCD_ADDR_CMD          (((uint8_t) (LCD_ADDR | 0x00)) << 1)
#define LCD_ADDR_DATA         (((uint8_t) (LCD_ADDR | 0x01)) << 1)
/************************Операции*************************/

#define     UC1601S_RESET       0xE2   //  Программный сброс
#define     UC1601S_BIAS_RATIO  0xE8   //  Контрастность
#define     UC1601S_VBIAS       0x81   //  Потенциометр Vbias (для установки контрастности)
#define     UC1601S_CTRL        0xC0   //  Управление отображением
#define     UC1601S_DISABLE     0xAE   //  
#define     UC1601S_ENABLE      0xAF   //  Включение дисплея 
#define     UC1601S_CLMN_LSB    0x0F   //  Установить адрес столбца (младшие 4 бита)
#define     UC1601S_CLMN_MSB    0x10   //  Установить адрес столбца (старшие 4 бита)
#define     UC1601S_ROW         0xB0   //  Установить адрес страницы
#define     UC1601S_INVERSE     0xA6


void LCD_init();

void LCD_disable();

void LCD_open();

void LCD_close();

void LCD_putchar(uint8_t, bool);

void LCD_put_hz(uint32_t n, uint8_t dec, uint8_t row, uint8_t column);

void LCD_put_volts(uint32_t n, uint8_t dec, uint8_t row, uint8_t column);

/**
 * Render integer in 11x15 font.
 * @param n         integer
 * @param dec       delimiter position
 * @param min_sz    minimum number of digits to display (including zeros)
 * @param row       row
 * @param column    column
 * @param align_right justify text to the right
 * @return 
 */
uint8_t LCD_put_large_int(uint32_t n, uint8_t dec, uint8_t min_sz,
        uint8_t row, uint8_t column, bool align_right);

void LCD_set_inverse_mode(uint8_t mode);

void LCD_goto(uint8_t row, uint8_t col);

void LCD_clear(void);

#endif /* __LCD_133X65_H__ */
