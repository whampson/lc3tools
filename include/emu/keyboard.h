#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <lc3.h>

#define KBD_IRQ     4

struct lc3kbd {
    lc3word kbsr;   /* status register */
    lc3word kbdr;   /* data register */
};

void kbd_reset(void);

void kbd_tick(void);

lc3word get_kbsr(void);

void set_kbsr(lc3word value);

lc3word get_kbdr(void);

void set_kbdr(lc3word value);

#endif /* __KEYBOARD_H */
