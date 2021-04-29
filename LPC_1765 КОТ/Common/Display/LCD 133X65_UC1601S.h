#ifndef __LCD_133X65_H__
#define __LCD_133X65_H__

#include <stdint.h>

#define LCD_WRITE_COMMAND     0x00
#define LCD_WRITE_DATA        0x01

#define LCD_ADDR             (0x70 >> 1) //Адрес дисплея

/************************Операции*************************/

#define		UC1601S_RESET		0xE2   //  Программный сброс
#define		UC1601S_BIAS_RATIO	0xE8   //  Контрастность
#define		UC1601S_VBIAS		0x81   //  Потенциометр Vbias (для установки контрастности)
#define		UC1601S_CTRL		0xC0   //  Управление отображением
#define		UC1601S_ENABLE		0xAF   //  Включение дисплея 
#define		UC1601S_CLMN_LSB	0x0F   //  Установить адрес столбца (младшие 4 бита)
#define		UC1601S_CLMN_MSB	0x10   //  Установить адрес столбца (старшие 4 бита)
#define		UC1601S_ROW			0xB0   //  Установить адрес страницы


typedef uint8_t tBoolean;

typedef union tagBuffer
{
    uint8_t LCD_Screen_Buff1[8][133];
    uint8_t LCD_Screen_Buff2[1064];
}Buffer;

extern Buffer *LCD_buffer;

typedef union tagSymbol
{
    uint16_t temp1[6];
    uint8_t temp2[12];
}Symbol;

typedef struct tagSYMBOL
{
    Symbol Sym;
    uint8_t Row;
    uint8_t Shift;
}SYMBOL;

typedef struct tagPosition
{
    uint8_t X;
    uint8_t Y;
}Position;

typedef struct tagStack
{
    Position StackXY[20];
    uint8_t Top;
}STACK;

/*
Настройка дисплея
*/
extern void LCD_init(void);

 
/*
Устанавливает курсор на позицию ХY, где
0 <= X <= 132
0 <= Y <= 64
Используется перед функцией printf
*/
extern void LCD_setXY(uint8_t X, uint8_t Y);


/*
Используется для изменения настроек дисплея
*/
extern void LCD_config_send(uint8_t data);


/*
Используется для очитки экрана
*/
extern void LCD_clear(void);


/*
Очистить буфер экрана
*/
extern void LCD_buff_clear(void);


/*
Очистить прямоугольную область экрана, где
X1Y1 - координаты верхнего левого угла области
X2Y2 - координаты нижнего правого угла области
*/
extern void LCD_erase_area(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2);


/*
Нарисовать прямоугольник
X1Y1 - координаты верхнего левого угла прямоугольника
X2Y2 - координаты нижнего правого угла прямоугольника
*/
extern void LCD_draw_rectangle(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2);


/*
Нарисовать картинку, занимающую весь экран
buf - одномерный массив, содержащий картинку
*/
//extern void LCD_draw_full_screen(uint8_t *buff);


/*
Нарисовать линию
X1Y1 - координаты начала линии
X2Y2 - координаты конца линии
!!!ВНИМАНИЕ!!! Х1 ДОЛЖЕН БЫТЬ МЕНЬШЕ Х2
*/
extern void LCD_draw_line(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2);


/*
Нарисовать картинку, занимающую часть экран
XY - координаты верхнего левого угла прямоугольника
dim_X - размер изображения в пикселях по X
dim_Y - размер изображения в пикселях по Y
image - одномерный массив, содержащий картинку
size - размер массива
*/
extern void LCD_draw_image(uint8_t X, uint8_t Y, uint8_t dim_X, uint8_t dim_Y, const uint8_t *image, uint16_t size);
 

/*
Закрасить точку на экране
XY - координаты точки
*/
extern void LCD_put_pixel(uint8_t X, uint8_t Y);


/*
XY - координаты точки
Возвращает true, если точка закрашена, false - в противном случае
*/
//extern tBoolean LCD_test_pixel(uint8_t X, uint8_t Y);


/*
Нарисовать окружность
XY - координаты центра окружности
R - радиус окружности
*/
//extern void LCD_draw_circle(uint8_t X, uint8_t Y, uint8_t R);


/*
Закрасить замкнутую область произвольной формы (границы экрана также считаются границами замкнутой области)
XY - точка внутри области
*/
//extern void LCD_fill_area(uint8_t X, uint8_t Y);

/*писать инвертированный текст*/
void invert_text(void);

/*писать неинвертированный текст*/
void direct_text(void);

#endif
