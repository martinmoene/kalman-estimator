//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef  led_FEATURE_BLINK_MS
# define led_FEATURE_BLINK_MS  200
#endif

#define led_ddr  DDRB
#define led_port PORTB
#define led_bit  PB5

template< typename ...Args >
constexpr uint8_t bitmask( Args&&... args )
{
    return static_cast<uint8_t>( ((1 << args) | ...) );
}

void toggle_led()
{
    led_port ^= bitmask( led_bit );
}

void init_led()
{
    led_port = 0b11111111;
    led_ddr |= bitmask( led_bit );
}

// Init timer 0 for general timing

void init_timer0_1ms()
{
    constexpr auto prescale = 64;
    constexpr auto count1ms = F_CPU_HZ / 1000 / prescale;

    static_assert( count1ms == 250 );

    TCCR0A |= bitmask( WGM01 );         // set the Timer Mode to CTC
    OCR0A   = count1ms;                 // set the count value
    TIMSK0 |= bitmask( OCIE0A );        // set the ISR COMPA vector
    TCCR0B |= bitmask( CS01, CS00 );    // set prescaler to 64 and start the timer
}

// Timer 0 overflow ISR:

ISR( TIMER0_COMPA_vect )
{
    static uint8_t cnt_10ms = 10;

    if ( --cnt_10ms > 0 )
        return ;

    cnt_10ms = 10;

    static uint8_t cnt_100ms = 10;

    if ( --cnt_100ms > 0 )
        return ;

    cnt_100ms = 10;

    static uint8_t cnt_1000ms = 10;

    if ( --cnt_1000ms > 0 )
        return ;

    cnt_1000ms = 10;

    toggle_led();
}

int main()
{
    init_led();
    init_timer0_1ms();

    sei();

    for( ;; )
    {
        asm("nop");
    }
}
