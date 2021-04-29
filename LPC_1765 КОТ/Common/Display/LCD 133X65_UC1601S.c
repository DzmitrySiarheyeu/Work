#include "config.h"

#if(_DISPLAY_)

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "Config.h"
#include "i2c_lpc17xx.h"
#include "LCD 133X65_UC1601S.h"
#include "Font.h"
#include "DriversCore.h"
#include "FreeRTOS.h"


volatile Position CurrentPos;
Buffer *LCD_buffer;
tBoolean invert_flag = false;

void LCD_init(void);
void LCD_config_send(uint8_t data);
void LCD_data_send(uint8_t data);
void LCD_data_send_buf(uint8_t *data, uint16_t count);
void LCD_setXY(uint8_t X, uint8_t Y);
void LCD_set_row(uint8_t Row);
void LCD_set_column(uint8_t Column);
void LCD_clear(void);
void LCD_buff_clear(void);
uint8_t LCD_buff_get(uint8_t column, uint8_t row);
uint8_t* LCD_buff_get_addr(uint8_t column, uint8_t row);
void LCD_buff_set(uint8_t column, uint8_t row, uint8_t data);
void LCD_put_symbol(const uint8_t *data);
void LCD_put_pixel(uint8_t X, uint8_t Y);
//tBoolean LCD_test_pixel(uint8_t X, uint8_t Y);
void LCD_draw_rectangle(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2);
//void LCD_draw_circle(uint8_t X, uint8_t Y, uint8_t R);
void LCD_draw_line(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2);
//void LCD_draw_full_screen(uint8_t *buff);
void LCD_draw_image(uint8_t X, uint8_t Y, uint8_t dim_X, uint8_t dim_Y, const uint8_t *image, uint16_t size);
//void LCD_fill_area(uint8_t X, uint8_t Y);
void LCD_erase_area(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2);
uint8_t byte_invers(uint8_t ch);
void invert_text(void) {invert_flag = true;}
void direct_text(void) {invert_flag = false;}
tBoolean is_invert(void) {return invert_flag;}


DEVICE_HANDLE hI2C;
#define openDriver() (hI2C = OpenDevice(_DISPLAY_L_L_DRIVER_NAME_))
#define closeDriver() (CloseDevice(hI2C))

void LCD_putchar(uint8_t ch)
{
    uint8_t index = 0;

    //обрабытываем символ перехода на новую строку
    if(ch == 0x0A) 
    {
        LCD_setXY(0, CurrentPos.Y + 8);
        return;
    }

	//обрабытываем символ перевода каретки
    if(ch == 0x0D) 
    {
        LCD_setXY(2, CurrentPos.Y + 9);
        return;
    }

    //первый символ в нашем алфавите - пробел (код 0x20)
    //символы до пробела мы не выводим
    if(ch < ' ') 
    {
        return;
    }
    //символы и латинские буквы начинаются с позиции 0 в массиве
    //поэтомы позиция = код - 0x20
    else if((int)ch >= ' ' && (int )ch <= '~')
    {
        index = ch - ' ';
    }
    //русские буквы начинаются с позиции 95 в массиве, код "А" - 192,
    //поэтому позиция = код - 192 + 95
    else if((int )ch >= (0x000000FF & 'А') && (int )ch <= (0x000000FF & 'я'))
    {
        index = ch - 'А' + 95;
    }

	// если символ уже не помещается в текущую строку
	if((CurrentPos.X + 6) > 131)
	{
		LCD_setXY(1, CurrentPos.Y + 8);
	}  	

	// если исмвол не влазит по вертикали
/*	if((CurrentPos.Y + 8) > 63)
	{
		LCD_setXY(CurrentPos.X, 8);
        return;
	}	*/
		
    LCD_put_symbol((const uint8_t *)suFont_OEM_6x8[index].symbol_buf); 
}

void LCD_init(void)
{
	openDriver();

	GPIO_SetValue(1, 1 << 25);		// RESET
	GPIO_SetDir(1, 1 << 25, 1);	

	vTaskDelay(20);

	LCD_config_send(UC1601S_RESET); 	// Программный RESET дисплея
	vTaskDelay(20);


	LCD_config_send(UC1601S_BIAS_RATIO | 3); 
	LCD_config_send(UC1601S_VBIAS);  
	LCD_config_send(110);			
	LCD_config_send(UC1601S_CTRL);  
	LCD_config_send(UC1601S_ENABLE);	

    LCD_buffer = (Buffer *)pvPortMalloc(sizeof(Buffer));
	closeDriver();

	LCD_clear();

}

void LCD_config_send(uint8_t data)
{
    SeekDevice(hI2C, 0xff, LCD_ADDR | LCD_WRITE_COMMAND);
    WriteDevice(hI2C, (void *)&data, 1);
}

void LCD_data_send(uint8_t data)
{
    data = byte_invers(data); 
	SeekDevice(hI2C, 0xFF, LCD_ADDR | LCD_WRITE_DATA);
    WriteDevice(hI2C, (void *)&data, 1);
}

void LCD_data_send_buf(uint8_t *data, uint16_t count)
{
    SeekDevice(hI2C, 0xFF, LCD_ADDR | LCD_WRITE_DATA);
    WriteDevice(hI2C, (void *)data, count);
}

void LCD_setXY(uint8_t X, uint8_t Y)
{
    if((X > 131) || (Y > 63)) return;

	openDriver();

    LCD_set_column(X);  
    CurrentPos.X = X;

    LCD_set_row(Y / 8); //Тут устанавливается не сам пискель, а строка бит, которой принадлежит пиксель 
    CurrentPos.Y = Y;

	closeDriver();
}

void LCD_set_column(uint8_t Column)
{
	 LCD_config_send(UC1601S_CLMN_LSB & Column);
	 LCD_config_send((Column >> 4) | UC1601S_CLMN_MSB);
}

void LCD_set_row(uint8_t Row)
{
	 LCD_config_send(UC1601S_ROW | Row);
}

void LCD_clear(void)
{
	if(LCD_buffer == 0)
		return;

	openDriver();

    LCD_buff_clear();

	LCD_config_send(UC1601S_ROW | 0); 	
	LCD_config_send(UC1601S_CLMN_LSB & 0x00);   
	LCD_config_send(UC1601S_CLMN_MSB | 0x00);	
			

    LCD_data_send_buf(LCD_buff_get_addr(0, 0), sizeof(LCD_buffer->LCD_Screen_Buff2));

	closeDriver();
}

uint8_t byte_invers(uint8_t ch)
{
	int i = 0;
	uint8_t res = 0;

	for(; i < 8; i++)
	{
		res = res << 1;
		res |= (ch & (0x01 << i)) >> i;
	}
	return res;

}

void LCD_buff_clear(void)
{
    uint16_t i = 0;
    for(i = 0;i<1063;i++) LCD_buffer->LCD_Screen_Buff2[i] = 0;
}

uint8_t LCD_buff_get(uint8_t column, uint8_t row)
{
    return byte_invers(LCD_buffer->LCD_Screen_Buff1[row][column]);
}

uint8_t* LCD_buff_get_addr(uint8_t column, uint8_t row)
{
    return &LCD_buffer->LCD_Screen_Buff1[row][column];
}

void LCD_buff_set(uint8_t column, uint8_t row, uint8_t data)
{
    if(row >= 8)
    {
        row = 7;
    }
    if(column >= 131)
    {
        column = 131;
    }
    LCD_buffer->LCD_Screen_Buff1[row][column] = byte_invers(data);
}


void LCD_put_symbol(const uint8_t *symbol)
{
    SYMBOL SYM;
    uint8_t i = 0;
    uint8_t temp;
  
    SYM.Row = CurrentPos.Y / 8; //Определяем строку, в которую пишем
    SYM.Shift = (SYM.Row+1)*8 - CurrentPos.Y - 1; //Определяем смещение относительно начала строки

    if((SYM.Shift == 0) || (SYM.Row == 0)) //Если пишем без смещения, т.е. нужно переписивать 1 строку
    {
		LCD_setXY(CurrentPos.X, CurrentPos.Y);

		openDriver();

        if(is_invert() == false)
        {
            LCD_data_send(symbol[0]);
            LCD_data_send(symbol[1]);
            LCD_data_send(symbol[2]);
            LCD_data_send(symbol[3]);
            LCD_data_send(symbol[4]);
            LCD_data_send(symbol[5]);
        }
        else
        {
            LCD_data_send(~symbol[0]);
            LCD_data_send(~symbol[1]);
	    	LCD_data_send(~symbol[2]);
            LCD_data_send(~symbol[3]);
            LCD_data_send(~symbol[4]);
            LCD_data_send(~symbol[5]);
        }
        CurrentPos.X += 6;

		closeDriver();
        return;
    }

	openDriver();

    //Пишем со смещением, т.е. нужно переписывать 2 строки
    for(i = 0 ; i < 6; i++)
    {
		if(is_invert() == false) SYM.Sym.temp1[i] = 0x00FF & symbol[i]; //Заполняем нижнюю строку
		else SYM.Sym.temp1[i] = 0x00FF & ~symbol[i];
        SYM.Sym.temp1[i] = SYM.Sym.temp1[i] << SYM.Shift; 
    }

    LCD_set_row(SYM.Row); //Переходим к текущей координате
    
    //Записываем нижнюю строку символа
    for(i = 0; i < 11; i+=2)
    {
        temp = LCD_buff_get(CurrentPos.X, SYM.Row); //Получаем текущее состояние переписываемого байта из буфера
        temp = temp & ~(0xFF << (SYM.Shift));
        LCD_data_send(SYM.Sym.temp2[i] | temp); //Выводим с учетом текущего состояния 
        LCD_buff_set(CurrentPos.X, SYM.Row, SYM.Sym.temp2[i] | temp); //Записываем обратно в буфер
        CurrentPos.X++; //Корректируем текущее положение по Х
    }

    CurrentPos.X -= 6; //Корректируем положение по Х для перехода к началу символа
    LCD_set_row(SYM.Row - 1); //Переходим к верхней строке символа
    LCD_set_column(CurrentPos.X); //Переходим к началу символа
    
    //Выводим верхнюю строку символа
    for(i = 1; i < 12; i+=2)
    {
        temp = LCD_buff_get(CurrentPos.X, SYM.Row - 1);
        temp = temp & ~(0xFF >> (7 - SYM.Shift));
        LCD_data_send(SYM.Sym.temp2[i] | temp);
        LCD_buff_set(CurrentPos.X, SYM.Row - 1, SYM.Sym.temp2[i] | temp);
        CurrentPos.X++;
    }

	closeDriver();
}

void LCD_put_pixel(uint8_t X, uint8_t Y)
{
uint8_t row, shift;
    
    if((X > 131) || (Y > 63)) return;

    row = Y / 8;
    shift = (row+1)*8 - Y - 1;

    LCD_buff_set(X, row, LCD_buff_get(X, row) | (1 << shift));

    LCD_setXY(X,Y);

	openDriver();
    LCD_data_send(LCD_buff_get(X, row));
	closeDriver();
}

//tBoolean LCD_test_pixel(uint8_t X, uint8_t Y)
//{
//uint8_t row, shift;
//    
//    if((X > 131) || (Y > 63)) return 1;
//
//    row = Y / 8;
//    shift = (row+1)*8 - Y - 1;
//    
//    if(LCD_buff_get(X, row) & (1 << shift)) return true;
//    else return false;
//}



void LCD_draw_rectangle(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2)
{
    uint8_t i;

    if((X1 >= X2) || (Y1 >= Y2)) return;

    for(i = X1; i <= X2; i++) //Рисуем верх/низ
    {
        LCD_put_pixel(i, Y1);
        LCD_put_pixel(i, Y2);
    }
    for(i = Y1; i <= Y2; i++) //Рисуем боковые стенки
    {
        LCD_put_pixel(X1, i);
        LCD_put_pixel(X2, i);
    }
}


void LCD_draw_line(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2)
{
int16_t deltax, deltay, error, deltaerr, y, x, step;
    
    if((X1 > 131) || (Y1 > 63)) return;
    if((X2 > 131) || (Y2 > 63)) return;
    if(X1 > X2) return;

    if(X1 == X2)
    {
        for(y = Y1; y <= Y2; y++)
          LCD_put_pixel(X1,y);

        return;
    }

    if(Y1 == Y2)
    {
        for(x = X1; x <= X2; x++)
          LCD_put_pixel(x,Y1);

        return;
    }

    if(Y1 > Y2) step = -1;
    else step = 1;

    deltax = abs(X2 - X1);
    deltay = abs(Y2 - Y1);
    error = 0;
    deltaerr = deltay;
    y = Y1;
    for(x = X1; x <= X2; x++)
    {
        LCD_put_pixel(x,y);
        error = error + deltaerr;
        if (2 * error >= deltax) 
        {
            y = y + step;
            error = error - deltax;
        }
    }
}

//void LCD_draw_circle(uint8_t X, uint8_t Y, uint8_t R)
//{
//int16_t x, y, delta, error;
//
//    x = 0;
//    y = R;
//    delta = 2 - 2 * R;
//    error = 0;
//
//    while (y >= 0)
//    {
//        LCD_put_pixel(X + x, Y + y);
//        LCD_put_pixel(X + x, Y - y);
//        LCD_put_pixel(X - x, Y + y);
//        LCD_put_pixel(X - x, Y - y);
//       
//        error = 2 * (delta + y) - 1;
//        if ((delta < 0) && (error <= 0))
//        {
//            delta += 2 * ++x + 1;
//            continue;
//        }
//        error = 2 * (delta - x) - 1;
//        if ((delta > 0) && (error > 0))
//        {
//            delta += 1 - 2 * --y;
//            continue;
//        }
//        x++;
//        delta += 2 * (x - y);
//        y--;
//    }
//}

//void LCD_draw_full_screen(uint8_t *buff)
//{
//    LCD_setXY(0,0);
//
//	openDriver();
//    LCD_data_send_buf(buff, 1063);
//	closeDriver();
//}

void LCD_draw_image(uint8_t X, uint8_t Y, uint8_t dim_X, uint8_t dim_Y, const uint8_t *image, uint16_t size)
{
uint8_t Row1, Row2, Shift, i, j, tmp, temp;
static uint16_t counter; 

    if((X > 131) || (Y > 63)) return;

    Row1 = Y / 8;
    Shift = (Row1+1)*8 - Y - 1;

    Row2 = (Y + dim_Y) / 8;

    counter = 0;

    for(i = Row1; i <= Row2; i++)
    {
        if(i == Row1) 
        {
            for(j = X; j < X + dim_X; j++) 
            {   
                tmp = LCD_buff_get(j, i);
                tmp = tmp & ~(0xFF >> (7 - Shift));
                temp = image[counter];
                temp = temp >> (7 - Shift);
                LCD_buff_set(j, i, tmp | temp);
                counter++;
            }
        }

        else
        {
            for(j = X; j < X + dim_X; j++)  
            {      
                tmp = LCD_buff_get(j, i);
                tmp = tmp & ~(0xFF << (Shift + 1));
                temp = image[counter - dim_X];
                temp = temp << (Shift + 1);
                LCD_buff_set(j, i, tmp | temp);
                    
                if(counter < size)
                {
                    tmp = LCD_buff_get(j, i);
                    tmp = tmp & ~(0xFF >> (7 - Shift));
                    temp = image[counter];
                    temp = temp >> (7 - Shift);
                    LCD_buff_set(j, i, tmp | temp);
                }
                counter++;
            }
        }
    }

	openDriver();

    j = X;
    for(i = Row1; i <= Row2; i++)
    {
        LCD_set_column(j);
        LCD_set_row(i);

        LCD_data_send_buf(LCD_buff_get_addr(j, i), dim_X);

    }

	closeDriver();
}

//void LCD_fill_area(uint8_t X, uint8_t Y)
//{
//    STACK Stack;
//    uint8_t temp_x, temp_y, xm, xl, xr;
//    int8_t j;
//    tBoolean C;
//
//    Stack.Top = 1;
//
//    Stack.StackXY[Stack.Top].X = X;
//    Stack.StackXY[Stack.Top].Y = Y;
//
//    while(Stack.Top != 0)
//    {
//        temp_x = Stack.StackXY[Stack.Top].X;
//        temp_y = Stack.StackXY[Stack.Top].Y;
//        Stack.Top--;
//        LCD_put_pixel(temp_x, temp_y);
//        xm = temp_x;
//
//        temp_x++;
//        while(!LCD_test_pixel(temp_x, temp_y))
//        {
//            LCD_put_pixel(temp_x, temp_y);
//            temp_x++;
//        }
//
//        xr = temp_x - 1;
//        temp_x = xm;
//        
//        temp_x--;
//        while(!LCD_test_pixel(temp_x, temp_y))
//        {
//            LCD_put_pixel(temp_x, temp_y);
//            temp_x--;
//        }
//
//        xl = temp_x + 1;
//        j = 1;
//MARK1 :
//        temp_y += j;
//        temp_x = xl;
//MARK2 :
//        while(temp_x <= xr)
//        {
//            C = false;
//
//            while((!LCD_test_pixel(temp_x, temp_y)) && (temp_x < xr)) 
//            {
//                C = true;
//                temp_x++;
//            }
//
//            if(C) 
//            {
//                Stack.Top++;
//                Stack.StackXY[Stack.Top].X = temp_x - 1;
//                Stack.StackXY[Stack.Top].Y = temp_y;
//            }
//
//            do
//            {
//                temp_x++;
//            }while(LCD_test_pixel(temp_x, temp_y) && (temp_x < xr));
//            
//
//            goto MARK2;
//        }
//
//        j -= 3;
//        
//        if(j >= -2) goto MARK1;
//    }
//}

void LCD_erase_area(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2)
{
    uint8_t Row1, Row2, Shift1, Shift2, i, j, temp, tmp;

    if((X1 >= X2) || (Y1 >= Y2)) return;

    if(X1 > 131) X1 = 131;
    if(Y1 > 63) Y1  = 63;

    Row1 = Y1 / 8;
    Shift1 = (Row1+1)*8 - Y1 - 1;

    Row2 = Y2 / 8;
    Shift2 = (Row2+1)*8 - Y2 - 1;

    for(i = Row1; i <= Row2; i++)
    {
        temp = 0xFF;
        if(Row1 != Row2)
        {
            if(i == Row1) temp = temp << (Shift1 + 1);
            else if(i == Row2) temp = temp >> (8 - Shift2);
            else temp = 0;
        }
        else
        {
            tmp = 0xFF;
            temp = temp << (Shift1 + 1);
            tmp = tmp >> (8 - Shift2);
            temp = tmp | temp;
        }
                  
        for(j = X1; j <= X2; j++)  
        {
            tmp = LCD_buff_get(j, i);
            LCD_buff_set(j, i, tmp & temp);   
        }
    }

	openDriver();

    j = X1;
    for(i = Row1; i <= Row2; i++)
    {
        LCD_set_column(j);
        LCD_set_row(i);
        LCD_data_send_buf(LCD_buff_get_addr(j, i), X2 - X1 + 1);
    }

	closeDriver();
}

#endif // _DISPLAY_














