#ifndef FRAMES_H
#define FRAMES_H

#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "logo.h"

#define FRAME_LENGTH    22

void create_frames();

struct frame_t {
    PGM_P const *frame_ptr;
};

/***************** Frame descriptors *********************/
enum frameEnum {
    FRAME_MODE_SELECT,
    FRAME_RS232_MAIN,
    FRAME_RS485_MAIN,
    FRAME_RS232,
    FRAME_MAIN,
    FRAME_FREQ,
    FRAME_RS232_SETTINGS,
    FRAME_ID_SELECT,
    FRAME_BAUD_SELECT,
    FRAME_FREQ_ADJUST,

    FRAMES_COUNT
};

struct frame_t frame[FRAMES_COUNT];

/********************* Version *************************/
//const char STR_DEVICE_NAME[] PROGMEM = "Калибратор 3";
const char STR_DEVICE_NAME[] PROGMEM = "Calibrator 3";
//const char STR_FW_TITLE[] PROGMEM = "Версия ПО:";
const char STR_FW_TITLE[] PROGMEM = "Version  :";
const char STR_FW_VERSION[] PROGMEM = "1.0.9";

/********************* Strings *************************/
//const char str_disconnect_electrode[] PROGMEM = " Откл. щуп от датчика";
const char str_disconnect_electrode[] PROGMEM = " Disconnect probe    ";
//const char str_connect_electrode[] PROGMEM = " Подкл. щуп к датчику";
const char str_connect_electrode[] PROGMEM = " Connect probe       ";
const char str_cal_in_progress[] PROGMEM = "  Calibrating ...   ";
//const char str_cal_in_progress[] PROGMEM = "  Идет калибровка...";
const char str_cal_success[] PROGMEM = " Сalibration complete";
//const char str_cal_success[] PROGMEM = " Калибровка завершена";
//const char str_cal_check[] PROGMEM = "Проверка калибровки...";
const char str_cal_check[] PROGMEM = "Check calibration...";
const char str_cal_error[] PROGMEM = "  Error calibration!";
//const char str_cal_error[] PROGMEM = "  Ошибка калибровки!";
const char str_null[] PROGMEM = "";

/********************* Frame 0 *************************/
//const char str_f0_0[] PROGMEM = "[     Тип датчика    ]";
const char str_f0_0[] PROGMEM = "[     Sensor type    ]";
//const char str_f0_1[] PROGMEM = "Частотный";
const char str_f0_1[] PROGMEM = "Frequency";
//const char str_f0_2[] PROGMEM = "Аналоговый";
const char str_f0_2[] PROGMEM = "Analog    ";
const char str_f0_3[] PROGMEM = "RS-232";
const char str_f0_4[] PROGMEM = "RS-485";
///        str_null[]           "                      ";
///        str_null[]           "                      ";
//const char str_f0_7[] PROGMEM = "       Выключить";
const char str_f0_7[] PROGMEM = "      Switch off";

/******************** RS232 Main ************************/
///        str_title_rs232[]    "   [ RS232 датчик ]   ";
///        str_null[]           "                      ";
///        str_null[]           "                      ";
///        str_null[]           "                      ";
///        str_null[]           "                      ";
///        str_null[]           "                      ";
//const char str_exset[] PROGMEM = " Выход      Настройка";
const char str_exset[] PROGMEM = " Exit       Settings ";

/********************* Frame 1 *************************/
//const char str_title_rs232[] PROGMEM = "   [ RS232 датчик ]";
//const char str_title_rs232[] PROGMEM = "[     RS232 датчик   ]";
const char str_title_rs232[] PROGMEM = "[     RS232 output   ]";
//const char str_level_precent[] PROGMEM = "Уровень, %";
const char str_level_precent[] PROGMEM = "Level,   %";
//const char str_level_n[] PROGMEM = "Уровень, N";
const char str_level_n[] PROGMEM = "Level,   N";
//const char str_freq[] PROGMEM = "Частота";
const char str_freq[] PROGMEM = "Frequency";
//const char str_modbus_error[] PROGMEM = "Ошибка";
const char str_modbus_error[] PROGMEM = "Error ";
//const char str_param_edit[] PROGMEM = "    Ред. параметры";
const char str_param_edit[] PROGMEM = "    Edit settings ";
//const char str_empty_tank[] PROGMEM = "Пустой бак";
const char str_empty_tank[] PROGMEM = "Empty tank";
//const char str_full_tank[] PROGMEM = "Полный бак";
const char str_full_tank[] PROGMEM = "Full tank";
const char str_n_max[] PROGMEM = "Nmax";
//const char str_t_average[] PROGMEM = "Усреднение";
const char str_t_average[] PROGMEM = "Average   ";
//const char str_conn_settings[] PROGMEM = "Настройка связи";
const char str_conn_settings[] PROGMEM = "Connection  ...";
//const char str_exit[] PROGMEM = "Выход";
const char str_exit[] PROGMEM = "Exit ";

//const char str_current_draw[] PROGMEM = "Ток,мА";
const char str_current_draw[] PROGMEM = "I, mА";

/**************** Frame 1 Strings **********************/
//const char str_on[] PROGMEM = "ВКЛ";
const char str_on[] PROGMEM = "ON ";
//const char str_off[] PROGMEM = "ВЫКЛ";
const char str_off[] PROGMEM = "OFF ";
const char str_bin_mode[] PROGMEM = "BIN";
const char str_text_mode[] PROGMEM = "TEXT";
//const char str_no_conn[] PROGMEM = "       нет связи";
const char str_no_conn[] PROGMEM = "   no connection";

/********************* Frame 2 *************************/
//const char str_f2_0[] PROGMEM = "[      Настройка     ]";
const char str_f2_0[] PROGMEM = "[      Settings      ]";
const char str_id[] PROGMEM = "ID";
const char str_baudrate[] PROGMEM = "Baudrate";
//const char str_net_mode[] PROGMEM = "Сетевой режим";
const char str_net_mode[] PROGMEM = "Network mode ";
//const char str_auto_send[] PROGMEM = "Авт. выдача";
const char str_auto_send[] PROGMEM = "Self transmit";
//const char str_interval[] PROGMEM = "Период выдачи, с";
const char str_interval[] PROGMEM = "Transmit time, с";
///        str_exit             "Выход                 ";
///        str_null[]           "                      ";

/********************* Frame 3 *************************/
//const char str_title_rs485[] PROGMEM = "   [ RS485 датчик ]";
//const char str_title_rs485[] PROGMEM = "[     RS485 датчик   ]";
const char str_title_rs485[] PROGMEM = "[     RS485 output   ]";

/********************* Frame 4 *************************/
//const char str_f4_0[] PROGMEM = "[  Частотный датчик  ]";
const char str_f4_0[] PROGMEM = "[  Frequency output  ]";
///        str_null[]           "                      ";
///        str_null[]           "                      ";
///        str_null[]           "                      ";
///        str_null[]           "                      ";
///        str_null[]           "                      ";
//const char str_excl[] PROGMEM = " Выход     Калибровка";
const char str_excl[] PROGMEM = " Exit     Calibration";

/********************* Frame 5 *************************/
//const char str_f5_0[] PROGMEM = "[ Аналоговый  датчик ]";
const char str_f5_0[] PROGMEM = "[ Analog output      ]";

/******************** ID select ************************/
//const char str_f6_0[] PROGMEM = "[     ID датчика     ]";
const char str_f6_0[] PROGMEM = "[     ID sensor      ]";
///        str_null[]           "                      ";
///        str_null[]           "                      ";
const char str_f6_3[] PROGMEM = "    -   [ 01 ]   +";
///        str_null[]           "                      ";
///        str_null[]           "                      ";
///        str_null[]           "                      ";
//const char str_okcn[] PROGMEM = "  Отмена          ОК";
const char str_okcn[] PROGMEM = "  Cancel          ОК";

/******************* Baud select ***********************/
const char str_f7_0[] PROGMEM = "[      Baudrate      ]";
///        str_null[]           "                      ";
const char str_f7_2[] PROGMEM = "  9600   14440 19200";
const char str_f7_3[] PROGMEM = "  38400  57600 115200";
///        str_null[]           "                      ";
//const char str_f7_5[] PROGMEM = "  (По умолч.: 19200)";
const char str_f7_5[] PROGMEM = "  (Default  : 19200)";
///        str_null[]           "                      ";
//const char str_f7_7[] PROGMEM = "        Отмена";
const char str_f7_7[] PROGMEM = "        Cancel";

/***************** Frequency adjust ********************/
//const char str_f8_0[] PROGMEM = "  [ Ред. параметра ]";
const char str_f8_0[] PROGMEM = "  [ Edit settings  ]";
///        str_null[]           "                      ";
///        str_f8_2[]           "       Параметр:      ";
///        str_null[]           "       000000         ";
///        str_null[]           "                      ";
//const char str_f8_5[] PROGMEM = "        Сброс";
const char str_f8_5[] PROGMEM = "        Reset";
///        str_null[]           "                      ";
///        str_okcn[]           "  Отмена          ОК  ";
/*******************************************************/

PGM_P const frame_mode_select[] PROGMEM = {
    /** 0 **/
    str_f0_0,
    str_f0_1,
    str_f0_2,
    str_f0_3,
    str_f0_4,
    str_null,
    str_null,
    str_f0_7,
};

PGM_P const frame_rs232[] PROGMEM = {
    /** 1.0 **/
    str_title_rs232,
    str_level_precent,
    str_level_n,
    str_freq,
    str_modbus_error,
    str_param_edit,
    str_empty_tank,
    str_full_tank,
    /** 1.1 **/
    str_n_max,
    str_t_average,
    str_conn_settings,
    str_exit,
};

PGM_P const frame_rs232_settings[] PROGMEM = {
    /** 2 **/
    str_f2_0,
    str_id,
    str_baudrate,
    str_net_mode,
    str_auto_send,
    str_interval,
    str_exit,
    str_null,
};

PGM_P const frame_freq[] PROGMEM = {
    /** 4 **/
    str_f4_0,
    str_null,
    str_null,
    str_null,
    str_null,
    str_null,
    str_null,
    str_excl,
};

PGM_P const frame_analog[] PROGMEM = {
    /** 5 **/
    str_f5_0,
    str_null,
    str_null,
    str_null,
    str_null,
    str_null,
    str_null,
    str_excl,
};

PGM_P const frame_id_select[] PROGMEM = {
    /** 6 **/
    str_f6_0,
    str_null,
    str_null,
    str_f6_3,
    str_null,
    str_null,
    str_null,
    str_okcn,
};

PGM_P const frame_baud_select[] PROGMEM = {
    /** 7 **/
    str_f7_0,
    str_null,
    str_f7_2,
    str_f7_3,
    str_null,
    str_f7_5,
    str_null,
    str_f7_7,
};

PGM_P const frame_param_adjust[] PROGMEM = {
    /** 8 **/
    str_f8_0,
    str_null,
    str_null,
    str_null,
    str_null,
    str_f8_5,
    str_null,
    str_okcn,
};

PGM_P const frame_rs232_main[] PROGMEM = {
    /** 232 **/
    str_title_rs232,
    str_null,
    str_null,
    str_null,
    str_null,
    str_null,
    str_null,
    str_exset,
};

PGM_P const frame_rs485_main[] PROGMEM = {
    /** 485 **/
    str_title_rs485,
    str_null,
    str_null,
    str_null,
    str_null,
    str_null,
    str_null,
    str_exset,
};

/*******************************************************/

void create_frames() {
    frame[FRAME_MODE_SELECT].frame_ptr = &frame_mode_select[0];
    frame[FRAME_RS232_MAIN].frame_ptr = &frame_rs232_main[0];
    frame[FRAME_RS485_MAIN].frame_ptr = &frame_rs485_main[0];
    frame[FRAME_MAIN].frame_ptr = &frame_analog[0];
    frame[FRAME_FREQ].frame_ptr = &frame_freq[0];
    frame[FRAME_RS232].frame_ptr = &frame_rs232[0];
    frame[FRAME_RS232_SETTINGS].frame_ptr = &frame_rs232_settings[0];
    frame[FRAME_ID_SELECT].frame_ptr = &frame_id_select[0];
    frame[FRAME_BAUD_SELECT].frame_ptr = &frame_baud_select[0];
    frame[FRAME_FREQ_ADJUST].frame_ptr = &frame_param_adjust[0];
}

void LCD_puts(PGM_P str) {
    char c[16], *ptr;
    strcpy_P(c, str);
    ptr = c;
    while (*ptr)
        LCD_putchar(*(ptr++), false);
}

void boot_logo_display() {
    //eeprom_write_block((const void *) logo, (void *) 0x00, sizeof (logo));
    LCD_goto(0, 0);
    uint8_t row = 0;
    for (int i = 0; i < (sizeof (logo) / sizeof (logo[0])); i++) {
        if (i % 64 == 0) {
            LCD_goto(row++, 0);
        }
        LCD_open();
        i2c_write(eeprom_read_byte((void *) i));
        LCD_close();
    }
    LCD_goto(2, 60);

    LCD_puts(STR_DEVICE_NAME);
    LCD_goto(3, 60);
    LCD_puts(STR_FW_TITLE);
    LCD_goto(4, 60);
    LCD_puts(STR_FW_VERSION);
    _delay_ms(2000);
}

#endif /* FRAMES_H */
