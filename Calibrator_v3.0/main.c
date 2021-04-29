#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdio.h>
#include <math.h>
#include <avr/interrupt.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#define LNG_ENGLISH

#include "lib/config.h"
#include "lib/misc.h"
#include "lib/i2c.h"
#include "lib/LCD_133X65_UC1601S.h"
#include "lib/Button.h"

#ifdef LNG_ENGLISH
#include "lib/frames_eng.h"
#else
#include "lib/frames.h"
#endif

#include "lib/event.h"
#include "lib/timer.h"
#include "lib/freq.h"
#include "lib/ADC.h"
#include "lib/modbus/MBServer.h"
#include "lib/modbus/DOT_registers.h"
#include "lib/uart.h"
#include "lib/logo.h"

volatile struct {
    unsigned char ShortKeyPressed : 1;
    unsigned char LongKeyPressed : 1;
    unsigned char TaskChanged : 1;
} ApplicationEvents;

volatile struct {
    unsigned char ExternalVoltage : 1;
    unsigned char LowBattery : 1;
} ApplicationFlags;

static char framebuffer[8][FRAME_LENGTH];

static struct {
    int8_t row : 6;
    uint8_t sel_start : 5;
    uint8_t sel_end : 5;
} frameSelection;

static const uint32_t baud_lut[] = {9600, 14440, 19200, 38400, 57600, 115200};
static uint8_t baud_index = 2;

enum sensorType {
    SENSOR_FREQUENCY = 1, SENSOR_ANALOG, SENSOR_RS232, SENSOR_RS485
};

static struct ADC_result adc;

static struct task_cb
mode_select_task, analog_task, freq_task, rs232_main_task, rs485_main_task,
rs232_task, rs232_settings_task, rs232_id_select_task, rs232_baud_select_task,
rs232_param_adjust_task;

static task_t *current_task;

static void schedule_task(task_t *t) {
    current_task = t;
    ApplicationEvents.TaskChanged = 1;
}

static inline void handle_keys() {
    if (get_key_short(1 << KEY3))
        ApplicationEvents.ShortKeyPressed = 1;

    if (get_key_long(1 << KEY3)) {
        make_annoying_beep(BEEP_SHORT);
        ApplicationEvents.LongKeyPressed = 1;
    }

    if (ApplicationEvents.ShortKeyPressed || ApplicationEvents.LongKeyPressed)
        sleep_timer_reset();
}

/******************************************************************************
 * Framebuffer functions
 *****************************************************************************/

/**
 * Redraw a single line from framebuffer and apply selection.
 * @param row index in framebuffer
 */
static void redraw_partial(uint8_t row) {
    /* quick workaround to display external power */
    framebuffer[0][1] = (ApplicationFlags.ExternalVoltage) ? EXT_PWR_CHAR : ' ';

    LCD_goto(row, 0);
    if (frameSelection.row == row) {
        register uint8_t j = 0;
        while (j < frameSelection.sel_start)
            LCD_putchar(framebuffer[row][j++], 0);

        while (j < frameSelection.sel_end)
            LCD_putchar(framebuffer[row][j++], 1);

        while (j < FRAME_LENGTH)
            LCD_putchar(framebuffer[row][j++], 0);
    } else {
        for (uint8_t j = 0; j < FRAME_LENGTH; j++)
            LCD_putchar(framebuffer[row][j], 0);
    }
}

/**
 * Redraw screen from framebuffer and apply menu selection.
 * Clears frameSelection.
 */
static void redraw() {
    for (uint8_t i = 0; i < 8; i++)
        redraw_partial(i);
    frameSelection.row = -1;
}

static void fb_clear() {
    char *c = framebuffer[0];
    while (c <= &framebuffer[7][21])
        *(c++) = ' ';
}

static void fb_clear_line(uint8_t l) {
    for (uint8_t i = 0; i < FRAME_LENGTH; i++)
        framebuffer[l][i] = ' ';
}

static void fb_put_u8(uint8_t i, uint8_t row, uint8_t col) {
    char c[4];
    uint8_t ofs = 0;
    if (i < 10)
        c[ofs++] = '0';
    itoa(i, &c[ofs], 10);
    memcpy(&framebuffer[row][col], c, strlen(c));
}

static void fb_put_tag(uint8_t row, PGM_P tag, uint32_t value) {
    char v[11];
    ultoa(value, v, 10);
    fb_clear_line(row);
    strcpy_P(framebuffer[row], tag);
    memcpy(&framebuffer[row][FRAME_LENGTH - strlen(v)], v, strlen(v));
}

static void fb_put_tag_at(uint8_t row, uint8_t col, PGM_P tag, uint16_t value) {
    char v[11];
    ultoa(value, v, 10);
    fb_clear_line(row);
    char *dest = strcpy_P(&framebuffer[row][col], tag);
    //if (strlen(v) + strlen_P(tag) + col % 5 >= FRAME_LENGTH) return;
    memcpy(dest + strlen_P(tag), v, strlen(v));
}

/**
 * @param row framebuffer row (0..7)
 * @param str PGM_P string (left-aligned)
 * @param tag text tag (right-aligned)
 */
static void fb_put_tag_str(uint8_t row, PGM_P str, PGM_P tag) {
    fb_clear_line(row);
    strcpy_P(framebuffer[row], str);
    /*@note: strcpy would copy additional character */
    memcpy_P(&framebuffer[row][FRAME_LENGTH - strlen_P(tag)], tag, strlen_P(tag));
}

/**
 * @param row framebuffer row (0..7)
 * @param str PGM_P string
 */
static void fb_put_str(uint8_t row, PGM_P str) {
    fb_clear_line(row);
    memcpy_P(framebuffer[row], str, strlen_P(str));
}

/**
 * Write string into framebuffer and perform partial LCD redraw
 * @param str PGM_P string (left-aligned)
 * @param row framebuffer row (0..7)
 */
static void fb_put_str_redraw(PGM_P str, uint8_t row) {
    fb_put_str(row, str);
    redraw_partial(row);
}

/**
 * Load frame associated w/ current task
 */
static void load_frame() {
    fb_clear();
    struct frame_t *fs = (struct frame_t *) current_task->container;
    for (uint8_t i = 0; i < 8; i++)
        strcpy_P(framebuffer[i], (PGM_P) pgm_read_word(&(fs->frame_ptr[i])));
}

/**
 * Load frame and scroll down by 1 line
 * @param offset index of the next line in frame
 */
static void load_frame_scroll(uint8_t offset) {
    struct frame_t *fs = (struct frame_t *) current_task->container;
    /* skip framebuffer[0] to leave title intact and scroll lines down */
    for (uint8_t i = 1; i < 7; i++)
        memcpy(framebuffer[i], framebuffer[i + 1], FRAME_LENGTH);

    /* clear last line */
    fb_clear_line(7);

    /* load last line from flash */
    strcpy_P(framebuffer[7], (PGM_P) pgm_read_word(&(fs->frame_ptr[7 + offset])));
}

/**
 * Highlight selection
 * @param row
 * @param sel_start selection start
 * @param sel_end selection end
 */
static void menu_item_highlight(uint8_t row, uint8_t sel_start, uint8_t sel_end) {
    frameSelection.row = row;
    frameSelection.sel_start = sel_start;
    frameSelection.sel_end = sel_end;
}

/**
 * Highlight complete row
 * @param row
 */
static void menu_row_highlight(uint8_t row) {
    frameSelection.row = row;
    frameSelection.sel_start = 0;
    frameSelection.sel_end = FRAME_LENGTH;
}

/******************************************************************************
 * Modbus-specific functions
 *****************************************************************************/

/**
 * Modbus non-blocking read.
 *
 * @param addr register address
 * @param count registers count
 * @param id client ID
 * @return 0 - busy,
 *         1 - packet OK
 */
static uint8_t modbus_poll_r(uint8_t addr, uint8_t count, uint8_t id) {
    if (timer0_expired())
        ServerReadInputRegister(addr, count, id);
    return modbus_read_ok(count, id);
}

/**
 * Modbus blocking write.
 *
 * @param address register address
 * @param reg register value
 * @param id client ID
 * @return 0 - busy,
 *         1 - packet OK
 */
static int8_t modbus_poll_wr(uint16_t address, uint16_t reg, uint8_t id) {
    ServerWriteSingleRegister(address, reg, id);
    uint32_t ctr = 0;
    uint8_t ok = 0;
    while (!ok) {
        if (ctr++ >= F_CPU / 512) {
            ctr = 0;
            return -1;
        }
        ok = modbus_write_ok(id);
    }
    return ok;
}

static uint8_t print_float_register(uint8_t reg, uint8_t pos, uint8_t device_id, PGM_P str) {
    uint8_t got_val = modbus_poll_r(reg, 2, device_id);
    if (got_val && pos > 0 && pos < 8) {
        float f = GetInpRegsFLOAT(0);
        long unsigned int a = f;
        fb_put_tag(pos, str, a);
    }
    return got_val;
}

static uint8_t print_ushort_register(uint8_t reg, uint8_t pos, uint8_t device_id, PGM_P str) {
    uint8_t got_val = modbus_poll_r(reg, 1, device_id);
    if (got_val && pos > 0 && pos < 8) {
        unsigned short us = GetInpRegsUSHORT(0);
        fb_put_tag(pos, str, us);
    }
    return got_val;
}

/******************************************************************************
 * Analog sensor calibration routines
 *****************************************************************************/
enum sensorCalState {
    CAL_STATE0, CAL_STATE1, CAL_STATE2, CAL_STATE3, CAL_STATE4
};

/**
 * Analog sensor calibration procedure
 * 1) Sensor: 300-1500Hz [+-2Hz] || 0-15V [+-60mV]
 * 2) User: short out central electrode
 * 3) Sensor: 340Hz || 1.4V
 * 4) Device: 430Hz
 * 5) Sensor: 330Hz || 1.3V
 * 6) User: disconnect electrode
 * 7) Sensor: 460Hz || 2.6V (~2sec)
 * 8) Sensor: 500Hz || 0V
 *
 * @param value measured frequency or voltage
 * @param type sensor type
 * @param state
 * @return 0 = incorrect value; 1 = correct value
 */
static uint8_t analog_sensor_check_result(uint16_t value, enum sensorType type, uint8_t state) {

    enum {
        dV = 6, // 60mV
        dF = 20 // 2Hz
    };

    switch (state) {
        case CAL_STATE0:
            if (type == SENSOR_ANALOG) {
                return (value >= 0 && value <= 1500 + dV);
            } else if (type == SENSOR_FREQUENCY) {
                return (value >= 3000 - dF && value <= 15000 + dF);
            }
            break;
        case CAL_STATE1:
            if (type == SENSOR_ANALOG) {
                return (abs(value - 140) <= dV);
            } else if (type == SENSOR_FREQUENCY) {
                return (abs(value - 3400) <= dF || abs(value - 15000) <= dF);
            }
            break;
        case CAL_STATE2:
            if (type == SENSOR_ANALOG) {
                return (abs(value - 130) <= dV);
            } else if (type == SENSOR_FREQUENCY) {
                return (abs(value - 3300) <= dF);
            }
            break;
        case CAL_STATE3:
            if (type == SENSOR_ANALOG) {
                return (abs(value - 260) <= dV);
            } else if (type == SENSOR_FREQUENCY) {
                return (abs(value - 4600) <= dF);
            }
            break;
        case CAL_STATE4:
            if (type == SENSOR_ANALOG) {
                return (abs(value) <= dV + 2); // allow a bit more deviation around zero
            } else if (type == SENSOR_FREQUENCY) {
                return (abs(value - 5000) <= dF);
            }
            break;
    }
    return 0;
}

/**
 * Calibrate analog or frequency sensor (depending on sensorType)
 * @param value measured frequency or voltage
 * @param reset reset calibration state
 * @param type sensor type
 * @return 0 = calibration in progress; 1 = success; -1 = fail
 */
static int8_t run_analog_calibration(uint16_t value, bool reset, enum sensorType type) {
    static uint8_t state;
    static volatile uint32_t clb_confirm_ctr;

    if (reset) {
        state = CAL_STATE0;
        return 0;
    }

    uint8_t check = analog_sensor_check_result(value, type, state);

    switch (state) {
        case CAL_STATE0:
            if (check) {
                fb_put_str_redraw(str_connect_electrode, 5);
                soft_PWM_stop();
                state = CAL_STATE1;
                clb_confirm_ctr = 0;
                timer0_preload_ms(1000);
                timer0_reset();
            } else {
                return -1;
            }
            break;
        case CAL_STATE1:
            if (check) {
                fb_put_str_redraw(str_cal_in_progress, 5);
                if (timer0_expired()) {
                    /* '429' produces closest output to 430Hz */
                    soft_PWM_start(429);
                    state = CAL_STATE2;
                }
            } else {
                timer0_reset();
            }
            break;
        case CAL_STATE2:
            if (check) {
                if (timer0_expired()) {
                    fb_put_str_redraw(str_disconnect_electrode, 5);
                    make_annoying_beep(BEEP_DOUBLE);
                    state = CAL_STATE3;
                }
            } else {
                timer0_reset();
            }
            break;
        case CAL_STATE3:
            if (check) {
                fb_put_str_redraw(str_cal_check, 5);
                state = CAL_STATE4;
                timer0_reset();
            }
            break;
        case CAL_STATE4:
            if (check) {
                if (timer0_expired()) {
                    make_annoying_beep(BEEP_DOUBLE);
                    soft_PWM_stop();
                    state = CAL_STATE0;
                    clb_confirm_ctr = 0;
                    return 1;
                }
            } else {
                timer0_reset();
                if (clb_confirm_ctr++ >= F_CPU / 300) {
                    clb_confirm_ctr = 0;
                    return -1;
                }
            }
            break;
    };

    return 0;
}

/******************************************************************************
 * Event Handlers
 *****************************************************************************/
static void mode_select_event_cb(task_t *evt) {

    enum menuSelector {
        BTN_FREQ = 1, BTN_ANALOG, BTN_232, BTN_485, BTN_SHUTDOWN = 7
    };

    static enum menuSelector row_selector = BTN_FREQ;

    if (ApplicationEvents.TaskChanged) {
        load_frame();
        menu_row_highlight(row_selector);
        redraw();
        ADC_enable(&adc);
        ApplicationEvents.TaskChanged = 0;
    }

    if (ADC_ready()) {
        ApplicationFlags.ExternalVoltage = (adc.channel_12v >= EXT_VOLTAGE_THRESHOLD) ? 1 : 0;
        redraw_partial(0);
    }

    if (ApplicationEvents.ShortKeyPressed) {
        row_selector++;

        if (row_selector > BTN_SHUTDOWN)
            row_selector = BTN_FREQ;
        else if (row_selector > BTN_485)
            row_selector = BTN_SHUTDOWN;

        menu_row_highlight(row_selector);
        redraw();

        ApplicationEvents.ShortKeyPressed = 0;
    }

    if (ApplicationEvents.LongKeyPressed) {
        switch (row_selector) {
            case BTN_FREQ:
                schedule_task(&freq_task);
                break;
            case BTN_ANALOG:
                schedule_task(&analog_task);
                break;
            case BTN_232:
                schedule_task(&rs232_main_task);
                break;
            case BTN_485:
                schedule_task(&rs485_main_task);
                break;
            case BTN_SHUTDOWN:
                ADC_disable();
                LCD_clear();
                schedule_delayed_sleep();
                break;
        }

        ApplicationEvents.LongKeyPressed = 0;
    }
}

enum {
    USHORT_TYPE, FLOAT_TYPE, ULONG_TYPE
};

struct {
    uint32_t default_value;
    uint32_t register_value;
    uint16_t max_value;
    uint8_t register_addr;
    uint8_t register_type;
    const char *param_str;
} parameters;

struct {
    float full_tank;
    float empty_tank;
    float freq;
    uint16_t max_n;
    uint16_t time_average;
} modbus_rx_data;

static void rs232_event_cb(task_t *evt) {

    enum menuSelector {
        BTN_LEVEL = 1, BTN_LEVEL_N, BTN_FREQ, BTN_ERROR, BTN_PARAM_EDIT_LABEL,
        BTN_EMPTY, BTN_FULL, BTN_NMAX_EDIT, BTN_TAVERAGE_EDIT, BTN_SETTINGS,
        BTN_EXIT
    };

    enum optionSelector {
        MB_LEVEL_PERCENT = 1, MB_LEVEL_N, MB_DOT_FREQ, MB_ERROR, MB_BLANK,
        MB_EMPTY_TANK, MB_FULL_TANK, MB_NMAX, MB_TIME_AVERAGE
    };

    static uint8_t row_selector;
    static uint8_t scroll;
    static uint8_t device_id;

    static enum optionSelector option_selector;

    if (ApplicationEvents.TaskChanged) {
        enum sensorType sensor_type = (enum sensorType) evt->type;

        modbus_rx_data.empty_tank = 0;
        modbus_rx_data.full_tank = 0;
        modbus_rx_data.max_n = 0;
        modbus_rx_data.time_average = 0;

        if (!ApplicationFlags.ExternalVoltage)
            boost_enable();
        UART_init(baud_lut[baud_index]);
        load_frame();
        bool use_rs485 = false;
        if (sensor_type == SENSOR_RS485) {
            fb_put_str(0, (PGM_P) str_title_rs485);
            use_rs485 = true;
            rs232_disable();
        } else if (sensor_type == SENSOR_RS232) {
            rs485_disable();
            rs232_enable();
        }
        Modbus_enable(use_rs485);
        timer0_preload_ms(50); // retry failed packet every 50ms
        timer0_reset();
        device_id = 0;
        scroll = 0;
        option_selector = MB_LEVEL_PERCENT;
        row_selector = BTN_EMPTY;
        menu_row_highlight(row_selector);
        redraw();

        ApplicationEvents.TaskChanged = 0;
    }

    if (device_id) {
        uint8_t got_val = 0;
        uint8_t pos = option_selector - scroll;

        static float lvl_percent = 0;

        switch (option_selector) {
            case MB_LEVEL_PERCENT:
                got_val = print_float_register(DOT_LEVEL_PERCENT, pos, device_id, str_level_precent);
                if (got_val) {
                    lvl_percent = GetInpRegsFLOAT(0);
                    option_selector++;
                }
                break;
            case MB_LEVEL_N:
                got_val = modbus_poll_r(DOT_OMNI_MAX_N, 1, device_id);
                if (got_val) {
                    unsigned short omni_max_n = GetInpRegsUSHORT(0);
                    modbus_rx_data.max_n = omni_max_n;
                    omni_max_n *= (double) (lvl_percent / 100);
                    if (pos > 0) {
                        fb_put_tag(pos, str_level_n, omni_max_n);
                    }
                    option_selector++;
                }
                break;
            case MB_DOT_FREQ:
                got_val = print_float_register(DOT_FREQ, pos, device_id, str_freq);
                if (got_val) {
                    modbus_rx_data.freq = GetInpRegsFLOAT(0);
                    option_selector++;
                }
                break;
            case MB_ERROR:
                got_val = print_ushort_register(DOT_ERROR_CODE, pos, device_id, str_modbus_error);
                if (got_val) option_selector++;
                break;
            case MB_BLANK:
                option_selector++;
                break;
            case MB_EMPTY_TANK:
                got_val = print_float_register(DOT_EMPTY_TANK, pos, device_id, str_empty_tank);
                if (got_val) {
                    modbus_rx_data.empty_tank = GetInpRegsFLOAT(0);
                    option_selector++;
                }
                break;
            case MB_FULL_TANK:
                got_val = print_float_register(DOT_FULL_TANK, pos, device_id, str_full_tank);
                if (got_val) {
                    modbus_rx_data.full_tank = GetInpRegsFLOAT(0);
                    option_selector++;
                }
                break;
            case MB_NMAX:
                if (pos > 0 && pos < 8)
                    fb_put_tag(pos, str_n_max, modbus_rx_data.max_n);
                option_selector++;
                break;
            case MB_TIME_AVERAGE:
                got_val = print_ushort_register(DOT_TIME_AVERAGE, pos, device_id, str_t_average);
                if (got_val) {
                    modbus_rx_data.time_average = GetInpRegsUSHORT(0);
                    option_selector++;
                }
                break;
            default:
                menu_row_highlight(row_selector - scroll);
                redraw();
                option_selector = MB_LEVEL_PERCENT;
                break;
        }
        if (got_val) {
            timer3s_reset();
            //            if ((enum sensorType) evt->type == SENSOR_RS485)
            //                fb_put_str(0, (PGM_P) str_title_rs485);
            //            else
            //                fb_put_str(0, (PGM_P) str_title_rs232);
        } else if (timer3s_expired()) {
            fb_put_str(0, (PGM_P) str_no_conn);
            redraw();
            device_id = 0;
        }
    } else {
        int8_t got_id = modbus_poll_r(DOT_DEVICE_ID, 1, 0);
        if (got_id > 0) {
            timer3s_reset();
            device_id = modbus_get_id();
        }
    }

    if (ApplicationEvents.ShortKeyPressed) {
        if (++row_selector > 7) {
            if (row_selector > BTN_EXIT) {
                load_frame();
                if ((enum sensorType) evt->type == SENSOR_RS485)
                    fb_put_str(0, (PGM_P) str_title_rs485); // load 'rs485' title
                //row_selector = BTN_LEVEL;
                row_selector = BTN_EMPTY;
                option_selector = MB_LEVEL_PERCENT;
                scroll = 0;
            } else {
                scroll++;
                load_frame_scroll(row_selector - 7);
            }
        }
        if (row_selector == BTN_PARAM_EDIT_LABEL) row_selector++;
        menu_row_highlight(row_selector - scroll);
        redraw();

        ApplicationEvents.ShortKeyPressed = 0;
    }

    redraw_partial(0);


    if (ApplicationEvents.LongKeyPressed) {
        switch (row_selector) {
            case BTN_EMPTY:
                parameters.default_value = modbus_rx_data.freq;
                parameters.register_value = modbus_rx_data.empty_tank;
                parameters.max_value = 0;
                parameters.register_type = FLOAT_TYPE;
                parameters.register_addr = DOT_EMPTY_TANK;
                parameters.param_str = str_empty_tank;

                task_set_id(&rs232_param_adjust_task, device_id);
                schedule_task(&rs232_param_adjust_task);
                break;
            case BTN_FULL:
                parameters.default_value = modbus_rx_data.freq;
                parameters.register_value = modbus_rx_data.full_tank;
                parameters.max_value = 0;
                parameters.register_type = FLOAT_TYPE;
                parameters.register_addr = DOT_FULL_TANK;
                parameters.param_str = str_full_tank;

                task_set_id(&rs232_param_adjust_task, device_id);
                schedule_task(&rs232_param_adjust_task);
                break;
            case BTN_NMAX_EDIT:
                parameters.default_value = 1023;
                parameters.register_value = modbus_rx_data.max_n;
                parameters.max_value = 65535;
                parameters.register_type = USHORT_TYPE;
                parameters.register_addr = DOT_OMNI_MAX_N;
                parameters.param_str = str_n_max;

                task_set_id(&rs232_param_adjust_task, device_id);
                schedule_task(&rs232_param_adjust_task);
                break;
            case BTN_TAVERAGE_EDIT:
                parameters.default_value = 0;
                parameters.register_value = modbus_rx_data.time_average;
                parameters.max_value = 90;
                parameters.register_type = USHORT_TYPE;
                parameters.register_addr = DOT_TIME_AVERAGE;
                parameters.param_str = str_t_average;

                task_set_id(&rs232_param_adjust_task, device_id);
                schedule_task(&rs232_param_adjust_task);
                break;
            case BTN_SETTINGS:
                task_set_type(&rs232_settings_task, (enum sensorType) evt->type);
                schedule_task(&rs232_settings_task);
                break;
            case BTN_EXIT:
                Modbus_disable();
                boost_disable();
                rs232_disable();
                rs485_disable();
                schedule_task(&mode_select_task);
                break;
        }
        ApplicationEvents.LongKeyPressed = 0;
    }
}

static void rs232_id_select_event_cb(task_t * evt) {

    enum menuSelector {
        BTN_MINUS, BTN_PLUS, BTN_CANCEL, BTN_OK
    };

    static enum menuSelector menu_selector;
    static uint8_t id;

    if (ApplicationEvents.TaskChanged) {
        id = evt->id;

        menu_selector = 0;
        load_frame();
        fb_put_u8(id, 3, 10);
        menu_item_highlight(3, 3, 6);
        redraw();

        ApplicationEvents.TaskChanged = 0;
    }

    if (ApplicationEvents.ShortKeyPressed) {
        if (++menu_selector > 3)
            menu_selector = 0;

        switch (menu_selector) {
            case BTN_MINUS:
                menu_item_highlight(3, 3, 6);
                break;
            case BTN_PLUS:
                menu_item_highlight(3, 16, 19);
                break;
            case BTN_CANCEL:
                menu_item_highlight(7, 1, 9);
                break;
            case BTN_OK:
                menu_item_highlight(7, 17, 21);
                break;
        }
        redraw();
        ApplicationEvents.ShortKeyPressed = 0;
    }

    //@FIXME: this still works like shit..
    if (ApplicationEvents.LongKeyPressed || get_key_rpt_l(1 << KEY3)) {
        switch (menu_selector) {
            case BTN_MINUS:
                if (id > 1) {
                    id--;
                    menu_item_highlight(3, 3, 6);
                    fb_put_u8(id, 3, 10);
                    redraw();
                }
                break;
            case BTN_PLUS:
                if (id < 99) {
                    id++;
                    menu_item_highlight(3, 16, 19);
                    fb_put_u8(id, 3, 10);
                    redraw();
                }
                break;
            case BTN_OK:
                /* Perform write operation */
                if (modbus_poll_wr(DOT_DEVICE_ID, id, evt->id) > 0)
                    schedule_task(&rs232_settings_task);
                break;
            case BTN_CANCEL:
                schedule_task(&rs232_settings_task);
                break;
        }

        ApplicationEvents.LongKeyPressed = 0;
    }
}

static void rs232_param_adjust_event_cb(task_t *evt) {

    enum menuSelector {
        BTN_DIGIT0 = 0, BTN_DIGIT5 = 5, BTN_RESET, BTN_CANCEL, BTN_OK
    };

    const uint8_t pofs = 7; // subtitle and editor offset
    static enum menuSelector menu_selector;
    static uint8_t device_id;
    static uint8_t edit_mode = 0;
    static char val[7];

    void lcpy(int32_t num) {
        strcpy_P(val, PSTR("000000"));
        char tmp[10];
        ultoa(num, tmp, 10);
        int len = strlen(tmp);
        for (int i = 1; i <= len; i++) {
            val[6 - i] = tmp[len - i];
        }
        memcpy(&framebuffer[3][pofs], val, 6);
    }

    if (ApplicationEvents.TaskChanged) {
        load_frame();
        device_id = evt->id;
        menu_selector = BTN_DIGIT0;
        edit_mode = 0;

        //@WARNING: unsafe!
        memcpy_P(&framebuffer[2][pofs], parameters.param_str, strlen_P(parameters.param_str));
        lcpy(parameters.register_value);

        menu_item_highlight(3, pofs, pofs + 1);
        redraw();

        ApplicationEvents.TaskChanged = 0;
    }

    if (ApplicationEvents.ShortKeyPressed) {
        if (!edit_mode) {
            if (++menu_selector > BTN_OK)
                menu_selector = BTN_DIGIT0;
        }

        switch (menu_selector) {
            case BTN_DIGIT0 ... BTN_DIGIT5:
                if (edit_mode) {
                    if (++val[menu_selector] > '9')
                        val[menu_selector] = '0';
                    memcpy(&framebuffer[3][pofs], val, 6);
                    timer0_reset();
                } else {
                    uint8_t sel = menu_selector + pofs;
                    menu_item_highlight(3, sel, sel + 1);
                }
                break;

            case BTN_RESET:
                menu_item_highlight(5, 7, 7 + 7);
                break;

            case BTN_CANCEL:
                menu_item_highlight(7, 1, 9);
                break;

            case BTN_OK:
                menu_item_highlight(7, 17, 21);
                break;
        }
        redraw();

        ApplicationEvents.ShortKeyPressed = 0;
    }

    if (timer0_expired() && edit_mode) {
        /* Blink digit selector */
        static uint8_t flip = 0;
        if (flip ^= 1) {
            uint8_t sel = menu_selector + pofs;
            menu_item_highlight(3, sel, sel + 1);
        }
        redraw();
    }

    if (ApplicationEvents.LongKeyPressed) {
        switch (menu_selector) {
            case BTN_DIGIT0 ... BTN_DIGIT5:
                edit_mode = !edit_mode;
                timer0_preload_ms(400);
                uint8_t sel = menu_selector + pofs;
                menu_item_highlight(3, sel, sel + 1);
                redraw_partial(3);
                break;

            case BTN_RESET:
                lcpy(parameters.default_value);
                redraw_partial(3);
                break;

            case BTN_CANCEL:
                schedule_task(&rs232_task);
                break;

            case BTN_OK:;
                int8_t ok = 0;
                /* Perform write */
                if (parameters.register_type == FLOAT_TYPE) {
                    float reg = atol(val);

                    union {
                        float f;
                        uint16_t r[2];
                    } fcast;
                    fcast.f = reg;

                    modbus_poll_wr(parameters.register_addr, fcast.r[1], device_id);
                    ok = modbus_poll_wr(parameters.register_addr + 1, fcast.r[0], device_id);
                } else if (parameters.register_type == USHORT_TYPE) {
                    uint32_t reg = atol(val);
                    if (reg > parameters.max_value) {
                        reg = parameters.max_value;
                    }
                    ok = modbus_poll_wr(parameters.register_addr, reg, device_id);
                    if (parameters.register_addr == DOT_TIME_AVERAGE) {
                        ok = modbus_poll_wr(DOT_AVERAGE, 0x01, device_id);
                    }
                }
                if (ok)
                    schedule_task(&rs232_task);
                break;
        }
        ApplicationEvents.LongKeyPressed = 0;
    }
}

static void rs232_baud_select_event_cb(task_t * evt) {
    static uint8_t menu_selector;
    static uint8_t device_id;

    if (ApplicationEvents.TaskChanged) {
        load_frame();
        device_id = evt->id;
        menu_selector = baud_index - 1;
        ApplicationEvents.ShortKeyPressed = 1;

        ApplicationEvents.TaskChanged = 0;
    }

    if (ApplicationEvents.ShortKeyPressed) {
        if (++menu_selector > 6)
            menu_selector = 0;

        switch (menu_selector) {
            case 0: // 9600
                menu_item_highlight(2, 2, 6);
                break;
            case 1: // 14440
                menu_item_highlight(2, 9, 14);
                break;
            case 2: // 19200
                menu_item_highlight(2, 15, 20);
                break;
            case 3: // 38400
                menu_item_highlight(3, 2, 7);
                break;
            case 4: // 57600
                menu_item_highlight(3, 9, 14);
                break;
            case 5: // 115200
                menu_item_highlight(3, 15, 21);
                break;
            case 6: // Cancel
                menu_row_highlight(7);
                break;
        }
        redraw();

        ApplicationEvents.ShortKeyPressed = 0;
    }

    if (ApplicationEvents.LongKeyPressed) {
        switch (menu_selector) {
            case 0 ... 5:
                /* Write and exit */
                baud_index = menu_selector;
                uint32_t baudrate = baud_lut[baud_index];
                modbus_poll_wr(DOT_DEVICE_BAUD, baudrate >> 16, device_id);
                modbus_poll_wr(DOT_DEVICE_BAUD + 1, (uint16_t) baudrate, device_id);
                UART_init(baudrate);
                schedule_task(&rs232_settings_task);
                break;
            case 6:
                schedule_task(&rs232_settings_task);
                break;
        }
        ApplicationEvents.LongKeyPressed = 0;
    }
}

static void rs232_settings_event_cb(task_t * evt) {

    enum interfaceSettingsSelector {
        BTN_ID = 1, BTN_BAUDRATE, BTN_NETWORK, BTN_AUTO, BTN_PERIOD, BTN_EXIT
    };

    enum settingsOptionSelector {
        MB_WRITE_OPTION = 0, MB_ID = 2, MB_BAUD, MB_NET_MODE, MB_AUTO_TX, MB_TX_INTERVAL
    };

    static enum interfaceSettingsSelector row_selector;
    static enum settingsOptionSelector option_selector;
    static uint8_t device_id;
    static uint8_t write_reg;

    if (ApplicationEvents.TaskChanged) {
        device_id = 0;
        load_frame();
        row_selector = BTN_ID;
        option_selector = MB_ID;
        menu_row_highlight(row_selector);
        redraw();
        ApplicationEvents.TaskChanged = 0;
    }

    static uint8_t net_mode, auto_send, omni_interval, reg_value;
    uint8_t pos = option_selector - 1;
    uint8_t got_val = 0;
    if (device_id) {
        switch (option_selector) {
            case MB_WRITE_OPTION:
                _delay_ms(50); // skip any incoming reply
                got_val = 0;
                modbus_poll_wr(write_reg, reg_value, device_id);
                option_selector = MB_ID;
                break;
            case MB_ID:
                fb_put_tag(pos, str_id, device_id);
                option_selector++;
                break;
            case MB_BAUD:
                got_val = modbus_poll_r(DOT_DEVICE_BAUD, 2, device_id);
                if (got_val) {
                    uint32_t brate = GetInpRegsLONG(0);
                    fb_put_tag(pos, str_baudrate, brate);
                    option_selector++;
                }
                break;
            case MB_NET_MODE:
                got_val = modbus_poll_r(DOT_OMNI_NET_MODE, 1, device_id);
                if (got_val) {
                    net_mode = GetInpRegsUSHORT(0);
                    fb_put_tag_str(pos, str_net_mode, net_mode == 1 ? (PGM_P) str_on : (PGM_P) str_off);
                    option_selector++;
                }
                break;
            case MB_AUTO_TX:
                got_val = modbus_poll_r(DOT_OMNI_AUTO_SEND, 1, device_id);
                if (got_val) {
                    const char *opt[] = {(PGM_P) str_off, (PGM_P) str_bin_mode, (PGM_P) str_text_mode};
                    auto_send = GetInpRegsUSHORT(0);
                    if (auto_send < 3)
                        fb_put_tag_str(pos, str_auto_send, opt[auto_send]);
                    option_selector++;
                }
                break;
            case MB_TX_INTERVAL:
                got_val = print_ushort_register(DOT_OMNI_INTERVAL, pos, device_id, str_interval);
                if (got_val) {
                    omni_interval = GetInpRegsUSHORT(0);
                    option_selector++;
                }
                break;
            default:
                menu_row_highlight(row_selector);
                redraw();
                option_selector = MB_ID;
                break;

        }
    } else {
        int8_t got_id = modbus_poll_r(DOT_DEVICE_ID, 1, 0);
        if (got_id > 0) device_id = modbus_get_id();
    }

    if (ApplicationEvents.ShortKeyPressed) {
        if (++row_selector > BTN_EXIT)
            row_selector = BTN_ID;
        menu_row_highlight(row_selector);
        redraw();

        ApplicationEvents.ShortKeyPressed = 0;
    }

    if (ApplicationEvents.LongKeyPressed) {
        switch (row_selector) {
            case BTN_ID:
                task_set_id(&rs232_id_select_task, device_id);
                task_set_type(&rs232_id_select_task, evt-> type);
                schedule_task(&rs232_id_select_task);
                break;
            case BTN_BAUDRATE:
                schedule_task(&rs232_baud_select_task);
                break;
            case BTN_NETWORK:
                write_reg = DOT_OMNI_NET_MODE;
                reg_value = net_mode ? 0 : 1;
                option_selector = MB_WRITE_OPTION;
                break;
            case BTN_AUTO:
                write_reg = DOT_OMNI_AUTO_SEND;
                reg_value = auto_send;
                if (++reg_value > 2) reg_value = 0;
                option_selector = MB_WRITE_OPTION;
                break;
            case BTN_PERIOD:
                write_reg = DOT_OMNI_INTERVAL;
                reg_value = omni_interval + 10;
                if (reg_value > 60) reg_value = 10;
                option_selector = MB_WRITE_OPTION;
                break;
            case BTN_EXIT:
                task_set_type(&rs232_task, evt->type);
                schedule_task(&rs232_task);
                break;
        }
        ApplicationEvents.LongKeyPressed = 0;
    }
}

static void main_event_cb(task_t * evt) {

    enum clSelector {
        BTN_BACK, BTN_CALIBRATE
    };

    const uint8_t row = 7;
    static enum clSelector item_selector;
    static bool isCalibrating, lcdLock;
    static enum sensorType sensor_type;
    static uint32_t average_val, average_current;
    static uint8_t device_id;
    static uint8_t refresh_counter; // just to slow things down

    if (ApplicationEvents.TaskChanged) {
        /* First time we entered this event */
        if (!ApplicationFlags.ExternalVoltage)
            boost_enable();
        load_frame();
        menu_item_highlight(row, 0, 7);
        redraw();
        item_selector = BTN_BACK;
        isCalibrating = false;
        refresh_counter = 0xFF;
        ADC_enable(&adc);
        sensor_type = (enum sensorType) evt->type;

        if (sensor_type == SENSOR_FREQUENCY) {
            pullup_enable();
            frequency_measurement_start();
        } else if (sensor_type == SENSOR_ANALOG) {
            pullup_disable();
        } else {
            bool use_rs485 = false;
            UART_init(baud_lut[baud_index]);
            if (sensor_type == SENSOR_RS485) {
                use_rs485 = true;
                rs232_disable();
            } else if (sensor_type == SENSOR_RS232) {
                rs485_disable();
                rs232_enable();
            }
            timer0_preload_ms(50); // how often we resend a packet
            timer0_reset();
            device_id = 0;
            Modbus_enable(use_rs485);
        }

        ApplicationEvents.TaskChanged = 0;
    }

    const uint8_t adc_ready = ADC_ready();

    /* Get frequency or voltage */
    uint16_t val;
    if (sensor_type == SENSOR_FREQUENCY) {
        const uint8_t offset = 2;
        val = get_frequency() + offset;
        average_val = val;
        if (val != 0xFFFF && val > 600) LCD_put_hz(val, 1, 2, 8 + 4);
        else LCD_put_hz(0, 1, 2, 8 + 4);
    } else if (sensor_type == SENSOR_ANALOG) {
        if (adc_ready) {
            val = ADC_convert_sensor_voltage(adc.channel_vin);
            //val = ADC_read(adc.channel_vin);
            average_val = (val >> 2) + average_val - (average_val >> 2);
            LCD_put_volts(average_val, 2, 2, 8 + 4);
        }
    } else {
        if (device_id) {
            uint8_t got_val = 0;
            static uint8_t option_selector = 0;
            static float lvl_percent = 0;
            switch (option_selector) {
                case 0:
                    got_val = modbus_poll_r(DOT_LEVEL_PERCENT, 2, device_id);
                    if (got_val) {
                        lvl_percent = GetInpRegsFLOAT(0);
                        option_selector++;
                    }
                    break;
                case 1:
                    got_val = modbus_poll_r(DOT_OMNI_MAX_N, 1, device_id);
                    if (got_val) {
                        unsigned short level_n = GetInpRegsUSHORT(0);
                        fb_put_tag_at(2, 12, PSTR("ID  :"), device_id);
                        fb_put_tag_at(3, 12, PSTR("Nmax:"), level_n);
                        redraw_partial(2);
                        redraw_partial(3);
                        level_n *= (double) (lvl_percent / 100);
                        LCD_put_large_int(level_n, 0, 0, 2, 0, true);
                        option_selector = 0;
                    }
                    break;
            }
            if (got_val) {
                timer3s_reset();
            } else if (timer3s_expired()) {
                /* Disconnected */
                load_frame();
                redraw();
                device_id = 0;
            }
        } else {
            int8_t got_id = modbus_poll_r(DOT_DEVICE_ID, 1, 0);
            if (got_id > 0) {
                timer3s_reset();
                device_id = modbus_get_id();
            }
        }
    }

    if (isCalibrating) {
        /* Check if calibration has finished */
        int8_t calibrationOK = run_analog_calibration(average_val, false, sensor_type);
        if (calibrationOK == 1) {
            lcdLock = true;
            isCalibrating = false;
            fb_put_str_redraw(str_cal_success, 5);
        } else if (calibrationOK == -1) {
            lcdLock = true;
            isCalibrating = false;
            fb_put_str_redraw(str_cal_error, 5);
        }
    } else if (adc_ready && !lcdLock) {
        /* Display current consumption */
        uint16_t adcVal = ADC_convert_supply_current(adc.channel_shunt);
        average_current = (adcVal >> 2) + average_current - (average_current >> 2);
        if (refresh_counter++ >= 64) {
            refresh_counter = 0;
            char v[5];
            itoa(average_current, v, 10);
            fb_clear_line(5);
            strcpy_P(&framebuffer[5][7], str_current_draw);
            memcpy(&framebuffer[5][7 + 1 + strlen_P(str_current_draw)], v, strlen(v));
            redraw_partial(5);
        }
    }

    if (ApplicationEvents.ShortKeyPressed) {
        item_selector++;

        if (item_selector > BTN_CALIBRATE)
            item_selector = BTN_BACK;
        if (item_selector == BTN_BACK)
            menu_item_highlight(row, 0, 7);
        else if (item_selector == BTN_CALIBRATE) {
            if (sensor_type == SENSOR_ANALOG || sensor_type == SENSOR_FREQUENCY)
                menu_item_highlight(row, 10, 22);
            else
                menu_item_highlight(row, 11, 22);
        }

        lcdLock = false;
        redraw_partial(7);
        ApplicationEvents.ShortKeyPressed = 0;
    }

    if (ApplicationEvents.LongKeyPressed) {
        switch (item_selector) {
            case BTN_BACK:
                Modbus_disable();
                rs232_disable();
                rs485_disable();

                pullup_disable();
                boost_disable();
                frequency_measurement_stop();
                soft_PWM_stop();
                isCalibrating = false;
                schedule_task(&mode_select_task);
                break;
            case BTN_CALIBRATE:
                if (sensor_type == SENSOR_RS232) {
                    task_set_type(&rs232_task, SENSOR_RS232);
                    schedule_task(&rs232_task);
                } else if (sensor_type == SENSOR_RS485) {
                    task_set_type(&rs232_task, SENSOR_RS485);
                    schedule_task(&rs232_task);
                } else {
                    isCalibrating = true;
                    run_analog_calibration(0, true, sensor_type);
                }
                break;
        }
        ApplicationEvents.LongKeyPressed = 0;
    }
}

static void register_callbacks() {
    task_register(&mode_select_task, 0, &frame[FRAME_MODE_SELECT], mode_select_event_cb);
    task_register(&analog_task, SENSOR_ANALOG, &frame[FRAME_MAIN], main_event_cb);
    task_register(&freq_task, SENSOR_FREQUENCY, &frame[FRAME_FREQ], main_event_cb);
    task_register(&rs232_main_task, SENSOR_RS232, &frame[FRAME_RS232_MAIN], main_event_cb);
    task_register(&rs485_main_task, SENSOR_RS485, &frame[FRAME_RS485_MAIN], main_event_cb);
    task_register(&rs232_task, SENSOR_RS232, &frame[FRAME_RS232], rs232_event_cb);
    task_register(&rs232_settings_task, SENSOR_RS232, &frame[FRAME_RS232_SETTINGS], rs232_settings_event_cb);
    task_register(&rs232_id_select_task, 0, &frame[FRAME_ID_SELECT], rs232_id_select_event_cb);
    task_register(&rs232_baud_select_task, 0, &frame[FRAME_BAUD_SELECT], rs232_baud_select_event_cb);
    task_register(&rs232_param_adjust_task, 0, &frame[FRAME_FREQ_ADJUST], rs232_param_adjust_event_cb);
}

static void try_to_sleep() {
    static bool firstBoot = true;
    wdt_reset();
    if (ready_to_sleep() || firstBoot) {
        cli();

        if (firstBoot) {
            /* Reset ports to default state
             * in case bootloader messed things up */
            DDRB = DDRC = DDRD = 0x00;
            PORTB = PORTC = PORTD = 0x00;
            firstBoot = false;
        } else {
            LCD_disable();
            i2c_disable();
        }

        /* Disable peripherals */
        ACSR |= 1 << ACD; // disable comparator
        ADC_disable();
        UART_disable();
        rs232_disable();
        rs485_disable();
        boost_disable();

        /* Set up wake-up interrupt */
        EICRA |= (1 << ISC11); // interrupt on falling edge
        EIMSK |= (1 << INT1); // enable interrupt INT1
        EIFR |= (1 << INTF1); // clear pending interrupt

        /* Prepare to sleep */
        PRR |= ((1 << PRTIM1)
                | (1 << PRTIM0)
                | (1 << PRUSART0)
                | (1 << PRADC)
                | (1 << PRSPI));

        MCUSR = 0; // tell the dog to keep quiet..
        wdt_disable();

        /* Enter sleep mode */
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_enable();
        sei();
        sleep_cpu();

        /* CPU wakes up at this point */
        EIMSK &= ~(1 << INT1);
        PRR &= ~((1 << PRTIM1)
                | (1 << PRTIM0)
                | (1 << PRUSART0)
                | (1 << PRADC));

        /* Initialize peripherals */
        wdt_enable(WDTO_4S);
        GPIO_init();
        key_handler_init();
        i2c_init();
        LCD_init();
        ADC_enable(&adc);

        /* Allow some time to settle and discard key press */
        _delay_ms(300);
        get_key_short(1 << KEY3);
        get_key_long(1 << KEY3);

        /* Check for external voltage */
        uint16_t val = ADC_read(ADC_12V_CHANNEL);
        ApplicationFlags.ExternalVoltage = (val > EXT_VOLTAGE_THRESHOLD) ? 1 : 0;

        /* Disable sleep mode */
        sleep_disable();
        sleep_timer_reset();

        /* Reschedule previous task */
        enum sensorType sensor = (enum sensorType) current_task->type;
        switch (sensor) {
            case SENSOR_ANALOG:
            case SENSOR_FREQUENCY:
                schedule_task(current_task);
                break;
            case SENSOR_RS232:
                task_set_type(&rs232_main_task, SENSOR_RS232);
                schedule_task(&rs232_main_task);
                break;
            case SENSOR_RS485:
                task_set_type(&rs485_main_task, SENSOR_RS485);
                schedule_task(&rs485_main_task);
                break;
            default:
                schedule_task(&mode_select_task);
                break;
        }
    }
}

ISR(INT1_vect) {
    /* Do nothing */
}

/** TODO
 * 1. Handle overflow during frequency measurement
 * 2. Disable IO ports during sleep
 * 3. Decrease LCD refresh rate when
 *    displaying voltage/frequency
 * 4. Remove Modbus initialization in rs232_task
 * 5. Fix press&hold key handler
 */
void main(void) {
    try_to_sleep();
    boot_logo_display();
    create_frames();
    register_callbacks();
    schedule_task(&mode_select_task);

    while (1) {
        handle_keys();
        current_task->event_handler(current_task);
        try_to_sleep();
    }
}
