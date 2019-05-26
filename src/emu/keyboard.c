#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>

#include <lc3.h>
#include <keyboard.h>
#include <pic.h>

#define RD()        (kbd.kbsr & KBSR_RD)
#define SET_RD(x)   (kbd.kbsr = (x)?(kbd.kbsr|KBSR_RD):(kbd.kbsr&~KBSR_RD))

#define IE()        (kbd.kbsr & KBSR_IE)
#define SET_IE(x)   (kbd.kbsr = (x)?(kbd.kbsr|KBSR_IE):(kbd.kbsr&~KBSR_IE))

static struct lc3kbd kbd;

static int kbhit(void);
static int getch(void);

void kbd_reset(void)
{
    kbd.kbsr = 0;
    kbd.kbdr = 0;
    SET_IE(1);
}

void kbd_tick(void)
{
    unsigned char c;

    if (kbhit()) {
        c = getch();
        if (c == 3) {
            printf("CTRL+C pressed!\r\n");
            exit(127);
        }
        kbd.kbdr = c;
        SET_RD(1);
    }

    if (RD() && IE()) {
        raise_irq(KBD_IRQ);
    }
}

lc3word get_kbsr(void)
{
    return kbd.kbsr;
}

void set_kbsr(lc3word value)
{
    kbd.kbsr = value;
}

lc3word get_kbdr(void)
{
    return kbd.kbdr;
}

void set_kbdr(lc3word value)
{
    kbd.kbdr = value;
}

static int kbhit(void)
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    return select(1, &fds, NULL, NULL, &tv);
}

static int getch(void)
{
    int r;
    unsigned char c;

    if ((r = read(STDIN_FILENO, &c, sizeof(unsigned char))) < 0) {
        return r;
    }

    return c;
}
