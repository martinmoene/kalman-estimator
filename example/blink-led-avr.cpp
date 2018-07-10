//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//#include <avr/pgmspace.h>
#include <avr/io.h>
//#include <avr/interrupt.h>

#ifndef  led_FEATURE_BLINK_MS
# define led_FEATURE_BLINK_MS  200
#endif

#define led_ddr  DDRB
#define led_port PORTB
#define led_bit  PB5

constexpr uint8_t bitmask( uint8_t pin )
{
    return static_cast<uint8_t>( 1 << pin );
}

void delay_ms( uint16_t ms )
{
    constexpr auto ms_count = F_CPU_HZ / ( 7 /*instr*/ * 1000 );

    for( auto i = ms; i != 0; --i )
        for( auto k = ms_count; k != 0; --k )
            asm("nop");
}

int main()
{
    led_port = 0b11111111;
    led_ddr |= bitmask( led_bit );

    for( ;; )
    {
#if led_FEATURE_BLINK_MS
        delay_ms( led_FEATURE_BLINK_MS );
#endif
        led_port ^= bitmask( led_bit );
    }
}
