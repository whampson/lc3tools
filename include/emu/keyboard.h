#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <lc3.h>

/*
 * Keyboard IRQ line (interrupt priority).
 */
#define KBD_IRQ     4

/*
 * Keyboard state.
 */
struct lc3kbd {
    lc3word kbsr;   /* status register */
    lc3word kbdr;   /* data register */
};

/*
 * Reset the keyboard state.
 */
void kbd_reset(void);

/*
 * Execute one clock cycle on the keyboard.
 */
void kbd_tick(void);

/*
 * Get the value of the Keyboard Status Register.
 *
 * @return      current value in KBSR
 */
lc3word get_kbsr(void);

/*
 * Set the value of the Keyboard Status Register.
 *
 * @param value the value to put in KBSR
 */
void set_kbsr(lc3word value);

/*
 * Get the value of the Keyboard Data Register.
 *
 * @return      current value in KBDR
 */
lc3word get_kbdr(void);

/*
 * Set the value of the Keyboard Data Register.
 *
 * @param value the value to put in KBDR
 */
void set_kbdr(lc3word value);

#endif /* __KEYBOARD_H */
