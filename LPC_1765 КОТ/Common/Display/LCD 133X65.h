#ifndef __LCD_133X65_H__
#define __LCD_133X65_H__

#include <stdint.h>

#define LCD_WRITE_CONFIG     0x00
#define LCD_WRITE_RAM        0x40

#define LCD_ADDR             (0x78 >> 1) //����� �������
#define LCD_JUMP_TO_H_111    0x01 //�� �������� �������� 111
#define LCD_NOP              0x00 //��� ��������

/*********************�������� �������� 111*****************************/

#define LCD_JUMP_TO_H_110    0x0E //�� �������� �������� 110
#define LCD_JUMP_TO_H_101    0x0D //�� �������� �������� 101  
#define LCD_JUMP_TO_H_011    0x0B //�� �������� �������� 011

#define LCD_PD               0x04 //Power-down mode
#define LCD_V                0x02 //��������� LCD
#define LCD_FUNCTION_SET     0x10 //����� �� Power-down mode, ��������� - ������������
                                  //������������ � ����: (LCD_FUNCTION_SET | LCD_PD | LCD_V)

#define LCD_RAM_PAGE         0x20 //��� ��������� �� � > 127
#define LCD_SET_RAM_PAGE     0x04 //������������ � ����: (LCD_RAM_PAGE | LCD_SET_RAM_PAGE)
#define LCD_SET_Y_ADDR       0x40 //����� �� 0<= Y <=8 (������: LCD_SET_Y_ADDR | "�����")
#define LCD_SET_X_ADDR       0x80 //����� �� 0<= X <=132 (������: LCD_SET_X_ADDR | "�����")

/*********************�������� �������� 110******************************/

#define LCD_E                0x01 
#define LCD_D                0x02 //������ � LCD_E ���������� ����� �����������
                                  //DE : 00 -> �������� ��� �������
                                  //     10 -> ���������� �����
                                  //     01 -> ������ ��� �������
                                  //     11 -> ��������� �����
#define LCD_DISPLAY_CONTROL  0x04 ////������������ � ����: (LCD_DISPLAY_CONTROL | LCD_D | LCD_E) 

#define LCD_MIRROR_X         0x04 //���������� ��������� �� �
#define LCD_MIRROR_Y         0x02 //���������� ��������� �� Y
#define LCD_EXT_DISP_CONTROL 0x08 //������������ � ����: (LCD_EXT_DISP_CONTROL | LCD_MIRROR_X | LCD_MIRROR_Y)

#define LCD_BS0              0x01 
#define LCD_BS1              0x02
#define LCD_BS2              0x03 //���������� �������� ����������
#define LCD_BIAS_SYSTEM      0x10 //������������ � ����: (LCD_BIAS_SYSTEM | LCD_BS0 | LCD_BS1 | LCD_BS2)

#define LCD_IB               0x04 //�������� ���� (��� ������� �������� ������� � 1)
#define LCD_DISPLAY_SIZE     0x20 //������������ � ����: (LCD_DISPLAY_SIZE | LCD_IB)

#define LCD_M0               0x01
#define LCD_M1               0x02
#define LCD_M2               0x04 //���������� ������ �������
#define LCD_MULTIPLEX_RATE   0x80 //������������ � ����: (LCD_MULTIPLEX_RATE | LCD_M0 | LCD_M1 | LCD_M2)

/*********************�������� �������� 101******************************/

#define LCD_HVE              0x01 //�������� ���������� ����������
#define LCD_PRS              0x02 //Programming range HIGH
#define LCD_HV_GEN_CONTROL   0x04 //������������ � ����: (LCD_HV_GEN_CONTROL | LCD_HVE | LCD_PRS)

#define LCD_S0               0x01 
#define LCD_S1               0x02 //���������� ����������� ��������� ����������
#define LCD_HV_GEN_STAGES    0x08 //������������ � ����: (LCD_HV_GEN_STAGES | LCD_S0 | LCD_S1)

#define LCD_TC0              0x01 
#define LCD_TC1              0x02 
#define LCD_TC2              0x03 //���������� ������������� �����������
#define LCD_TEMP_COEF        0x10 //������������ � ����: (LCD_TEMP_COEF | LCD_TC0 | LCD_TC1 | LCD_TC2)

#define LCD_START_TEMP_MEASURE 0x21 //������ ��������� �����������

#define LCD_Vop0             0x01
#define LCD_Vop1             0x02
#define LCD_Vop2             0x04
#define LCD_Vop3             0x08
#define LCD_Vop4             0x10
#define LCD_Vop5             0x20
#define LCD_Vop6             0x40 //���������� ������������

#define LCD_VLCD_CONTROL     0x80 //������������ � ����: (LCD_VLCD_CONTROL | LCD_Vop0 | LCD_Vop1 | ....)

/*********************�������� �������� 011******************************/

#define LCD_DM               0x01 //Direct mode
#define LCD_DOF              0x02 //Display off mode
#define LCD_STATE_CONTROL    0x04 //������������ � ����: (LCD_STATE_CONTROL | LCD_DM | LCD_DOF)

#define LCD_EC               0x02 //������� ���������� �������� ���������
#define LCD_OSC_SETTING      0x04 //������������ � ����: (LCD_OSC_SETTING | LCD_EC)

#define LCD_BRS              0x08 
#define LCD_TRS              0x10 //��������� ������� ���������� �����
#define LCD_COG_TCP          0x40 //������������ � ����: (LCD_COG_TCP | LCD_TRS | LCD_BRS)
                                  //������ �������� 0x58 ��� ���������� ������

typedef uint8_t tBoolean;
#define false 0
#define true  1

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
extern void LCD_draw_full_screen(uint8_t *buff);


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
extern tBoolean LCD_test_pixel(uint8_t X, uint8_t Y);


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
extern void LCD_fill_area(uint8_t X, uint8_t Y);

/*������ ��������������� �����*/
void invert_text(void);

/*������ ����������������� �����*/
void direct_text(void);

#endif
