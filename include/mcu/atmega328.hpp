//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCU_ATMEGA328_HPP_INCLUDED
#define MCU_ATMEGA328_HPP_INCLUDED

#include "mcu/register.hpp"

namespace mcu {
namespace atmega328 {

//
// MCU control, status and peripherals:
//

// Convert to uint8_t:

template< typename Id >
constexpr auto to_integral( Id id )
{
    return static_cast< uint8_t >( id );
}


// 12. AVR Memories
//
// 12.6.1 Accessing 16-Bit Registers
// 12.6.2 EEARH, EEARL: EEPROM Address Register Low and High Byte
// 12.6.3 EEDR : EEPROM Data Register
// 12.6.4 EECR : EEPROM Control Register
// 12.6.5 GPIOR2: General Purpose I/O Register 2
// 12.6.6 GPIOR1: General Purpose I/O Register 1
// 12.6.7 GPIOR0: General Purpose I/O Register 0

namespace mem
{
    using eeprom_data_t    = uint8_t;
    using eeprom_address_t = uint16_t;

    static constexpr address_t eear_addr   = 0x41;
    static constexpr address_t eedr_addr   = 0x40;
    static constexpr address_t eecr_addr   = 0x3f;

    static constexpr address_t gpior2_addr = 0x4b;
    static constexpr address_t gpior1_addr = 0x4a;
    static constexpr address_t gpior0_addr = 0x3e;

    // EEPROM programming mode, see 8.6.3 eecr

    enum class eeprom_programming_modes
    {
        erase_write = 0     // 3.4 ms, atomic operation
        , erase_only        // 1.8 ms
        , write_only        // 1.8 ms
        , reserved3         // -
    };

    using eear = register_t< eeprom_address_t, rw_t, eear_addr >;
    using eedr = register_t< eeprom_data_t   , rw_t, eedr_addr >;

    inline auto eeprom_address()
    {
        return eear::read();
    }

    inline auto eeprom_address( eeprom_address_t address )
    {
        eear::write( address );
    }

    inline auto eeprom_data()
    {
        return eedr::read();
    }

    inline auto eeprom_data( eeprom_data_t data )
    {
        eedr::write( data );
    }

    // 8.6.3 - EECR: The EEPROM Control Register
    //
    // 5,4: EEPM1 and EEPM0: EEPROM Programming Mode Bits
    // 3: EERIE: EEPROM Ready Interrupt Enable
    // 2: EEMPE: EEPROM Master Write Enable
    // 1: EEPE: EEPROM Write Enable
    // 0: EERE: EEPROM Read Enable

    namespace eecr
    {
        using whole = register8_t< rw_t, eecr_addr >;
        using eepm  = bitfield8_t< rw_t, eecr_addr , EEPM1, EEPM0 >;
        using eerie = bitfield8_t< rw_t, eecr_addr , EERIE        >;
        using eempe = bitfield8_t< rw_t, eecr_addr , EEMPE        >;
        using eepe  = bitfield8_t< rw_t, eecr_addr , EEPE         >;
        using eere  = bitfield8_t< rw_t, eecr_addr , EERE         >;

        inline auto eeprom_programming_mode()
        {
            return eeprom_programming_modes{ eepm::read() };
        }

        inline auto eeprom_programming_mode( eeprom_programming_modes mode )
        {
            return eepm::write_lazy( to_integral(mode) );
        }

        inline auto enabled_eeprom_ready_interrupt()
        {
            return eerie::read();
        }

        inline auto enable_eeprom_ready_interrupt( bool on )
        {
            return eerie::write_lazy( on );
        }

        inline auto enabled_eeprom_master_write()
        {
            return eempe::read();
        }

        inline auto enable_eeprom_master_write( bool on )
        {
            return eempe::write_lazy( on );
        }

        inline auto enabled_eeprom_write()
        {
            return eepe::read();
        }

        inline auto enable_eeprom_write( bool on )
        {
            return eepe::write_lazy( on );
        }

        inline auto enabled_eeprom_read()
        {
            return eere::read();
        }

        inline auto enable_eeprom_read( bool on )
        {
            return eere::write_lazy( on );
        }
    }   // eecr

    // 12.6.5/6/7 GPIOR2/1/0: General Purpose I/O Register 2/1/0

    // Note: register 0 is bit-addressable
    // Example: mem::gpio<mem::r0>::bit<2>::set();

    // See also: http://bit.ly/mm-avr-gpior-usage

    static constexpr address_t r2 = gpior2_addr;
    static constexpr address_t r1 = gpior1_addr;
    static constexpr address_t r0 = gpior0_addr;

    template< address_t gpiorn_address >
    struct gpio
    {
        using whole = register8_t< rw_t, gpiorn_address >;

        template< uint8_t bit_id >
        using bit = bitfield8_t< rw_t, gpiorn_address, bit_id >;
    };

    // provide functions in mem namespace:

    using eecr::eeprom_programming_mode;
    using eecr::enabled_eeprom_ready_interrupt;
    using eecr::enable_eeprom_ready_interrupt;
    using eecr::enabled_eeprom_master_write;
    using eecr::enable_eeprom_master_write;
    using eecr::enabled_eeprom_write;
    using eecr::enable_eeprom_write;
    using eecr::enabled_eeprom_read;
    using eecr::enable_eeprom_read;
}

// 11. AVR CPU Core
//
// 11.3 Status Register
// 11.4 General Purpose Register File
// 11.5 Stack Pointer Register Low and High byte

// 13. System Clock and Clock Options
//
// 13.12.1 OSCCAL: Oscillator Calibration Register
// 13.12.2 CLKPR : Clock Prescaler Register

// 14. Power Management and Sleep Modes
//
// 14.12.1 SMCR : Sleep Mode Control Register
// 14.12.2 MCUCR: MCU Control Register
// 14.12.3 PRR : Power Reduction Register

// 15. System Control and Reset
//
// 15.9.1 MCUSR : MCU Status Register
// 15.9.2 WDTCSR: Watchdog Timer Control Register

namespace mcu
{
    static constexpr address_t gpreg_base  = 0x00;
    static constexpr address_t sreg_addr   = 0x5f;
    static constexpr address_t sp_addr     = 0x5d;
    static constexpr address_t spl_addr    = sp_addr;
    static constexpr address_t sph_addr    = sp_addr + 1;
    static constexpr address_t osccal_addr = 0x66;
    static constexpr address_t clkpr_addr  = 0x61;
    static constexpr address_t smcr_addr   = 0x53;
    static constexpr address_t mcusr_addr  = 0x54;
    static constexpr address_t mcucr_addr  = 0x55;
    static constexpr address_t wdtcsr_addr = 0x60;
    static constexpr address_t prr_addr    = 0x64;

    // 7.3.1 - SREG: AVR Status Register:
    //
    // 7: Global Interrupt Enable; or use sei(), cli()
    // 6: T: Bit Copy Storage
    // 5: H: Half Carry Flag
    // 4: S: Sign Bit, S = N xor V
    // 3: V: Two’s Complement Overflow Flag
    // 2: N: Negative Flag
    // 1: Z: Zero Flag
    // 0: C: Carry Flag

    namespace sreg
    {
        using whole = register8_t< rw_t, sreg_addr    >;     // or: 'whole'
        using     I = bitfield8_t< rw_t, sreg_addr, 7 >;
        using     T = bitfield8_t< rw_t, sreg_addr, 6 >;
        using     H = bitfield8_t< rw_t, sreg_addr, 5 >;
        using     S = bitfield8_t< rw_t, sreg_addr, 4 >;
        using     V = bitfield8_t< rw_t, sreg_addr, 3 >;
        using     N = bitfield8_t< rw_t, sreg_addr, 2 >;
        using     Z = bitfield8_t< rw_t, sreg_addr, 1 >;
        using     C = bitfield8_t< rw_t, sreg_addr, 0 >;
    }

    // 11.4 - r0..r31, rx, y, z: AVR General Purpose Register File:

    namespace gpwr
    {
        using r0  = register8_t< rw_t, 0x00 >;
        using r1  = register8_t< rw_t, 0x01 >;
        using r2  = register8_t< rw_t, 0x02 >;
        using r3  = register8_t< rw_t, 0x03 >;
        using r4  = register8_t< rw_t, 0x04 >;
        using r5  = register8_t< rw_t, 0x05 >;
        using r6  = register8_t< rw_t, 0x06 >;
        using r7  = register8_t< rw_t, 0x07 >;
        using r8  = register8_t< rw_t, 0x08 >;
        using r9  = register8_t< rw_t, 0x09 >;
        using r10 = register8_t< rw_t, 0x0a >;
        using r11 = register8_t< rw_t, 0x0b >;
        using r12 = register8_t< rw_t, 0x0c >;
        using r13 = register8_t< rw_t, 0x0d >;
        using r14 = register8_t< rw_t, 0x0e >;
        using r15 = register8_t< rw_t, 0x0f >;
        using r16 = register8_t< rw_t, 0x10 >;
        using r17 = register8_t< rw_t, 0x11 >;
        using r18 = register8_t< rw_t, 0x12 >;
        using r19 = register8_t< rw_t, 0x13 >;
        using r20 = register8_t< rw_t, 0x14 >;
        using r21 = register8_t< rw_t, 0x15 >;
        using r22 = register8_t< rw_t, 0x16 >;
        using r23 = register8_t< rw_t, 0x17 >;
        using r24 = register8_t< rw_t, 0x18 >;
        using r25 = register8_t< rw_t, 0x19 >;
        using r26 = register8_t< rw_t, 0x1a >;      // XL
        using r27 = register8_t< rw_t, 0x1b >;      // XH
        using r28 = register8_t< rw_t, 0x1c >;      // YL
        using r29 = register8_t< rw_t, 0x1d >;      // YH
        using r30 = register8_t< rw_t, 0x1e >;      // ZL
        using r31 = register8_t< rw_t, 0x1f >;      // ZH

        using x   = register16_t< rw_t, 0x1a >;
        using y   = register16_t< rw_t, 0x1c >;
        using z   = register16_t< rw_t, 0x1e >;
    }

    // 11.5 Stack Pointer Register Low and High byte

    namespace sp
    {
        using whole = register16_t< rw_t, sp_addr  >;
        using lo    =  register8_t< rw_t, spl_addr >;
        using hi    =  register8_t< rw_t, sph_addr >;
    }

    // 13.12.1 OSCCAL: Oscillator Calibration Register:

    namespace osccal
    {
//    osccal_addr

    }

    // 13.12.2 CLKPR : Clock Prescaler Register:

//    clkpr_addr
    namespace clkpr
    {

    }

    // 14.12.1 SMCR : Sleep Mode Control Register:
//    smcr_addr
    namespace smcr
    {

    }

    // 14.12.2 MCUCR: MCU Control Register:

//    mcusr_addr
    namespace mcusr
    {

    }

    // 14.12.3 PRR: Power Reduction Register:

//    prr_addr
    namespace prr
    {

    }

    // 15.9.1 MCUSR : MCU Status Register:
//    mcucr_addr
    namespace mcucr
    {

    }

    // 15.9.2 WDTCSR: Watchdog Timer Control Register:

//    wdtcsr_addr
    namespace wdtcsr
    {

    }
}

// 17. External Interrupts
//
// 17.2.1 EICRA : External Interrupt Control Register A
// 17.2.2 EIMSK : External Interrupt Mask Register
// 17.2.3 EIFR  : External Interrupt Flag Register
// 17.2.4 PCICR : Pin Change Interrupt Control Register
// 17.2.5 PCIFR : Pin Change Interrupt Flag Register
// 17.2.6 PCMSK2: Pin Change Mask Register 2
// 17.2.7 PCMSK1: Pin Change Mask Register 1
// 17.2.8 PCMSK0: Pin Change Mask Register 0

namespace ei
{
    static constexpr address_t eicra_addr  = 0x69;
    static constexpr address_t eimsk_addr  = 0x3d;
    static constexpr address_t eifr_addr   = 0x3c;
    static constexpr address_t pcicr_addr  = 0x68;
    static constexpr address_t pcifr_addr  = 0x3b;
    static constexpr address_t pcmsk2_addr = 0x6d;
    static constexpr address_t pcmsk1_addr = 0x6c;
    static constexpr address_t pcmsk0_addr = 0x6b;
}

// 18. I/O-Ports
//
// 18.4.1 MCUCR: MCU Control Register: bit 4: pull-up diable, see namespace mcu
// 18.4.2 PORTB: Port B Data Register
// 18.4.3 DDRB : Port B Data Direction Register
// 18.4.4 PINB : Port B Input Pins Address
// 18.4.5 PORTC: Port C Data Register (no bit7)
// 18.4.6 DDRC : Port C Data Direction Register (no bit7)
// 18.4.7 PINC : Port C Input Pins Address (no bit7)
// 18.4.8 PORTD: Port D Data Register
// 18.4.9 DDRD : Port D Data Direction Register
// 18.4.10 PIND: Port D Input Pins Address

enum class port { B, C, D, };

template< port port_id >
struct gpio
{
    static constexpr address_t base_addr  = 0x23 + 3 * to_integral(port_id);
    static constexpr address_t pin_addr   = base_addr + 0;
    static constexpr address_t ddr_addr   = base_addr + 1;
    static constexpr address_t port_addr  = base_addr + 2;

    using pin  = register8_t<rw_t,  pin_addr>;  // pin input level
    using ddr  = register8_t<rw_t,  ddr_addr>;  // data direction register
    using port = register8_t<rw_t, port_addr>;  // port io register

    template< uint8_t pin_id >
    struct p
    {
        using dir = bitfield8_t< rw_t, ddr_addr , pin_id >;
        using get = bitfield8_t< r_t , pin_addr , pin_id >;
        using set = bitfield8_t< rw_t, port_addr, pin_id >;
    };
};

// Timer-counters:
//
// 19. 8-bit Timer/Counter0 (TC0) with PWM
// 19.9.1 TCCR0A: TC0 Control Register A
// 19.9.2 TCCR0B: TC0 Control Register B
// 19.9.3 TIMSK0: TC0 Interrupt Mask Register
// 19.9.4 GTCCR : General Timer/Counter Control Register
// 19.9.5 TCNT0 : TC0 Counter Value Register
// 19.9.6 OCR0A : TC0 Output Compare Register A
// 19.9.7 OCR0B : TC0 Output Compare Register B
// 19.9.8 TIFR0 : TC0 Interrupt Flag Register
//
// 20. 16-bit Timer/Counter1 (TC1) with PWM
// 20.15.1 TCCR0A  : TC1 Control Register A
// 20.15.2 TCCR0B  : TC1 Control Register B
// 20.15.3 TCCR0C  : TC1 Control Register C
// 20.15.4 TCNT1L/H: TC1 Counter Value Low and High byte
// 20.15.5 ICR1L/H : Input Capture Register 1 Low and High byte
// 20.15.6 OCR1AL/H: Output Compare Register 1 A Low and High byte
// 20.15.7 OCR1BL/H: Output Compare Register 1 B Low and High byte
// 20.15.8 TIMSK1  : Timer/Counter 1 Interrupt Mask Register
// 20.15.9 TIFR1   : TC1 Interrupt Flag Register
//
// 21. Timer/Counter 0, 1 Prescalers
// 21.4.1 General Timer/Counter Control Register
//
// 22. 8-bit Timer/Counter2 (TC2) with PWM and Asynchronous Operation
// 22.11.1 TCCR2A: TC2 Control Register A
// 22.11.2 TCCR2B: TC2 Control Register B
// 22.11.3 TCNT2 : TC2 Counter Value Register
// 22.11.4 OCR2A : TC2 Output Compare Register A
// 22.11.5 OCR2B : TC2 Output Compare Register B
// 22.11.6 TIMSK2: TC2 Interrupt Mask Register
// 22.11.7 TIFR2 : TC2 Interrupt Flag Register
// 22.11.8 ASSR : Asynchronous Status Register
// 22.11.9 GTCCR : General Timer/Counter Control Register

namespace tc
{
}

// 23. SPI - Serial Peripheral Interface (SPI)
//
// 23.5.1 SPCR0: SPI Control Register 0
// 23.5.2 SPSR0: SPI Status Register 0
// 23.5.3 SPDR0: SPI Data Register 0

namespace spi
{
}

// 24. USART - Universal Synchronous Asynchronous Receiver Transceiver (USART)
//
// 24.12.1 UDR0    : USART I/O Data Register 0
// 24.12.2 UCSR0A  : USART Control and Status Register 0 A
// 24.12.3 UCSR0B  : USART Control and Status Register 0 B
// 24.12.4 UCSR0C  : USART Control and Status Register 0 C
// 24.12.5 UBRR0L/H: USART Baud Rate 0 Register Low and High byte
//
// 25. USART in SPI (USARTSPI) Mode
// See 24

namespace usart
{
}

// 26. TWI - Two-Wire Serial Interface (TWI)
//
// 26.9.1 TWBR : TWI Bit Rate Register
// 26.9.2 TWSR : TWI Status Register
// 26.9.3 TWAR : TWI (Slave) Address Register
// 26.9.4 TWDR : TWI Data Register
// 26.9.5 TWCR : TWI Control Register
// 26.9.6 TWAMR: TWI (Slave) Address Mask Register

namespace twi
{
}

// 27. AC - Analog Comparator (AC)
//
// 27.3.1 ACSR : Analog Comparator Control and Status Register
// 27.3.2 DIDR1: Digital Input Disable Register 1

namespace ac
{
    static constexpr address_t addr_acsr  = 0x50;
    static constexpr address_t addr_didr1 = 0x7f;

    enum class interrupt : uint8_t
    {
        toggle = 0      // comparator interrupt on output toggle
        , reserved1     // -
        , falling_edge  // comparator interrupt on falling output edge
        , rising_edge   // comparator interrupt on rising output edge
    };

    namespace acsr
    {
        using acd  = bitfield8_t< rw_t, addr_acsr, ACD  >;
        using acbg = bitfield8_t< rw_t, addr_acsr, ACBG >;
        using aco  = bitfield8_t< r_t , addr_acsr, ACO  >;
        using aci  = bitfield8_t< rc_w1_t, addr_acsr, ACI  >;
        using acie = bitfield8_t< rw_t, addr_acsr, ACIE >;
        using acic = bitfield8_t< rw_t, addr_acsr, ACIC >;
        using acis = bitfield8_t< rw_t, addr_acsr, ACIS1, ACIS0 >;

        inline auto disabled_comparator()
        {
            return acd::read();
        }

        inline auto disable_comparator( bool on )
        {
            return acd::write_lazy( on );
        }

        inline auto fixed_bandgap()
        {
            return acbg::read();
        }

        inline auto fixed_bandgap( bool on )
        {
            return acbg::write_lazy( on );
        }

        inline auto comparator_output()
        {
            return aco::read();
        }

        inline auto interrupt_flag()
        {
            return aci::read();
        }

        inline auto clear_interrupt_flag()
        {
            aci::clear();
        }

        inline auto enabled_interrupt()
        {
            return acie::read();
        }

        inline auto enable_interrupt( bool on )
        {
            return acie::write_lazy( on );
        }

        inline auto enabled_input_capture();
        inline auto enable_input_capture( bool on );

        // disable/restore interrupt enable bit:

        struct scoped_interrupt_guard
        {
            const bool ei = enabled_interrupt();
            scoped_interrupt_guard() { enable_interrupt(false); }
            ~scoped_interrupt_guard() { enable_interrupt(ei); }
        };

        inline auto interrupt_on()
        {
            return interrupt{ acis::read() };
        }

        inline auto interrupt_on( interrupt what )
        {
            return acis::write_lazy( to_integral( what ) );
        }
    }

    // didr1  – Digital Input Disable Register 1:
    // exampple: adc::didr1::write( bitmask( AIN1D, AIN0D ) );

    using didr1 = bitfield8_t< rw_t, addr_didr1, AIN1D, AIN0D >;

    // provide types and functions in ac namespace:

    using acsr::scoped_interrupt_guard;

    using acsr::disabled_comparator;
    using acsr::disable_comparator;
    using acsr::fixed_bandgap;
    using acsr::fixed_bandgap;
    using acsr::comparator_output;
    using acsr::interrupt_flag;
    using acsr::clear_interrupt_flag;
    using acsr::enabled_interrupt;
    using acsr::enable_interrupt;
    using acsr::enabled_input_capture;
    using acsr::enable_input_capture;
    using acsr::interrupt_on;
}

// 28. ADC - Analog-to-Digital Converter (ADC)
//
// 28.9.1 ADMUX : ADC Multiplexer Selection Register
// 28.9.2 ADCSRA: ADC Control and Status Register A
// 28.9.3 ADCL/H: ADC Data Register Low and High Byte (ADLAR=0)
// 28.9.4 ADCL/H: ADC Data Register Low and High Byte (ADLAR=1)
// 28.9.5 ADCSRB: ADC Control and Status Register B
// 28.9.6 DIDR0 : Digital Input Disable Register 0

namespace adc
{
    // register addresses:

    static constexpr address_t base_addr   = 0x78;
    static constexpr address_t addr_adcl   = base_addr + 0;
    static constexpr address_t addr_adch   = base_addr + 1;
    static constexpr address_t addr_adcsra = base_addr + 2;
    static constexpr address_t addr_adcsrb = base_addr + 3;
    static constexpr address_t addr_admux  = base_addr + 4;
    // + 5: reserved
    static constexpr address_t addr_didr0  = base_addr + 6;
    static constexpr address_t addr_didr1  = base_addr + 7;

    // adc conversion result type:

    using result_t = uint16_t;

    // admux voltage reference:

    enum class voltage_ref : uint8_t
    {
        aref = 0
        , avcc
        , reserved
        , intern_1v1
    };

    // admux adc result adjustment:

    enum class adjustment : uint8_t
    {
        right = 0
        , left
    };

    // admux input channel:

    enum class input : uint8_t
    {
        ch0 = 0
        , ch1
        , ch2
        , ch3
        , ch4
        , ch5
        , ch6
        , ch7
        , ch8
        , reserved9
        , reserved10
        , reserved11
        , reserved12
        , reserved13
        , internal_1v1  // 1.1 V
        , gnd           // 0 V
    };

    // adcsra prescaler factor:

    enum class factor : uint8_t
    {
        x02 = 0
        , x2
        , x4
        , x8
        , x16
        , x32
        , x64
        , x128
    };

    // adcsrb trigger source:

    enum class auto_trigger_source : uint8_t
    {
        free_run = 0
        , analog_comparator
        , external_interrupt_0
        , timer_counter_0_cmp_match_a
        , timer_counter_0_overflow
        , timer_counter_1_cmp_match_b
        , timer_counter_1_overflow
        , timer_counter_1_capture_event
    };

    // adc: ADCL and ADCH – The ADC Data Register:

    namespace adc
    {
        // see also admux::adjust()

        using adchl = register16_t< r_t, addr_adcl >;   // 16-bit result
        using adcl  =  register8_t< r_t, addr_adcl >;   //  8-bit lsb
        using adch  =  register8_t< r_t, addr_adch >;   //  2-bit msb

        inline auto result()
        {
            return adchl::read();
        }

        inline auto result_lsb()
        {
            return adcl::read();
        }

        inline auto result_msb()
        {
            return adch::read();
        }
    }

    // admux – ADC Multiplexer Selection Register:

    namespace admux
    {
        using refs  = bitfield8_t< rw_t, addr_admux, REFS1, REFS0 >; // voltage reference
        using adlar = bitfield8_t< rw_t, addr_admux, ADLAR        >; // left adjust result
        using mux   = bitfield8_t< rw_t, addr_admux, MUX3 , MUX0  >; // input channel

        inline auto reference()
        {
            return voltage_ref{ refs::read() };
        }

        inline auto reference( voltage_ref vref )
        {
            return refs::write_lazy( to_integral(vref) );
        }

        inline auto adjust()
        {
            return adjustment{ adlar::read() };
        }

        inline auto adjust( adjustment adjust )
        {
            return adlar::write_lazy( adjust == adjustment::left );
        }

        inline auto channel()
        {
            return input{ mux::read() };
        }

        inline auto channel( input ch )
        {
            return mux::write_lazy( to_integral(ch) );
        }
    };

    // adcsra – ADC Control and Status Register A:

    namespace adcsra
    {
        using aden  = bitfield8_t< rw_t   , addr_adcsra, ADEN >;        // ADC Enable
        using adsc  = bitfield8_t< rw_t   , addr_adcsra, ADSC >;        // ADC Start Conversion
        using adate = bitfield8_t< rw_t   , addr_adcsra, ADATE>;        // ADC Auto Trigger Enable
        using adif  = bitfield8_t< rc_w1_t, addr_adcsra, ADIF >;        // ADC Interrupt Flag
        using adie  = bitfield8_t< rw_t   , addr_adcsra, ADIE >;        // ADC Interrupt Enable
        using adps  = bitfield8_t< rw_t   , addr_adcsra, ADPS2, ADPS0 >;// ADC Prescaler Select Bits

        inline auto enabled()
        {
            return aden::read();
        }

        inline auto enable( bool on = true )
        {
            return aden::write_lazy( on );
        }

        inline auto start_conversion()
        {
            return adsc::write_lazy( true );
        }

        inline auto auto_trigger()
        {
            return 0 != adate::read();
        }

        inline auto auto_trigger( bool on )
        {
            return adate::write_lazy( on );
        }

        inline auto interrupt_flag()
        {
            return adif::read();
        }

        inline auto clear_interrupt_flag()
        {
            return adif::clear();
        }

        inline auto enabled_interrupt()
        {
            return 0 != adps::read();
        }

        inline auto enable_interrupt( bool on )
        {
            return adps::write_lazy( on );
        }

        inline auto prescale()
        {
            return factor{ adps::read() };
        }

        inline auto prescale( factor x )
        {
            return adps::write_lazy( to_integral(x) );
        }
    };

    // adcsrb – ADC Control and Status Register B:

    namespace adcsrb
    {
        using acme = bitfield8_t< rw_t, addr_adcsrb, ACME         >;    // ADC Analog Comparator Multiplexer Enabled
        using adts = bitfield8_t< rw_t, addr_adcsrb, ADTS2, ADTS0 >;    // ADC Auto Trigger Source

        inline auto enabled_comparator_mux()
        {
            return 0 != acme::read();
        }

        inline auto enable_comparator_mux( bool on )
        {
            return acme::write_lazy( on );
        }

        inline auto trigger_source()
        {
            return auto_trigger_source{ adts::read() };
        }

        inline auto trigger_source( auto_trigger_source x )
        {
            return adts::write_lazy( to_integral(x) );
        }
    };

    // didr0  – Digital Input Disable Register 0:
    // exampple: adc::didr0::write( bitmask( ADC1D, ADC0D ) );

    using didr0 = bitfield8_t< rw_t, addr_didr0, ADC5D, ADC0D >;

    // didr1  – Digital Input Disable Register 1:
    // exampple: adc::didr1::write( bitmask( AIN1D, AIN0D ) );

    using didr1 = ac::didr1;

    // provide functions in adc namespace:

    using adc::result;
    using adc::result_lsb;
    using adc::result_msb;

    using admux::reference;
    using admux::adjust;
    using admux::channel;

    using adcsra::enable;
    using adcsra::enabled;
    using adcsra::start_conversion;
    using adcsra::auto_trigger;
    using adcsra::interrupt_flag;
    using adcsra::clear_interrupt_flag;
    using adcsra::enable_interrupt;
    using adcsra::enabled_interrupt;
    using adcsra::prescale;

    using adcsrb::enabled_comparator_mux;
    using adcsrb::enable_comparator_mux;
    using adcsrb::trigger_source;
}

// debugWIRE On-chip Debug System:
//
// 25.6.1 - DWDR: debugWire Data Register

namespace dbgw
{
}

// 30. Boot Loader Support – Read-While-Write Self-programming (BTLDR)
//
// 30.9.1 SPMCSR: Store Program Memory Control and Status Register (SPMCSR)

namespace spf  // self-programming flash
{
}

// DAC - Digital-analog conversion via PWM:

namespace dac
{
    using value_type = uint16_t;
}

// LED on pb5:

namespace led
{
    using led_t = gpio<port::B>::p<5>;

    inline void on()     { led_t::set::set();    }
    inline void off()    { led_t::set::clear();  }
    inline void toggle() { led_t::set::toggle(); }
    inline void enable() { led_t::dir::set();    }
};

} // namespace atmega328
} // namespace mcu

#endif // MCU_ATMEGA328_HPP_INCLUDED
