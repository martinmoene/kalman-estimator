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

namespace led
{
    using led_t = mcu::atmega328::led<config::led_port, config::led_pin>;

    inline auto on()     { led_t::on();     }
    inline auto off()    { led_t::off();    }
    inline auto toggle() { led_t::toggle(); }
    inline void init()   { led_t::enable(); }
}

// PWM DAC output a of timer 1 at 10-bit resolution (tc1):

namespace dac
{
    using dac_t = mcu::atmega328::tc1_pwm_on_a_pb1;

    // result type:

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

    using value_type = mcu::atmega328::adc::result_t;

    using mcu::atmega328::adc::result_min;
    using mcu::atmega328::adc::result_max;

    volatile value_type adc_result;

    // setup ADC:

    inline void init()
    {
        namespace ai = mcu::atmega328::adc;

        // register admux:
        ai::reference( ai::voltage_ref::avcc )
        , ai::channel( ai::input::ch0 )
            ;

        // register adccsrb:
        ai::trigger_source( ai::auto_trigger_source::free_run );

        // register adcsra:
        ai::prescale( ai::factor::x128 )
        , ai::auto_trigger( true )
        , ai::enable( true )
        , ai::enable_interrupt( true )
        , ai::start_conversion()
            ;
    }

    // atomic read of ADC conversion result:

    inline value_type read()
    {
        return mcu::atomic( adc_result );
    }

    // ADC conversion ready ISR:

    ISR( ADC_vect )
    {
        mcu::atmega328::core::scoped_sreg _;

        adc_result = mcu::atmega328::adc::result();
    }
}

// Timing mailboxes:

namespace mbx
{
    // Timer 0 overflow ISR:

    volatile bool mbx_1ms = false;
    volatile bool mbx_10ms = false;
    volatile bool mbx_100ms = false;
    volatile bool mbx_1000ms = false;
}

// Heartbeat in 1 ms, 10 ms, 100 ms and 1 s on timer 0 (tc0):

namespace beat
{
    // setup timer 0 for heartbeat:

    inline void init()
    {
        using namespace mcu::atmega328;

        constexpr auto prescale = 64;
        constexpr auto count1ms = F_CPU_HZ / 1000 / prescale;

        static_assert( count1ms == 250 );

        // register timsk1:
//      tc0::interrupt_on_a( true )
//      tc0::compare_a_with( count1ms )
        tc0::enable_output_compare_a_match_interrupt( true );
        tc0::output_compare_a( count1ms );

        // registers tccr1a, tccr1b
        tc0::waveform( tc0::waveforms::ctc /*_ocr0a*/ )
        , tc0::clock ( tc0::clocks::clk_64 )
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

    // quickly blink LED 3 times:

    for( auto i = 2 * config::led_blink; i != 0; --i )
    {
        while( ! mbx::mbx_100ms )
            ;

        mbx::mbx_100ms = false;
        led::toggle();
    }

    // continuously:
    // - each 100ms measure adc and put LEDon when adc result is in upper half;
    // - each second put LED off;
    // - each second pass-on adc result to pwm-mode dac on tc1.

    for(;;)
    {
        using namespace mbx;

        if ( mbx_1ms )
        {
            mbx_1ms = false;
        }

        if ( mbx_10ms )
        {
            mbx_10ms = false;
        }

        if ( mbx_100ms )
        {
            mbx_100ms = false;

            if ( adc::read() > adc::result_max / 2 )
                led::on();
        }

        if ( mbx_1000ms )
        {
            mbx_1000ms = false;

            led::off();

            dac::write( adc::read() );
        }
    }
}

