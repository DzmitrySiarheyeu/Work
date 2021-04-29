#ifndef __LCD_133X65_H__
#define __LCD_133X65_H__

#include <stdint.h>

#define LCD_WRITE_COMMAND     0x00
#define LCD_WRITE_DATA        0x01

#define LCD_ADDR             (0x70 >> 1) //����� �������

/************************��������*************************/

#define		UC1601S_RESET		0xE2   //  ����������� �����
#define		UC1601S_BIAS_RATIO	0xE8   //  �������������
#define		UC1601S_VBIAS		0x81   //  ������������ Vbias (��� ��������� �������������)
#define		UC1601S_CTRL		0xC0   //  ���������� ������������
#define		UC1601S_ENABLE		0xAF   //  ��������� ������� 
#define		UC1601S_CLMN_LSB	0x0F   //  ���������� ����� ������� (������� 4 ����)
#define		UC1601S_CLMN_MSB	0x10   //  ���������� ����� ������� (������� 4 ����)
#define		UC1601S_ROW			0xB0   //  ���������� ����� ��������


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
��������� �������
*/
extern void LCD_init(void);

 
/*
������������� ������ �� ������� �Y, ���
0 <= X <= 132
0 <= Y <= 64
������������ ����� �������� printf
*/
extern void LCD_setXY(uint8_t X, uint8_t Y);


/*
������������ ��� ��������� �������� �������
*/
extern void LCD_config_send(uint8_t data);


/*
������������ ��� ������ ������
*/
extern void LCD_clear(void);


/*
�������� ����� ������
*/
extern void LCD_buff_clear(void);


/*
�������� ������������� ������� ������, ���
X1Y1 - ���������� �������� ������ ���� �������
X2Y2 - ���������� ������� ������� ���� �������
*/
extern void LCD_erase_area(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2);


/*
���������� �������������
X1Y1 - ���������� �������� ������ ���� ��������������
X2Y2 - ���������� ������� ������� ���� ��������������
*/
extern void LCD_draw_rectangle(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2);


/*
���������� ��������, ���������� ���� �����
buf - ���������� ������, ���������� ��������
*/
//extern void LCD_draw_full_screen(uint8_t *buff);


/*
���������� �����
X1Y1 - ���������� ������ �����
X2Y2 - ���������� ����� �����
!!!��������!!! �1 ������ ���� ������ �2
*/
extern void LCD_draw_line(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2);


/*
���������� ��������, ���������� ����� �����
XY - ���������� �������� ������ ���� ��������������
dim_X - ������ ����������� � �������� �� X
dim_Y - ������ ����������� � �������� �� Y
image - ���������� ������, ���������� ��������
size - ������ �������
*/
extern void LCD_draw_image(uint8_t X, uint8_t Y, uint8_t dim_X, uint8_t dim_Y, const uint8_t *image, uint16_t size);
 

/*
��������� ����� �� ������
XY - ���������� �����
*/
extern void LCD_put_pixel(uint8_t X, uint8_t Y);


/*
XY - ���������� �����
���������� true, ���� ����� ���������, false - � ��������� ������
*/
//extern tBoolean LCD_test_pixel(uint8_t X, uint8_t Y);


/*
���������� ����������
XY - ���������� ������ ����������
R - ������ ����������
*/
//extern void LCD_draw_circle(uint8_t X, uint8_t Y, uint8_t R);


/*
��������� ��������� ������� ������������ ����� (������� ������ ����� ��������� ��������� ��������� �������)
XY - ����� ������ �������
*/
//extern void LCD_fill_area(uint8_t X, uint8_t Y);

/*������ ��������������� �����*/
void invert_text(void);

/*������ ����������������� �����*/
void direct_text(void);

#endif
