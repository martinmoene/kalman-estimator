//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//
// Show fading PWM on LED.
// Connect PD6 (PWM output) to PD0 (sample PWM)
//

//#include <avr/pgmspace.h>
#include <avr/io.h>
//#include <avr/interrupt.h>

#ifndef  led_FEATURE_BLINK_MS
# define led_FEATURE_BLINK_MS  200
#endif

#define led_ddr    DDRB
#define led_port   PORTB
#define led_bit    PB5

#define pwm0_ddr   DDRD
#define pwm0_port  PORTD
#define pwm0_bit   PD6
#define pwm0_value OCR0A

#define pwm1_ddr   DDRB
#define pwm1_port  PORTB
#define pwm1_bita  PB1
#define pwm1_bitb  PB2
#define pwm1_val_a OCR1A
#define pwm1_val_b OCR1B

#define spl_ddr    DDRD
#define spl_port   PIND   // read pins
#define spl_bit    PD0

#define tstbit( sfr, bit )  (0 != bit_is_set( sfr, bit ) )
#define setbit( sfr, bit )  ( sfr |=  bitmask(bit) )
#define xorbit( sfr, bit )  ( sfr ^=  bitmask(bit) )
#define clrbit( sfr, bit )  ( sfr &= ~bitmask(bit) )

#define tstmsk( sfr, msk )  (0 != (sfr & bit ) )
#define setmsk( sfr, msk )  ( sfr |=  msk )
#define xormsk( sfr, msk )  ( sfr ^=  msk )
#define clrmsk( sfr, msk )  ( sfr &= ~msk )

template< typename ...Args >
constexpr uint8_t bitmask( Args&&... args )
{
    return static_cast<uint8_t>( ((1 << args) | ...) );
}

// Prescaler
// CS0
// 2 1 0  Description
// 0 0 0  No clock source (Timer/Counter stopped)
// 0 0 1  clkI/O/1 (No prescaling)
// 0 1 0  clkI/O/8 (From prescaler)
// 0 1 1  clkI/O/64 (From prescaler)
// 1 0 0  clkI/O/256 (From prescaler)
// 1 0 1  clkI/O/1024 (From prescaler)
// 1 1 0  External clock source on T0 pin. Clock on falling edge.
// 1 1 1  External clock source on T0 pin. Clock on rising edge.

const auto msk_prescaler_t0_0    = 0;
const auto msk_prescaler_t0_1    = bitmask( CS00 );
const auto msk_prescaler_t0_8    = bitmask( CS01 );
const auto msk_prescaler_t0_64   = bitmask( CS01, CS00 );
const auto msk_prescaler_t0_256  = bitmask( CS02 );
const auto msk_prescaler_t0_1024 = bitmask( CS02, CS00 );
const auto msk_prescaler_t0_et0f = bitmask( CS02, CS01 );
const auto msk_prescaler_t0_et0r = bitmask( CS02, CS01, CS00 );

const auto msk_FAST_PWM_MAX = bitmask( WGM01, WGM00 );


void toggle_led()
{
    xorbit( led_port, led_bit );
}

void init_led()
{
    setbit( led_port, led_bit );
    setbit( led_ddr , led_bit );
}

void init_spl()
{
    clrbit( spl_ddr, spl_bit );
}

void init_timer0_pwm()
{
    // set PWM pin (PD6) as an output:
    setbit( pwm0_ddr, pwm0_bit );

    // timer configuration:

    // set non-inverting mode:
    setmsk( TCCR0A, bitmask( COM0A1 ) );

    // set Fast PWM mode:
    setmsk( TCCR0A, bitmask( WGM01, WGM00 ) );

    // set prescaler to 64:
    setmsk( TCCR0B, bitmask( CS01, CS00 ) );
}

void init_timer1_pwm()
{
    // set PWM pins PB1 and PB2 as output:
    setmsk( pwm1_ddr, bitmask( pwm1_bita, pwm1_bitb ) );

    // set non-inverting mode:
    setmsk( TCCR1A, bitmask( COM1A1, COM1B1 ) );

#if 1
    // set 10-bit Fast PWM mode, TOP is 0x3fff (15 kHz)
    setmsk( TCCR1A, bitmask( WGM11, WGM10 ) );
    setmsk( TCCR1B, bitmask( WGM12        ) );

#else
    // set Fast PWM mode using ICR1 as TOP (500 Hz)
    setmsk( TCCR1A, bitmask( WGM11        ) );
    setmsk( TCCR1B, bitmask( WGM12, WGM13 ) );
#endif

    // start the timer with no prescaler
    setbit( TCCR1B, CS10 );
}

void delay_ms( uint16_t ms )
{
    constexpr auto ms_count = F_CPU_HZ / ( 7 /*instr*/ * 1000 ) / 5 ;

    for( auto i = ms; i != 0; --i )
    {
        for( auto k = ms_count; k != 0; --k )
        {
//          asm("nop");
            if ( tstbit( spl_port, spl_bit ) )
                setbit( led_port, led_bit );
            else
                clrbit( led_port, led_bit );
        }
    }
}


int main()
{
    init_led();
    init_timer0_pwm();
    init_timer1_pwm();

    for(;;)
    {
        const auto top = 0x3ffu;

        // fade up:
        for( uint16_t i = 0; i < top; ++i )
        {
            // set duty cycle value
            pwm0_value = i / 4;
            pwm1_val_a = i;
            pwm1_val_b = top - i;
            delay_ms( 2 );
        }

        // fade down:
        for( uint16_t i = top; i > 0; --i )
        {
            // set duty cycle value
            pwm0_value = i / 4;
            pwm1_val_a = i;
            pwm1_val_b = top - i;
            delay_ms( 2 );
        }
    }
}
