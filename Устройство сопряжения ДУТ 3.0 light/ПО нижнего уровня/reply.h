#ifndef _REPLY_H_
#define _REPLY_H_

#include <avr/eeprom.h>
#include "omnicomm.h"

#define NET_MODE			0x01
#define INDEPENDENT_MODE	0x00

#define SET_ID				0x01
#define ID_READ				0x02
#define SET_ADDR			0x03
#define SET_NET_MODE		0x04
#define CONFIG_READ			0x05
#define DATA_READ			0x06
#define DATA_CONTIN			0x07
#define FCURR_FMIN			0x08
#define FCURR_FMAX			0x09
#define TSLOPE_T0			0x0A
#define SET_FMIN			0x0B
#define SET_FMAX			0x0C
#define SET_PWM_MAX			0x0D
#define SET_PWM_MOD			0x0E
#define SET_TRL				0x0F
#define SET_FILT_LENG		0x11
#define SET_PASSW			0x12
#define SET_PERIOD			0x13
#define FILT_LENG_READ		0x14
#define PASSW_READ			0x15
#define SET_AUTO_MODE		0x17


extern unsigned char my_addr;

extern unsigned char net_addr, net_mode, k_t0, t, pwm_mode, filt_length, period_auto, passw, report_on;
extern unsigned int n, f_curr, k_t, f_min_t, f_max_t, pwm_max, trl;
extern unsigned long id, password_hi, password_lo;

void Read_parameters (void);
void Set_id (void);
void Id_read (void);
void Set_addr (void);
void Set_net_mode (void);
void Config_read (void);
void Data_read (unsigned int _type);
void Data_contin (unsigned int _type);
void Fcurr_fmin (void);
void Fcurr_fmax (void);
void Tslope_t0 (void);
void Set_fmin (void);
void Set_fmax (void);
void Set_pwm_max (void);
void Set_pwm_mod (void);
void Set_trl (void);
void Set_filt_length (void);
void Set_passw (void);
void Set_period (void);
void Filt_leng_read (void);
void Passw_read (void);
void Set_auto_mode (unsigned char _param);
void Enable_auto_old_mode (void);
void Disable_auto_old_mode (void);
unsigned char Get_auto_old_mode (void);

#endif
