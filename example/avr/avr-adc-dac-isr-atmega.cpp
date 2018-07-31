//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "mcu/atmega328.hpp"

// configuration:

namespace config
{
    constexpr auto led_blink = 3;
    constexpr auto led_pin   = 5;
    constexpr auto led_port  = mcu::atmega328::port::B;
}

// clamped value:

template< typename T >
constexpr T clamp( T v, T lo, T hi )
{
    return v < lo ? lo : hi < v ? hi : v;
}

// Onboard LED at PB5 (Arduino pin 13):

struct led : mcu::atmega328::led<config::led_port, config::led_pin>
{
    static inline void init() { enable(); }
};

// PWM DAC output a of timer 1 at 10-bit resolution (tc1):

namespace dac
{
    using dac_t = mcu::atmega328::tc1_pwm_on_a_pb1;

    // dac value type:

    using value_type = dac_t::value_type;

    // init timer 1 for PWM-dac timing:

    inline void init()
    {
        dac_t::init();
    }

    // output DAC value:

    inline void write( value_type value )
    {
        dac_t::write( clamp( value, dac_t::value_min, dac_t::value_max ) );
    }
};

// ADC:

namespace adc
{
    // result type and range:

    using mcu::atmega328::adc::conversion::result_t;
    using mcu::atmega328::adc::conversion::result_min;
    using mcu::atmega328::adc::conversion::result_max;

    volatile result_t adc_result;

    // setup ADC:

    inline void init()
    {
        using namespace mcu::atmega328::adc;

        // register admux:
        set( input::reference::avcc )
        , set( input::channel::ch0 )
            ;

        // register adccsrb:
        set( trigger::on::free_run );

        // register adcsra:
        set( prescale::factor::x128 )
        , set( enable::adc::on )
        , set( enable::interrupt::on )
        , set( enable::auto_trigger::on )
        , conversion::start()
            ;
    }

    // atomic read of ADC conversion result:

    inline result_t result()
    {
        return mcu::atomic( adc_result );
    }

    // ADC conversion ready ISR:

    ISR( ADC_vect )
    {
        mcu::atmega328::core::scoped_sreg _;

        adc_result = mcu::atmega328::adc::conversion::result();
    }
}

// Timing mailboxes:

namespace mbx
{
    // mailbox with automatic reset:

    struct mbx
    {
        volatile bool flag = false;

        void operator=( bool x )
        {
            flag = x;
        }

        explicit operator bool()
        {
            struct scoped_reset
            {
                volatile bool & x;

                scoped_reset( volatile bool & x_) : x( x_ ) {}

                ~scoped_reset() { x = false; }
            };

            if ( flag )
            {
                scoped_reset _( flag );
                return true;
            }
            return false;
        }
    };

    mbx mbx_1ms;
    mbx mbx_10ms;
    mbx mbx_100ms;
    mbx mbx_1000ms;
}

// Heartbeat in 1 ms, 10 ms, 100 ms and 1 s on timer 0 (tc0):

namespace beat
{
    // setup timer 0 for heartbeat:

    inline void init()
    {
        using namespace mcu::atmega328::tc0;

        constexpr auto prescale = 64;
        constexpr auto count1ms = F_CPU_HZ / 1000 / prescale;

        static_assert( count1ms == 250 );

        // register timsk0:
        set( enable::interrupt::on::match_a::on );
        set( value::compare::a( count1ms ) );

        // registers tccr1a, tccr10
        set( output::waveform::ctc )
        , set( input::clock::x64 )
            ;
    }

    // timer 0 ISR:

    ISR( TIMER0_COMPA_vect )
    {
        using namespace mbx;

        mcu::atmega328::core::scoped_sreg _;

        static uint8_t cnt_10ms = 10;
        static uint8_t cnt_100ms = 10;
        static uint8_t cnt_1000ms = 10;

        mbx_1ms = true;

        if ( --cnt_10ms > 0 )
            return ;

        cnt_10ms = 10;
        mbx_10ms = true;

        if ( --cnt_100ms > 0 )
            return ;

        cnt_100ms = 10;
        mbx_100ms = true;

        if ( --cnt_1000ms > 0 )
            return ;

        cnt_1000ms = 10;
        mbx_1000ms = true;
    }
}

// main program:

int main()
{
    // setup hardware:

    led::init();
    adc::init();
    dac::init();
    beat::init();

    // enable interrupts :

    sei();

    // quickly blink LED a few times:

    using namespace mbx;

    for( auto i = uint8_t{ 2 * config::led_blink }; i != 0; --i )
    {
        while( ! mbx_100ms )
            ;

        led::toggle();
    }

    // continuously:
    // - each 100ms measure adc and put LEDon when adc result is in upper half;
    // - each second put LED off;
    // - each second pass-on adc result to pwm-mode dac on tc1.

    for(;;)
    {
        if ( mbx_1ms )
        {
        }

        if ( mbx_10ms )
        {
        }

        if ( mbx_100ms )
        {
            if ( adc::result() > adc::result_max / 2 )
                led::on();
        }

        if ( mbx_1000ms )
        {
            led::off();

            dac::write( adc::result() );
        }
    }
}

