//  speed.c
//  2018-06-28  Markku-Juhani O. Saarinen <markku@teserakt.io>

//  (c) 2018 Copyright Teserakt AG

//  Test / benchmark code

#include <avr/io.h>
#include <avr/interrupt.h>
#include "ser_print.h"

#include "sha3.h"

/* === speed-testing code === */

uint64_t tick_ticks;
static uint8_t tick_init_done = 0;

// intialize timers

static void tick_init(void)
{
    tick_ticks = 0;
#ifdef __AVR_ATmega128__
    TCCR1B = (1 << CS12);
    TIMSK |= (1 << TOIE1);
#else
    TCCR0B = (1 << CS00);
    TCCR1B = (1 << CS12);
    TIMSK1 |= (1 << TOIE1);
#endif
    TCNT0 = 0;
    TCNT1 = 0;
    sei(); // Enable global interrupts
    tick_init_done = 1;
}

// interrupt handler on TIMER1 overflow

ISR(TIMER1_OVF_vect)
{
    tick_ticks += (1UL << 24);
}

unsigned long long tick_cycles(void)
{
    if (!tick_init_done)
        tick_init();

    return tick_ticks | (((uint64_t) TCNT1) << 8) | ((uint64_t) TCNT0);
}

void print_block(uint8_t st[200])
{
    uint8_t i, j;

    j = 0;
    for (i = 0; i < 200; i++) {
        ser_hex8(st[i]);
        if ((i & 7) == 7) {
            j++;
            if (j == 5) {
                ser_write('\n');
                j = 0;
            } else {
                ser_write(' ');
            }
        }
    }
    ser_write('\n');
}

// in test_sha3.c
int test_sha3();
int test_shake();

int main(void)
{
    size_t i, run;
    uint64_t t;

    ser_print("test_sha3() = ");
    ser_hex8(test_sha3());
    ser_print("\ntest_shake() = ");
    ser_hex8(test_shake());
    ser_write('\n');

    // time the f function

    tick_init();

    for (run = 0; run < 10; run++) {

        ser_print("Run #");
        ser_hex8(run);
        ser_print(" ");

        t = tick_cycles();
        for (i = 1 << run; i > 0; i--) {
            keccak_f1600(&common_sha3_ctx, 24);
            keccak_f1600(&common_sha3_ctx, 24);
            keccak_f1600(&common_sha3_ctx, 24);
            keccak_f1600(&common_sha3_ctx, 24);
        }
        t = tick_cycles() - t;
        t >>= run + 2;

        ser_dec64(t);
        ser_print(" ticks / block\n");
    }

    ser_end();

    return 0;
}

