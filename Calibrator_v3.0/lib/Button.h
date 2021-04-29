/************************************************************************/
/*                                                                      */
/*                      Debouncing 8 Keys                               */
/*                      Sampling 4 Times                                */
/*                      With Repeat Function                            */
/*                                                                      */
/*              Author: Peter Dannegger                                 */
/*                      danni@specs.de                                  */
/*                                                                      */
/************************************************************************/
#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>
#include <avr/io.h>

#define KEY_PORT        PORTD
#define KEY_DR          DDRD
#define KEY_PIN         PIND
#define KEY0            PIND0
#define KEY1            PIND1
#define KEY2            PIND2
#define KEY3            PIND3

//#define ALL_KEYS        ((1 << KEY0) | (1 << KEY1) | (1 << KEY2) | (1 << KEY3))
//#define REPEAT_MASK     ((1 << KEY0) | (1 << KEY1) | (1 << KEY2) | (1 << KEY3))
#define ALL_KEYS        ((1 << KEY3))
#define REPEAT_MASK     ((1 << KEY3))
#define REPEAT_START    50                              // after 500ms
#define REPEAT_NEXT     20                              // every 200ms

/**
 * check if a key has been pressed. Each pressed key is reported
 * only once.
 * @param key_mask
 * @return 
 */
uint8_t get_key_press(uint8_t key_mask);

/**
 * check if a key has been pressed long enough such that the
 * key repeat functionality kicks in. After a small setup delay
 * the key is reported being pressed in subsequent calls
 * to this function. This simulates the user repeatedly
 * pressing and releasing the key.
 * @param key_mask
 * @return 
 */
uint8_t get_key_rpt(uint8_t key_mask);

uint8_t get_key_short(uint8_t key_mask);

uint8_t get_key_long(uint8_t key_mask);

uint8_t get_key_rpt_l(uint8_t key_mask);

void key_handler_init();

void debounce_keys();

#endif /* BUTTON_H */
