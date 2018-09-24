// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//
// Read adc, channel 0 and put LED on if ADC value is greater than Vcc/2.
//
// Configure ADC for free running sampling, generating an interrupt
// on conversion complete. In the interrupt service routine, save the
// 16-bit conversion value and its 8 most significant bits.
//
// In main(), update the status of the LED on PB5 from the 16-bit conversion
// result and from the 8-bit conversion result. Reading of the conversion
// values is guarded by interrupt of for types than are larger than one byte.
//

//#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define led_ddr   DDRB
#define led_port  PORTB
#define led_bit   PB5

template< typename ...Args >
constexpr uint8_t bitmask( Args&&... args )
{
    return static_cast<uint8_t>( ((1 << args) | ...) );
}

template< typename Sfr >
bool tstbit( Sfr && sfr, uint8_t bit )
{
    return bit_is_set( sfr, bit );
}

template< typename Sfr >
void setbit( Sfr && sfr, uint8_t bit )
{
    sfr |= bitmask(bit);
}

template< typename Sfr >
void xorbit( Sfr && sfr, uint8_t bit )
{
    sfr ^= bitmask(bit);
}

template< typename Sfr >
void clrbit( Sfr && sfr, uint8_t bit )
{
    sfr &= ~bitmask(bit);
}

template< typename Sfr >
void putbit( Sfr && sfr, uint8_t bit, bool on )
{
    if ( on )
        setbit( sfr, bit);
    else
        clrbit( sfr, bit);
}

template< typename Sfr >
void tstmsk( Sfr && sfr, uint8_t msk )
{
    return 0 != ( sfr & msk );
}

template< typename Sfr >
void setmsk( Sfr && sfr, uint8_t msk )
{
    sfr |= msk;
}

template< typename Sfr >
void xormsk( Sfr && sfr, uint8_t msk )
{
    sfr ^= msk;
}

template< typename Sfr >
void clrmsk( Sfr && sfr, uint8_t msk )
{
    sfr &= ~msk;
}

// scoped interrupt guard;
// simplification: interrupts are assumed to be on

struct guard
{
    ~guard() { sei(); }
    guard()  { cli(); }
};

// read multiple-byte values guarded by interrupts off

template< typename T >
T guarded( T const & value )
{
    if constexpr( sizeof(T) > 1 ) { guard _; return value; }
    else                          {          return value; }
}

void init_adc()
{
    // adc input register, use channel 0:
    clrmsk( ADMUX, bitmask( MUX3, MUX2, MUX2, MUX0 ) );

    // adc input register, reference and data configuration
    setmsk( ADMUX, bitmask(
        REFS0   // use AVcc as the reference:
//      , ADLAR //right adjust for 8 bit resolution
    ));

    // adc control register:
    setmsk( ADCSRA, bitmask(
        // 128 prescale for 16Mhz
        ADPS2, ADPS1, ADPS0
        , ADATE // enable adc auto triggger,
        , ADEN  // enable adc
        , ADIE  // enable adc interrupt
        , ADSC  // start first adc conversion
    ));

    // Special Modes of Operation for the ATmega168/328:
    // 0 for free running mode:
    ADCSRB = 0;
}

void init_led()
{
    setbit( led_port, led_bit );
    setbit( led_ddr , led_bit );
}

void led( bool on )
{
    setbit( led_port, led_bit );
}

constexpr uint16_t adc_level  = 512u;
constexpr uint16_t adc_level8 = 128u;

// Updated from isr:
volatile uint16_t adc_value;
volatile uint8_t  adc_value8;

int main()
{
    init_adc();
    init_led();

    sei();

    for(;;)
    {
        putbit( led_port, led_bit, guarded( adc_value  ) > adc_level  );
        putbit( led_port, led_bit, guarded( adc_value8 ) > adc_level8 );
    }
}

ISR( ADC_vect )
{
    adc_value  = ADC;
    adc_value8 = adc_value >> 2;
}
