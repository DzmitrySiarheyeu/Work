#ifndef TIMER_H
#define TIMER_H
#include <stdint.h>

#define SLEEP_TIMEOUT           18000  // 3 min
#define TIMEOUT_3SEC            300    // 3 sec
#define DELAYED_SLEEP_TIMEOUT   100    // 1 sec

enum beepMode {
    BEEP_SHORT = 1, BEEP_DOUBLE = 2
};

/**
 * Reset timer and clear TimerExpired flag.
 */
void timer0_reset();

/**
 * Reset timer and clear Timer3sExpired flag.
 */
void timer3s_reset();

/**
 * Set how often TimerExpired event is triggered.
 * @param ms preload time
 */
void timer0_preload_ms(uint16_t ms);

/**
 * Reset timer and clear sleep events.
 */
void sleep_timer_reset();

/**
 * After calling this function SleepScheduled event
 * will be triggered after SLEEP_TIMEOUT.
 */
void schedule_delayed_sleep();

/**
 * Check state of TimerExpired event. Timer 
 * will be automatically reset on expiration.
 * @return 1 - timer expired; 0 - timer still running
 */
uint8_t timer0_expired();

/**
 * Check state of Timer3sExpired event. Timer 
 * will be automatically reset on expiration.
 * @return 1 - timer expired; 0 - timer still running
 */
uint8_t timer3s_expired();

/**
 * Check state of SleepScheduled event.
 * @return 1 - ready; 0 - not ready
 */
uint8_t ready_to_sleep();

/**
 * note: this will pause key handler and timer0 callback.
 * @param mode BEEP_SHORT - annoying beep;
 *             BEEP_DOUBLE - twice as annoying
 */
void make_annoying_beep(enum beepMode mode);

#endif /* TIMER_H */
