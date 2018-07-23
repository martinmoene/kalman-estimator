//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#include "mcu/atmega328.hpp"

using namespace mcu::atmega328;

adc::result_t adc_result = 0;

int main()
{
    // combine operations on admux register (note comma):
    // can omit adcsra::
    adc::admux::reference( adc::voltage_ref::avcc ),
    adc::admux::adjust   ( adc::adjustment::right ),
    adc::admux::channel  ( adc::input::ch7        );

    // combine operations on adcsra register (note comma):
    // can omit adcsra::
    adc::adcsra::enable(),
    adc::adcsra::prescale( adc::factor::x128 ),
    adc::adcsra::start_conversion();

    // wait for conversion result to become available:
    while( !adc::interrupt_flag() )
        ;

    adc_result = adc::result();

    adc::clear_interrupt_flag();

    led::enable();

    for ( ;; )
    {
        led::toggle();

//#if led_FEATURE_BLINK_MS
//        delay_ms( led_FEATURE_BLINK_MS );
//#endif
    }
}

// Compilation with avr-g++ -O2 gives:
//
// 	.file	"atmega328.cpp"
// __SP_H__ = 0x3e
// __SP_L__ = 0x3d
// __SREG__ = 0x3f
// __tmp_reg__ = 0
// __zero_reg__ = 1
// 	.text
// 	.section	.text.startup.main,"ax",@progbits
// .global	main
// 	.type	main, @function
// main:
// /* prologue: function */
// /* frame size = 0 */
// /* stack size = 0 */
// .L__stack_usage = 0
// 	lds r24,124
// 	andi r24,lo8(16)
// 	ori r24,lo8(71)
// 	sts 124,r24
// 	lds r24,122
// 	ori r24,lo8(-57)
// 	sts 122,r24
// .L2:
// 	lds r24,122
// 	sbrs r24,4
// 	rjmp .L2
// 	lds r24,120
// 	lds r25,120+1
// 	sts adc_result+1,r25
// 	sts adc_result,r24
// 	lds r24,122
// 	ori r24,lo8(16)
// 	sts 122,r24
// 	sbi 0x4,5
// 	ldi r25,lo8(32)
// .L3:
// 	in r24,0x5
// 	eor r24,r25
// 	out 0x5,r24
// 	rjmp .L3
// 	.size	main, .-main
// .global	adc_result
// 	.section	.bss.adc_result,"aw",@nobits
// 	.type	adc_result, @object
// 	.size	adc_result, 2
// adc_result:
// 	.zero	2
// 	.ident	"GCC: (GNU) 8.1.0"
// .global __do_clear_bss
//
