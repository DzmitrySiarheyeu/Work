#include "Button.h"
#include <avr/interrupt.h>

static volatile uint8_t key_state; // debounced and inverted key state: bit = 1: key pressed
static volatile uint8_t key_press; // key press detect
static volatile uint8_t key_rpt; // key long press and repeat
static uint8_t ct0, ct1, rpt;

void debounce_keys() {
    uint8_t i;

    TCNT0 = (uint8_t) (int16_t)-(F_CPU / 1024 * 10e-3 + 0.5); // preload for 10ms
    //TCNT0 = (uint8_t) (int16_t)-(F_CPU / 12288 * 10e-3 + 0.5); // preload for 10ms

    i = key_state ^ ~KEY_PIN; // key changed ?
    ct0 = ~(ct0 & i); // reset or count ct0
    ct1 = ct0 ^ (ct1 & i); // reset or count ct1
    i &= ct0 & ct1; // count until roll over ?
    key_state ^= i; // then toggle debounced state
    key_press |= key_state & i; // 0->1: key press detect

    if ((key_state & REPEAT_MASK) == 0) // check repeat function
        rpt = REPEAT_START; // start delay
    if (--rpt == 0) {
        rpt = REPEAT_NEXT; // repeat delay
        key_rpt |= (key_state & REPEAT_MASK);
    }
}

uint8_t get_key_press(uint8_t key_mask) {
    cli(); // read and clear atomic !
    key_mask &= key_press; // read key(s)
    key_press ^= key_mask; // clear key(s)
    sei();
    return key_mask;
}

uint8_t get_key_rpt(uint8_t key_mask) {
    cli(); // read and clear atomic !
    key_mask &= key_rpt; // read key(s)
    key_rpt ^= key_mask; // clear key(s)
    sei();
    return key_mask;
}

uint8_t get_key_short(uint8_t key_mask) {
    cli(); // read key state and key press atomic !
    return get_key_press(~key_state & key_mask);
}

uint8_t get_key_long(uint8_t key_mask) {
    return get_key_press(get_key_rpt(key_mask));
}

uint8_t get_key_rpt_l(uint8_t key_mask) {
    return get_key_rpt(~key_press & key_mask);
}

void key_handler_init() {
    KEY_DR &= ~ALL_KEYS;
    KEY_PORT |= ALL_KEYS;
    TCCR0A = 0x00;
    TCCR0B = (1 << CS02) | (1 << CS00);
    TIMSK0 |= 1 << TOIE0; // enable timer interrupt
    ct0 = 0;
    ct1 = 0;
    rpt = 0;
}
