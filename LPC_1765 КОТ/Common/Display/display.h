#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#define CLOCK_SIZE_X 22
#define CLOCK_SIZE_Y 26

#define SERVICE_MENU_DEPTH  	16    //глубина подменю сервиса
#define SERVICE_INDEP_SUBMENU	9	  // номер подменю, которое будет отображаться независимо от конфигурации
#define ARCHIVE_MENU_DEPTH  	2    //глубина подменю архива

//определяем, что настриваем из время/даты
#define SETUP_NOTH  0
#define SETUP_MIN   1
#define SETUP_HOUR  2
#define SETUP_DATE  3
#define SETUP_MONTH 4
#define SETUP_YEAR  5


struct tagDigitPlace
{
    uint8_t X;
    uint8_t Y;
};

void init_lcd(void);
void update_menu(uint8_t drawMenuFlag);
void switch_menu(uint8_t press);
void reset_to_clock(void);
void go_to_error_page(void);

void lock_page(uint8_t page);
void set_blink_flag(uint8_t blinkFlag);
void clr_blink_flag(uint8_t blinkFlag);
void set_fail_page(uint8_t failFlag);
void clr_fail_page(uint8_t failFlag);
void set_obriv_flag(uint8_t obrivFlag);
void clr_obriv_flag(uint8_t blinkFlag);

#endif
