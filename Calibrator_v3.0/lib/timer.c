#include "timer.h"
#include "Button.h"
#include <avr/interrupt.h>

static uint16_t sleep_counter = 0;
static uint16_t counter_3s = 0;
static uint16_t preload = 0;
static uint16_t ctr = 0;

static uint8_t beeper_mode = 0;

static struct {
    unsigned char SleepScheduled : 1;
    unsigned char DelayedSleepScheduled : 1;
    unsigned char TimerExpired : 1;
    unsigned char Timer3sExpired : 1;
} TimerEvents;

void schedule_delayed_sleep() {
    TimerEvents.DelayedSleepScheduled = 1;
    sleep_counter = 0;
}

/**
 * Timer callback function (executed every 10ms).
 */
static void timer0_callback() {
    if (TimerEvents.SleepScheduled)
        sleep_counter = 0;
    else {
        sleep_counter++;
        if (sleep_counter >= SLEEP_TIMEOUT) {
            sleep_counter = 0;
            TimerEvents.SleepScheduled = 1;
        } else if (TimerEvents.DelayedSleepScheduled && sleep_counter >= DELAYED_SLEEP_TIMEOUT) {
            sleep_counter = 0;
            TimerEvents.SleepScheduled = 1;
            TimerEvents.DelayedSleepScheduled = 0;
        }
    }

    if (!TimerEvents.Timer3sExpired) {
        if (counter_3s++ >= TIMEOUT_3SEC) {
            counter_3s = 0;
            TimerEvents.Timer3sExpired = 1;
        }
    }

    if (!TimerEvents.TimerExpired) {
        if (ctr++ >= preload) {
            ctr = 0;
            TimerEvents.TimerExpired = 1;
        }
    }
}

void sleep_timer_reset() {
    sleep_counter = 0;
    TimerEvents.SleepScheduled = 0;
    TimerEvents.DelayedSleepScheduled = 0;
}

uint8_t ready_to_sleep() {
    return TimerEvents.SleepScheduled;
}

void timer0_reset() {
    TimerEvents.TimerExpired = 0;
    ctr = 0;
}

void timer0_preload_ms(uint16_t ms) {
    preload = ms / 10;
}

uint8_t timer0_expired() {
    if (TimerEvents.TimerExpired) {
        timer0_reset();
        return 1;
    }
    return 0;
}

void timer3s_reset() {
    TimerEvents.Timer3sExpired = 0;
    counter_3s = 0;
}

uint8_t timer3s_expired() {
    if (TimerEvents.Timer3sExpired) {
        timer3s_reset();
        return 1;
    }
    return 0;
}

void make_annoying_beep(uint8_t type) {
    TCCR0A = (1 << COM0A0) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS01) | (1 << CS00) | (1 << WGM02);

    if (type == BEEP_SHORT) {
        beeper_mode = BEEP_SHORT;
        OCR0A = 128;
    } else {
        OCR0A = 200;
        beeper_mode = BEEP_DOUBLE;
    }
}

ISR(TIMER0_OVF_vect) {
    static uint16_t ctr = 0;
    if (beeper_mode == BEEP_SHORT) {
        if (ctr++ >= 512) {
            ctr = 0;
            beeper_mode = 0;

            TCCR0B = (1 << CS02) | (1 << CS00);
            TCCR0A = 0x00;
        } else if (ctr >= 128) {
            OCR0A = 32;
        } else if (ctr >= 64) {
            OCR0A = 64;
        }
    } else if (beeper_mode == BEEP_DOUBLE) {
        if (ctr++ >= 1024 * 4) {
            ctr = 0;
            beeper_mode = 0;

            TCCR0B = (1 << CS02) | (1 << CS00);
            TCCR0A = 0x00;
        } else if (ctr >= 512) {
            OCR0A = 29;
        }
    } else {
        /* every 10ms */
        timer0_callback();
        debounce_keys();
    }
}
