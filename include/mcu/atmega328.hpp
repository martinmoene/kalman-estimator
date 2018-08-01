//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//
// Datasheet referenced: Microchip Technology Inc. ATmega328/P AVR MCU. 2018.
//

#ifndef MCU_ATMEGA328_HPP_INCLUDED
#define MCU_ATMEGA328_HPP_INCLUDED

#include "mcu/register.hpp"

namespace mcu {
namespace atmega328 {

//
// MCU control, status and peripherals:
//

// Convert to class enums to underlying type:

template<typename T, size_t = sizeof(T)> struct utype;
template<typename T> struct utype<T,1> { using type = uint8_t; };
template<typename T> struct utype<T,2> { using type = uint16_t; };

template<typename T> using utype_t = typename utype<T>::type;


template< typename Id >
constexpr auto to_integral( Id id )
{
    return static_cast< utype_t<Id> >( id );
}

// 11. AVR CPU Core
//
// 11.3 Status Register
// 11.4 General Purpose Register File
// 11.5 Stack Pointer Register Low and High byte

namespace core
{
    static constexpr address_t gpreg_base = 0x00;
    static constexpr address_t sreg_addr  = 0x5f;
    static constexpr address_t sp_addr    = 0x5d;
    static constexpr address_t spl_addr   = sp_addr;
    static constexpr address_t sph_addr   = sp_addr + 1;

    // 11.3 SREG: AVR Status Register
    //
    // 7: Global Interrupt Enable; or use sei(), cli()
    // 6: T: Bit Copy Storage
    // 5: H: Half Carry Flag
    // 4: S: Sign Bit, S = N xor V
    // 3: V: Two's Complement Overflow Flag
    // 2: N: Negative Flag
    // 1: Z: Zero Flag
    // 0: C: Carry Flag

    namespace sreg
    {
        using whole = register8_t< rw_t, sreg_addr    >;
        using     I = bitfield8_t< rw_t, sreg_addr, 7 >;
        using     T = bitfield8_t< rw_t, sreg_addr, 6 >;
        using     H = bitfield8_t< rw_t, sreg_addr, 5 >;
        using     S = bitfield8_t< rw_t, sreg_addr, 4 >;
        using     V = bitfield8_t< rw_t, sreg_addr, 3 >;
        using     N = bitfield8_t< rw_t, sreg_addr, 2 >;
        using     Z = bitfield8_t< rw_t, sreg_addr, 1 >;
        using     C = bitfield8_t< rw_t, sreg_addr, 0 >;

        using value_type = whole::value_type;
    }

    // save-restore status register for ISR:
    // example:
    // ISR() {
    //   core::scoped_sreg _;   // save sreg
    //   ...                    // isr functionality
    // }                        // sreg is restored
    //

    struct scoped_sreg
    {
        using value_type = sreg::value_type;

        value_type cache = sreg::whole::read();
        ~scoped_sreg() { sreg::whole::write( cache ); }
    };

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

        using value_type = whole::value_type;
    }
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
    namespace reg
    {
        // register addresses:

        static constexpr address_t eear_addr   = 0x41;
        static constexpr address_t eedr_addr   = 0x40;
        static constexpr address_t eecr_addr   = 0x3f;

        static constexpr address_t gpior2_addr = 0x4b;
        static constexpr address_t gpior1_addr = 0x4a;
        static constexpr address_t gpior0_addr = 0x3e;

        // 12.6.2 EEARH, EEARL: EEPROM Address Register Low and High Byte
        // 12.6.3 EEDR : EEPROM Data Register

        enum class data : uint8_t{};
        enum class address : uint16_t{};

        using eear = register_t< address, rw_t, eear_addr >;
        using eedr = register_t< data   , rw_t, eedr_addr >;

        // 12.6.4 EECR: EEPROM Control Register
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

            using value_type = whole::value_type;
        }   // eecr
    } // namespace reg

    // eeprom

    namespace eeprom
    {
        using reg::data;
        using reg::address;

        inline auto current( address )
        {
            return reg::eear::read();
        }

        inline auto set( address a )
        {
            reg::eear::write( a );
        }

        inline auto current( data )
        {
            return reg::eedr::read();
        }

        inline auto set( data v )
        {
            reg::eedr::write( v );
        }

        namespace program
        {
            enum class mode : uint8_t
            {
                erase_write = 0     // 3.4 ms, atomic operation
                , erase_only        // 1.8 ms
                , write_only        // 1.8 ms
                , reserved3         // -
            };

            inline auto current( mode )
            {
                return mode{ reg::eecr::eepm::read() };
            }

            inline auto set( mode m )
            {
                return reg::eecr::eepm::write_lazy( to_integral(m) );
            }
        }

        namespace enable
        {
            enum class read         : uint8_t { off, on };
            enum class write        : uint8_t { off, on };
            enum class master_write : uint8_t { off, on };

            inline auto current( master_write )
            {
                return master_write{ reg::eecr::eempe::read() };
            }

            inline auto set( master_write e )
            {
                return reg::eecr::eempe::write_lazy( to_integral(e) );
            }

            inline auto current( write )
            {
                return write{ reg::eecr::eepe::read() };
            }

            inline auto set( write e )
            {
                return reg::eecr::eepe::write_lazy( to_integral(e) );
            }

            inline auto current( read )
            {
                return read{ reg::eecr::eere::read() };
            }

            inline auto set( read e )
            {
                return reg::eecr::eere::write_lazy( to_integral(e) );
            }
        }

        namespace interrupt
        {
            enum class on_ready : uint8_t { off, on };

            inline auto current( on_ready )
            {
                return on_ready{ reg::eecr::eerie::read() };
            }

            inline auto set( on_ready i )
            {
                return reg::eecr::eerie::write_lazy( to_integral(i) );
            }
        }
    } // namespace eeprom

    // 12.6.5/6/7 GPIOR2/1/0: General Purpose I/O Register 2/1/0

    // Note: register 0 is bit-addressable
    // Example: mem::gpio<mem::r0>::bit<2>::set();

    // See also: http://bit.ly/mm-avr-gpior-usage

    static constexpr address_t r2 = reg::gpior2_addr;
    static constexpr address_t r1 = reg::gpior1_addr;
    static constexpr address_t r0 = reg::gpior0_addr;

    template< address_t gpiorn_address >
    struct gpio
    {
        using whole = register8_t< rw_t, gpiorn_address >;
        using value_type = typename whole::value_type;

        template< uint8_t bit_id >
        using bit = bitfield8_t< rw_t, gpiorn_address, bit_id >;
    };
}

// 13. System Clock and Clock Options
//
// 13.12.1 OSCCAL: Oscillator Calibration Register
// 13.12.2 CLKPR : Clock Prescaler Register

namespace clock
{
    namespace reg
    {
        static constexpr address_t osccal_addr = 0x66;
        static constexpr address_t clkpr_addr  = 0x61;

        // 13.12.1 OSCCAL: Oscillator Calibration Register:

        namespace osccal
        {
        }

        // 13.12.2 CLKPR : Clock Prescaler Register:

        namespace clkpr
        {
        }
    } // namespace reg
}

// 14. Power Management and Sleep Modes
//
// 14.12.1 SMCR : Sleep Mode Control Register
// 14.12.2 MCUCR: MCU Control Register
// 14.12.3 PRR : Power Reduction Register

namespace power
{
    namespace reg
    {
        static constexpr address_t smcr_addr   = 0x53;
        static constexpr address_t mcusr_addr  = 0x54;
        static constexpr address_t prr_addr    = 0x64;

        // 14.12.1 SMCR : Sleep Mode Control Register:

        namespace smcr
        {
        }

        // 14.12.2 MCUCR: MCU Control Register:

        namespace mcusr
        {
        }

        // 14.12.3 PRR: Power Reduction Register:

        namespace prr
        {
        }
    } // namespace reg
}

// 15. System Control and Reset
//
// 15.9.1 MCUSR : MCU Status Register
// 15.9.2 WDTCSR: Watchdog Timer Control Register

namespace reset
{
    namespace reg
    {
        static constexpr address_t mcucr_addr  = 0x55;
        static constexpr address_t wdtcsr_addr = 0x60;

        // 15.9.1 MCUSR : MCU Status Register:

        namespace mcucr
        {
        }

        // 15.9.2 WDTCSR: Watchdog Timer Control Register:

        namespace wdtcsr
        {
        }
    } // namespace reg
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

// external interrupt int0..1:
enum class eint : uint8_t { int0, int1, };

// pin change interrupt int0..2
enum class pcint : uint8_t { int0, int1, int2 };

namespace ei
{
    namespace reg
    {
    } // namespace reg

    static constexpr address_t eicra_addr  = 0x69;
    static constexpr address_t eimsk_addr  = 0x3d;
    static constexpr address_t eifr_addr   = 0x3c;
    static constexpr address_t pcicr_addr  = 0x68;
    static constexpr address_t pcifr_addr  = 0x3b;
    static constexpr address_t pcmsk2_addr = 0x6d;
    static constexpr address_t pcmsk1_addr = 0x6c;
    static constexpr address_t pcmsk0_addr = 0x6b;

    enum class sense : uint8_t
    {
        low = 0
        , change
        , falling_edge
        , rising_edge
    };

    // 17.2.1 EICRA : External Interrupt Control Register A

    namespace eicra
    {
        using whole = register8_t< rw_t, eicra_addr >;
        using ics1  = bitfield8_t< rw_t, eicra_addr , ISC11, ISC10 >;
        using ics0  = bitfield8_t< rw_t, eicra_addr , ISC01, ISC00 >;

        using value_type = whole::value_type;

        template< eint i >
        inline auto interrupt_sense()
        {
            if constexpr( i == eint::int1 )
            {
                return sense{ ics1::read() };
            }

            if constexpr( i == eint::int0 )
            {
                return sense{ ics0::read() };
            }
        }

        template< eint i >
        inline auto interrupt_sense( sense s )
        {
            if constexpr( i == eint::int1 )
            {
                return ics1::write_lazy( to_integral( s ) );
            }

            if constexpr( i == eint::int0 )
            {
                return ics0::write_lazy( to_integral( s ) );
            }
        }
    }

    // 17.2.2 EIMSK: External Interrupt Mask Register

    template< eint i >
    using eimsk = bitfield8_t< rw_t, eimsk_addr, to_integral(i) >;

    inline auto enabled_external_int0()
    {
        return eimsk<eint::int0>::read();
    }

    inline auto enabled_external_int1()
    {
        return eimsk<eint::int1>::read();
    }

    inline auto enable_external_int0( bool on )
    {
        return eimsk<eint::int0>::write( on );
    }

    inline auto enable_external_int1( bool on )
    {
        return eimsk<eint::int1>::write( on );
    }

    // 17.2.3 EIFR: External Interrupt Flag Register

    template< eint i >
    using eifr = bitfield8_t< rc_w1_t, eifr_addr, to_integral(i) >;

    inline auto external_int0_flag()
    {
        return eifr<eint::int0>::read();
    }

    inline auto external_int1_flag()
    {
        return eifr<eint::int1>::read();
    }

    inline auto clear_external_int0_flag()
    {
        return eifr<eint::int0>::clear();
    }

    inline auto clear_external_int1_flag()
    {
        return eifr<eint::int1>::clear();
    }

    // 17.2.4 PCICR: Pin Change Interrupt Control Register

    template< pcint i >
    using pcicr = bitfield8_t< rw_t, pcicr_addr, to_integral(i) >;

    inline auto enabled_pin_change_int2()
    {
        return pcicr<pcint::int2>::read();
    }

    inline auto enabled_pin_change_int1()
    {
        return pcicr<pcint::int1>::read();
    }

    inline auto enabled_pin_change_int0()
    {
        return pcicr<pcint::int0>::read();
    }

    inline auto enable_pin_change_int2( bool on )
    {
        return pcicr<pcint::int2>::write( on );
    }

    inline auto enable_pin_change_int1( bool on )
    {
        return pcicr<pcint::int1>::write( on );
    }

    inline auto enable_pin_change_int0( bool on )
    {
        return pcicr<pcint::int0>::write( on );
    }

    // 17.2.5 PCIFR: Pin Change Interrupt Flag Register

    template< pcint i >
    using pcifr = bitfield8_t< rw_t, pcifr_addr, to_integral(i) >;

    inline auto pin_change_int2_flag()
    {
        return pcifr<pcint::int2>::read();
    }

    inline auto pin_change_int1_flag()
    {
        return pcifr<pcint::int1>::read();
    }

    inline auto pin_change_int0_flag()
    {
        return pcifr<pcint::int0>::read();
    }

    inline auto clear_pin_change_int2_flag()
    {
        return pcifr<pcint::int2>::clear();
    }

    inline auto clear_pin_change_int1_flag()
    {
        return pcifr<pcint::int1>::clear();
    }

    inline auto clear_pin_change_int0_flag()
    {
        return pcifr<pcint::int0>::clear();
    }

    // 17.2.6 PCMSK2: Pin Change Mask Register 2
    // 17.2.7 PCMSK1: Pin Change Mask Register 1
    // 17.2.8 PCMSK0: Pin Change Mask Register 0

    static constexpr address_t pcmsk2 = pcmsk2_addr;
    static constexpr address_t pcmsk1 = pcmsk1_addr;
    static constexpr address_t pcmsk0 = pcmsk0_addr;

    template< address_t pcmskn_address >
    struct pcmsk
    {
        using whole = register8_t< rw_t, pcmskn_address >;
        using value_type = typename whole::value_type;

        template< uint8_t bit_id >
        using bit = bitfield8_t< rw_t, pcmskn_address, bit_id >;
    };

    inline auto pin_change_interrupt_mask2()
    {
        return pcmsk<pcmsk2>::whole::read();
    }

    inline auto pin_change_interrupt_mask1()
    {
        return pcmsk<pcmsk1>::whole::read();
    }

    inline auto pin_change_interrupt_mask0()
    {
        return pcmsk<pcmsk0>::whole::read();
    }

    inline auto pin_change_interrupt_mask2( uint8_t mask )
    {
        pcmsk<pcmsk2>::whole::write( mask );
    }

    inline auto pin_change_interrupt_mask1( uint8_t mask )
    {
        pcmsk<pcmsk1>::whole::write( mask );
    }

    inline auto pin_change_interrupt_mask0( uint8_t mask )
    {
        pcmsk<pcmsk0>::whole::write( mask );
    }

    // provide functions in ei namespace:

    using eicra::interrupt_sense;
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

// io port id:
enum class port : uint8_t { B, C, D, };

template< port port_id >
struct gpio
{
    static constexpr address_t base_addr  = 0x23 + 3 * to_integral(port_id);
    static constexpr address_t pin_addr   = base_addr + 0;
    static constexpr address_t ddr_addr   = base_addr + 1;
    static constexpr address_t port_addr  = base_addr + 2;

    using value_type = uint8_t;

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

namespace tc0
{
    namespace reg
    {
        // register addresses:

        static constexpr address_t tccr0a_addr = 0x44;
        static constexpr address_t tccr0b_addr = 0x45;
        static constexpr address_t timsk0_addr = 0x6e;
        static constexpr address_t gtccr_addr  = 0x43;
        static constexpr address_t tcnt0_addr  = 0x46;
        static constexpr address_t ocr0a_addr  = 0x47;
        static constexpr address_t ocr0b_addr  = 0x48;
        static constexpr address_t tifr0_addr  = 0x35;

        // 19.9.1 TCCR0A: TC0 Control Register A

        namespace tccr0a
        {
            using whole = register8_t< rw_t, tccr0a_addr >;
            using com0a = bitfield8_t< rw_t, tccr0a_addr, COM0A1, COM0A0 >;
            using com0b = bitfield8_t< rw_t, tccr0a_addr, COM0B1, COM0B0 >;
            using wgm0  = bitfield8_t< rw_t, tccr0a_addr, WGM01 , WGM00  >;

            using value_type = whole::value_type;
        }

        // 19.9.2 TCCR0B: TC0 Control Register B

        namespace tccr0b
        {
            using whole = register8_t< rw_t, tccr0b_addr >;
            using foc0a = bitfield8_t< rw_t, tccr0b_addr, FOC0A >;
            using foc0b = bitfield8_t< rw_t, tccr0b_addr, FOC0B >;
            using wgm02 = bitfield8_t< rw_t, tccr0b_addr, WGM02 >;
            using cs0   = bitfield8_t< rw_t, tccr0b_addr, CS02, CS00 >;

            using value_type = whole::value_type;
        }

        // 19.9.3 TIMSK0: TC0 Interrupt Mask Register

        namespace timsk0
        {
            using whole  = register8_t< rw_t, timsk0_addr >;
            using ocie0b = bitfield8_t< rw_t, timsk0_addr, OCIE0B >;
            using ocie0a = bitfield8_t< rw_t, timsk0_addr, OCIE0A >;
            using toie0  = bitfield8_t< rw_t, timsk0_addr, TOIE0  >;

            using value_type = whole::value_type;
        }

        // 19.9.4 GTCCR: General Timer/Counter Control Register:
        // See 1. Timer/Counter 0, 1 Prescalers (tcp)

        namespace gtccr {}

        // 19.9.5 TCNT0: TC0 Counter Value Register

        namespace tcnt0
        {
            using whole = register8_t< rw_t, tcnt0_addr >;
            using value_type = whole::value_type;
        }

        // 19.9.6 OCR0A: TC0 Output Compare Register A

        namespace ocr0a
        {
            using whole = register8_t< rw_t, ocr0a_addr >;
            using value_type = whole::value_type;
        }

        // 19.9.7 OCR0B: TC0 Output Compare Register B

        namespace ocr0b
        {
            using whole = register8_t< rw_t, ocr0b_addr >;
            using value_type = whole::value_type;
        }

        // 19.9.8 TIFR0 : TC0 Interrupt Flag Register

        namespace tifr0
        {
            using whole = register8_t< rw_t, tifr0_addr >;
            using ocf0b = bitfield8_t< rc_w1_t, tifr0_addr, OCF0B >;
            using ocf0a = bitfield8_t< rc_w1_t, tifr0_addr, OCF0A >;
            using tov0  = bitfield8_t< rc_w1_t, tifr0_addr, TOV0  >;

            using value_type = whole::value_type;
        }
    }  // namespace reg

    namespace input
    {
        // clock source:

        enum class clock : uint8_t
        {
            none = 0        // no clock source, timer/counter stopped
            , x1            // no prescaling
            , x8            // clk / 8
            , x64           // clk / 64
            , x256          // clk / 256
            , x1024         // clk / 1024
            , ext1_falling_adge
            , ext1_rising_adge
        };

        inline auto current( clock )
        {
            return clock{ reg::tccr0b::cs0::read() };
        }

        inline auto set( clock clk )
        {
            return reg::tccr0b::cs0::write_lazy( to_integral(clk) );
        }
    } // namespace input

    namespace output
    {
        namespace a
        {
            // output action on compare-match:

            enum class mode : uint8_t
            {
                none = 0    // normal port operation, oc1a/b disconnected
                , toggle    // set output to high level then low level (?)
                , clear     // set output to low level (non-inverting mode)
                , set       // set output to high level (inverting mode)
            };

            inline auto current( mode )
            {
                return mode{ reg::tccr0a::com0a::read() };
            }

            inline auto set( mode m )
            {
                return reg::tccr0a::com0a::write_lazy( to_integral(m) );
            }
        }

        namespace b
        {
            // output action on compare-match:

            enum class mode : uint8_t
            {
                none = 0    // normal port operation, oc1a/b disconnected
                , toggle    // set output to high level then low level (?)
                , clear     // set output to low level (non-inverting mode)
                , set       // set output to high level (inverting mode)
            };

            inline auto current( mode )
            {
                return mode{ reg::tccr0a::com0b::read() };
            }

            inline auto set( mode m )
            {
                return reg::tccr0a::com0b::write_lazy( to_integral(m) );
            }
        }

        // waveforms (see tccr0a::wgm0, tccr0b::wgm0):

        enum class waveform : uint8_t
        {
            normal = 0
            , pwm_phase_correct = 1
            , ctc_ocr0a = 2, ctc = 2
            , fast_pwm = 3
            , reserved4 = 4
            , pwm_phase_correct_ocr0a = 5
            , reserved6 = 6
            , fast_pwm_ocr0a = 7
        };

        // waveform (wgm) setting is spread over tccr1a and tccr1b:

        inline auto current( waveform w )
        {
            return waveform( shl(reg::tccr0b::wgm02::read(), WGM02) + reg::tccr0a::wgm0::read() );
        }

        inline auto set( waveform w )
        {
            constexpr auto wg01_mask = bitmask<uint8_t>(WGM01, WGM00);

            return
                reg::tccr0a::wgm0::write(            to_integral(w) & wg01_mask ),
                reg::tccr0b::wgm02::write_lazy( shr( to_integral(w),  2 )       );
        }
    } // namespace output

    namespace control
    {
        namespace compare
        {
            namespace force
            {
                struct a{};
                struct b{};

                inline auto set( a )
                {
                    reg::tccr0b::foc0a::set();
                }

                inline auto set( b )
                {
                    reg::tccr0b::foc0b::set();
                }
            }
        }
    } // control

    namespace value
    {
        enum class timer : reg::tcnt0::value_type {};

        inline auto current( timer )
        {
            return reg::tcnt0::whole::read();
        }

        inline auto set( timer v )
        {
            return reg::tcnt0::whole::write( to_integral(v) );
        }

        namespace compare
        {
            enum class a : reg::ocr0a::value_type {};
            enum class b : reg::ocr0b::value_type {};

            inline auto current( a )
            {
                a{ reg::ocr0a::whole::read() };
            }

            inline auto set( a v )
            {
                reg::ocr0a::whole::write( to_integral(v) );
            }

            inline auto current( b )
            {
                b{ reg::ocr0b::whole::read() };
            }

            inline auto set( b v )
            {
                reg::ocr0b::whole::write( to_integral(v) );
            }
        }
    } // namespace value

    namespace enable
    {
        namespace interrupt
        {
            namespace on
            {
                enum class match_a  : uint8_t { off, on };
                enum class match_b  : uint8_t { off, on };
                enum class overflow : uint8_t { off, on };

                inline auto current( match_a )
                {
                     return match_a{ reg::timsk0::ocie0a::read() };
                }

                inline auto set( match_a e )
                {
                     return reg::timsk0::ocie0a::write_lazy( to_integral(e) );
                }

                inline auto current( match_b )
                {
                     return match_b{ reg::timsk0::ocie0b::read() };
                }

                inline auto set(match_b e )
                {
                     return reg::timsk0::ocie0b::write_lazy( to_integral(e) );
                }

                inline auto current( overflow )
                {
                     return overflow{ reg::timsk0::toie0::read() };
                }

                inline auto set( overflow e )
                {
                     return reg::timsk0::toie0::write_lazy( to_integral(e) );
                }
            }
        }
    } // namespace enable

    namespace interrupt
    {
        namespace compare
        {
            namespace flag
            {
                struct a{};
                struct b{};

                inline auto current( a )
                {
                    return 0 != reg::tifr0::ocf0a::read();
                }

                inline auto clear( a )
                {
                    reg::tifr0::ocf0a::clear();
                }

                inline auto current( b )
                {
                    return 0 != reg::tifr0::ocf0b::read();
                }

                inline auto clear( b )
                {
                    reg::tifr0::ocf0b::clear();
                }
            }
        }

        namespace overflow
        {
            struct flag{};

            inline auto current( flag )
            {
                return 0 != reg::tifr0::tov0::read();
            }

            inline auto clear( flag )
            {
                reg::tifr0::tov0::clear();
            }
        }
    } // namespace interrupt
}

// 20. 16-bit Timer/Counter1 (TC1) with PWM
// 20.15.1 TCCR1A  : TC1 Control Register A
// 20.15.2 TCCR1B  : TC1 Control Register B
// 20.15.3 TCCR1C  : TC1 Control Register C
// 20.15.4 TCNT1L/H: TC1 Counter Value Low and High byte
// 20.15.5 ICR1L/H : Input Capture Register 1 Low and High byte
// 20.15.6 OCR1AL/H: Output Compare Register 1 A Low and High byte
// 20.15.7 OCR1BL/H: Output Compare Register 1 B Low and High byte
// 20.15.8 TIMSK1  : Timer/Counter 1 Interrupt Mask Register
// 20.15.9 TIFR1   : TC1 Interrupt Flag Register

namespace tc1
{
    namespace reg
    {
        // register addresses:

        static constexpr address_t tccr1a_addr = 0x80;
        static constexpr address_t tccr1b_addr = 0x81;
        static constexpr address_t tccr1c_addr = 0x82;
        static constexpr address_t tcnt1_addr  = 0x84;
        static constexpr address_t tcnt1l_addr = tcnt1_addr + 0;
        static constexpr address_t tcnt1h_addr = tcnt1_addr + 1;
        static constexpr address_t icr1_addr   = 0x86;
        static constexpr address_t icr1l_addr  = icr1_addr + 0;
        static constexpr address_t icr1h_addr  = icr1_addr + 1;
        static constexpr address_t ocr1a_addr  = 0x88;
        static constexpr address_t ocr1al_addr = ocr1a_addr + 0;
        static constexpr address_t ocr1ah_addr = ocr1a_addr + 1;
        static constexpr address_t ocr1b_addr  = 0x8a;
        static constexpr address_t ocr1bl_addr = ocr1b_addr + 0;
        static constexpr address_t ocr1bh_addr = ocr1b_addr + 1;
        static constexpr address_t timsk1_addr = 0x6f;
        static constexpr address_t tifr1_addr  = 0x36;

        // 20.15.1 TCCR0A: TC1 Control Register A

        namespace tccr1a
        {
            using whole = register8_t< rw_t, tccr1a_addr >;
            using com1a = bitfield8_t< rw_t, tccr1a_addr, COM1A1, COM1A0 >;
            using com1b = bitfield8_t< rw_t, tccr1a_addr, COM1B1, COM1B0 >;
            using wgm1  = bitfield8_t< rw_t, tccr1a_addr, WGM11 , WGM10  >;

            using value_type = whole::value_type;
        }

        // 20.15.2 TCCR0B  : TC1 Control Register B

        namespace tccr1b
        {
            using whole = register8_t< rw_t, tccr1b_addr >;
            using icnc1 = bitfield8_t< rw_t, tccr1b_addr, ICNC1 >;
            using ices1 = bitfield8_t< rw_t, tccr1b_addr, ICES1 >;
            using wgm1  = bitfield8_t< rw_t, tccr1b_addr, WGM13, WGM12 >;
            using cs1   = bitfield8_t< rw_t, tccr1b_addr, CS12 , CS10  >;

            using value_type = whole::value_type;
        }

        // 20.15.3 TCCR0C: TC1 Control Register C

        namespace tccr1c
        {
            using whole = register8_t< rw_t, tccr1c_addr >;
            using foc1a = bitfield8_t< rw_t, tccr1a_addr, FOC1A >;
            using foc1b = bitfield8_t< rw_t, tccr1a_addr, FOC1B >;

            using value_type = whole::value_type;
        }

        // 20.15.4 TCNT1L/H: TC1 Counter Value Low and High byte

        namespace tcnt1
        {
            using whole = register16_t< rw_t, tcnt1_addr  >;
            using lo    =  register8_t< rw_t, tcnt1l_addr >;
            using hi    =  register8_t< rw_t, tcnt1h_addr >;

            using value_type = whole::value_type;
        }

        // 20.15.5 ICR1L/H: Input Capture Register 1 Low and High byte

        namespace icr1
        {
            using whole = register16_t< rw_t, icr1_addr  >;
            using lo    =  register8_t< rw_t, icr1l_addr >;
            using hi    =  register8_t< rw_t, icr1h_addr >;

            using value_type = whole::value_type;
        }

        // 20.15.6 OCR1AL/H: Output Compare Register 1 A Low and High byte

        namespace ocr1a
        {
            using whole = register16_t< rw_t, ocr1a_addr  >;
            using lo    =  register8_t< rw_t, ocr1al_addr >;
            using hi    =  register8_t< rw_t, ocr1ah_addr >;

            using value_type = whole::value_type;
        }

        // 20.15.7 OCR1BL/H: Output Compare Register 1 B Low and High byte

        namespace ocr1b
        {
            using whole = register16_t< rw_t, ocr1b_addr  >;
            using lo    =  register8_t< rw_t, ocr1bl_addr >;
            using hi    =  register8_t< rw_t, ocr1bh_addr >;

            using value_type = whole::value_type;
        }

        // 20.15.8 TIMSK1: Timer/Counter 1 Interrupt Mask Register

        namespace timsk1
        {
            using whole  = register8_t< rw_t, timsk1_addr >;
            using icie1  = bitfield8_t< rw_t, timsk1_addr, ICIE1  >;
            using ocie1b = bitfield8_t< rw_t, timsk1_addr, OCIE1B >;
            using ocie1a = bitfield8_t< rw_t, timsk1_addr, OCIE1A >;
            using toie1  = bitfield8_t< rw_t, timsk1_addr, TOIE1  >;

            using value_type = whole::value_type;
        }

        // 20.15.9 TIFR1: TC1 Interrupt Flag Register

        namespace tifr1
        {
            using whole = register8_t< rc_w1_t, tifr1_addr >;
            using icf1  = bitfield8_t< rc_w1_t, tifr1_addr, ICF1  >;
            using ocf1b = bitfield8_t< rc_w1_t, tifr1_addr, OCF1B >;
            using ocf1a = bitfield8_t< rc_w1_t, tifr1_addr, OCF1A >;
            using tov1  = bitfield8_t< rc_w1_t, tifr1_addr, TOV1  >;

            using value_type = whole::value_type;
        }
    } // namespace reg

    namespace input
    {
        // clock source:

        enum class clock : uint8_t
        {
            none = 0        // no clock source, timer/counter stopped
            , x1            // no prescaling
            , x8            // clk / 8
            , x64           // clk / 64
            , x256          // clk / 256
            , x1024         // clk / 1024
            , ext1_falling_adge
            , ext1_rising_adge
        };

        inline auto current( clock )
        {
            return clock{ reg::tccr1b::cs1::read() };
        }

        inline auto set( clock clk )
        {
            return reg::tccr1b::cs1::write_lazy( to_integral(clk) );
        }

        namespace capture
        {
            enum class edge :uint8_t { falling, rising };

            inline auto current( edge )
            {
                return edge{ reg::tccr1b::ices1::read() };
            }

            inline auto set( edge e )
            {
                return reg::tccr1b::ices1::write_lazy( to_integral(e) );
            }
        }
    }

    namespace output
    {
        namespace a
        {
            // output action on compare-match:

            enum class mode : uint8_t
            {
                none = 0    // normal port operation, oc1a/b disconnected
                , toggle    // set output to high level then low level (?)
                , clear     // set output to low level (non-inverting mode)
                , set       // set output to high level (inverting mode)
            };

            inline auto current( mode )
            {
                return mode{ reg::tccr1a::com1a::read() };
            }

            inline auto set( mode m )
            {
                return reg::tccr1a::com1a::write_lazy( to_integral(m) );
            }
        }

        namespace b
        {
            // output action on compare-match:

            enum class mode : uint8_t
            {
                none = 0    // normal port operation, oc1a/b disconnected
                , toggle    // set output to high level then low level (?)
                , clear     // set output to low level (non-inverting mode)
                , set       // set output to high level (inverting mode)
            };

            inline auto current( mode )
            {
                return mode{ reg::tccr1a::com1b::read() };
            }

            inline auto set( mode m )
            {
                return reg::tccr1a::com1b::write_lazy( to_integral(m) );
            }
        }

        // waveform (see tccr1a::wgm1, tccr1b::wgm1):

        enum class waveform : uint8_t
        {
            normal = 0
            , pwm_phase_correct_8bit = 1
            , pwm_phase_correct_9bit = 2
            , pwm_phase_correct_10bit = 3
            , ctc_ocr1a = 4
            , fast_pwm_phase_correct_8bit = 5
            , fast_pwm_phase_correct_9bit = 6
            , fast_pwm_phase_correct_10bit = 7
            , pwm_phase_frequency_correct_icr1 = 8
            , pwm_phase_frequency_correct_ocr1a = 9
            , pwm_phase_correct_icr1 = 10
            , pwm_phase_correct_ocr1a = 11
            , ctc_icr1 = 12
            , reserved13 = 13
            , fast_pwm_icr1 = 14
            , fast_pwm_ocr1a = 15
        };

        // waveform (wgm) setting is spread over tccr1a and tccr1b:

        inline auto current( waveform )
        {
            return waveform( shl(reg::tccr1b::wgm1::read(), WGM12) + reg::tccr1a::wgm1::read() );
        }

        inline auto set( waveform w )
        {
            constexpr auto wg01_mask = bitmask<uint8_t>(WGM11, WGM10);

            return
                reg::tccr1a::wgm1::write(           to_integral(w) & wg01_mask ),
                reg::tccr1b::wgm1::write_lazy( shr( to_integral(w),  2 )       );
        }
    }

    namespace control
    {
        namespace compare
        {
            namespace force
            {
                struct a{};
                struct b{};

                inline auto set( a )
                {
                    reg::tccr1c::foc1a::set();
                }

                inline auto set( b )
                {
                    reg::tccr1c::foc1b::set();
                }
            }
        }
    }

    namespace value
    {
        enum class timer : reg::tcnt1::value_type {};

        inline auto current( timer )
        {
            return reg::tcnt1::whole::read();
        }

        inline auto set( timer v )
        {
            return reg::tcnt1::whole::write( to_integral(v) );
        }

        namespace compare
        {
            enum class a : reg::ocr1a::value_type {};
            enum class b : reg::ocr1b::value_type {};

            inline auto current( a )
            {
                a{ reg::ocr1a::whole::read() };
            }

            inline auto set( a v )
            {
                reg::ocr1a::whole::write( to_integral(v) );
            }

            inline auto current( b )
            {
                b{ reg::ocr1b::whole::read() };
            }

            inline auto set( b v )
            {
                reg::ocr1b::whole::write( to_integral(v) );
            }
        }
    }

    namespace enable
    {
        namespace capture
        {
            // ToDo: enable?
            inline auto input_capture_noise_canceler()
            {
                return reg::tccr1b::icnc1::read();
            }

            inline auto input_capture_noise_canceler( bool on )
            {
                return reg::tccr1b::icnc1::write_lazy( on );
            }
        }

        namespace interrupt
        {
            namespace on
            {
                enum class capture  : uint8_t { off, on };
                enum class match_a  : uint8_t { off, on };
                enum class match_b  : uint8_t { off, on };
                enum class overflow : uint8_t { off, on };

                inline auto current( capture )
                {
                    return capture{ reg::timsk1::icie1::read() };
                }

                inline auto set( capture e )
                {
                    return reg::timsk1::icie1::write_lazy( to_integral(e) );
                }

                inline auto current( match_a )
                {
                     return match_a{ reg::timsk1::ocie1a::read() };
                }

                inline auto set( match_a e )
                {
                     return reg::timsk1::ocie1a::write_lazy( to_integral(e) );
                }

                inline auto current( match_b )
                {
                     return match_b{ reg::timsk1::ocie1b::read() };
                }

                inline auto set(match_b e )
                {
                     return reg::timsk1::ocie1b::write_lazy( to_integral(e) );
                }

                inline auto current( overflow )
                {
                     return overflow{ reg::timsk1::toie1::read() };
                }

                inline auto set( overflow e )
                {
                     return reg::timsk1::toie1::write_lazy( to_integral(e) );
                }
            }
        }
    }

    namespace interrupt
    {
        namespace capture
        {
            struct flag{};

            inline auto current( flag )
            {
                return 0 != reg::tifr1::icf1::read();
            }

            inline auto clear( flag )
            {
                reg::tifr1::icf1::clear();
            }
        }

        namespace compare
        {
            namespace flag
            {
                struct a{};
                struct b{};

                inline auto current( a )
                {
                    return 0 != reg::tifr1::ocf1a::read();
                }

                inline auto clear( a )
                {
                    reg::tifr1::ocf1a::clear();
                }

                inline auto current( b )
                {
                    return 0 != reg::tifr1::ocf1b::read();
                }

                inline auto clear( b )
                {
                    reg::tifr1::ocf1b::clear();
                }
            }
        }

        namespace overflow
        {
            struct flag{};

            inline auto current( flag )
            {
                return 0 != reg::tifr1::tov1::read();
            }

            inline auto clear( flag )
            {
                reg::tifr1::tov1::clear();
            }
        }
    }
}

// 21. Timer/Counter 0, 1 Prescalers
// 21.4.1 GTCCR: General Timer/Counter Control Register

namespace tcp
{
    namespace reg
    {
        static constexpr address_t gtccr_addr = 0x43;

        // 21.4.1 GTCCR: General Timer/Counter Control Register

        namespace gtccr
        {
            using whole   = register8_t< rw_t, gtccr_addr >;
            using tsm     = bitfield8_t< rw_t, gtccr_addr, TSM >;
            using psrasy  = bitfield8_t< rw_t, gtccr_addr, PSRASY >;
            using psrsync = bitfield8_t< rw_t, gtccr_addr, PSRSYNC >;

            using value_type = whole::value_type;
        }

    } // namespace reg

    enum class synchronization : uint8_t { off, on };

    inline auto current( synchronization )
    {
        return synchronization{ reg::gtccr::tsm::read() };
    }

    inline auto set( synchronization e )
    {
        return reg::gtccr::tsm::write_lazy( to_integral(e) );
    }

    namespace reset
    {
        namespace prescaler
        {
            enum class tc2  : uint8_t { off, on };
            enum class tc01 : uint8_t { off, on };

            inline auto current( tc2 )
            {
                return tc2{ reg::gtccr::psrasy::read() };
            }

            inline auto set( tc2 e )
            {
                return reg::gtccr::psrasy::write_lazy( to_integral(e) );
            }

            inline auto current( tc01 )
            {
                return tc01{ reg::gtccr::psrsync::read() };
            }

            inline auto set( tc01 e )
            {
                return reg::gtccr::psrsync::write_lazy( to_integral(e) );
            }
        }
    }
}

// 22. 8-bit Timer/Counter2 (TC2) with PWM and Asynchronous Operation
// 22.11.1 TCCR2A: TC2 Control Register A
// 22.11.2 TCCR2B: TC2 Control Register B
// 22.11.3 TCNT2 : TC2 Counter Value Register
// 22.11.4 OCR2A : TC2 Output Compare Register A
// 22.11.5 OCR2B : TC2 Output Compare Register B
// 22.11.6 TIMSK2: TC2 Interrupt Mask Register
// 22.11.7 TIFR2 : TC2 Interrupt Flag Register
// 22.11.8 ASSR  : Asynchronous Status Register
// 22.11.9 GTCCR : General Timer/Counter Control Register

namespace tc2
{
    namespace reg
    {
        static constexpr address_t tccr2a_addr = 0xb0;
        static constexpr address_t tccr2b_addr = 0xb1;
        static constexpr address_t tcnt2_addr  = 0xb2;
        static constexpr address_t ocr2a_addr  = 0xb3;
        static constexpr address_t ocr2b_addr  = 0xb4;
        static constexpr address_t timsk2_addr = 0x70;
        static constexpr address_t tifr2_addr  = 0x37;
        static constexpr address_t assr_addr   = 0xb6;
        static constexpr address_t gtccr_addr  = 0x43;

        namespace tccr2a
        {
        }

        namespace tccr2b
        {
        }

        namespace tcnt2
        {
        }

        namespace ocr2a
        {
        }

        namespace ocr2b
        {
        }

        namespace timsk2
        {
        }

        namespace tifr2
        {
        }

        namespace assr
        {
        }

        namespace gtccr
        {
        }

        namespace gtccr
        {
        }
    } // namespace reg
}

// 23. SPI - Serial Peripheral Interface (SPI)
//
// 23.5.1 SPCR0: SPI Control Register 0
// 23.5.2 SPSR0: SPI Status Register 0
// 23.5.3 SPDR0: SPI Data Register 0

namespace spi
{
    namespace reg
    {
    } // namespace reg
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
    namespace reg
    {
    } // namespace reg
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
    namespace reg
    {
        static constexpr address_t addr_twbr  = 0xb8;
        static constexpr address_t addr_twsr  = 0xb9;
        static constexpr address_t addr_twar  = 0xba;
        static constexpr address_t addr_twdr  = 0xbb;
        static constexpr address_t addr_twcr  = 0xbc;
        static constexpr address_t addr_twamr = 0xbd;

        using twbr  = register8_t< rw_t, addr_twbr >;
        using twsr  = register8_t< rw_t, addr_twsr >;
        using twar  = register8_t< rw_t, addr_twar >;
        using twdr  = register8_t< rw_t, addr_twdr >;
        using twcr  = register8_t< rw_t, addr_twcr >;
        using twamr = register8_t< rw_t, addr_twamr >;
    } // namespace reg
}

// 27. AC - Analog Comparator (AC)
//
// 27.3.1 ACSR : Analog Comparator Control and Status Register
// 27.3.2 DIDR1: Digital Input Disable Register 1

namespace ac
{
    namespace reg
    {
        // register addresses:

        static constexpr address_t addr_acsr  = 0x50;
        static constexpr address_t addr_didr1 = 0x7f;

        // 27.3.1 ACSR : Analog Comparator Control and Status Register

        namespace acsr
        {
            using whole = register8_t< rw_t, addr_acsr >;
            using acd   = bitfield8_t< rw_t, addr_acsr, ACD  >;
            using acbg  = bitfield8_t< rw_t, addr_acsr, ACBG >;
            using aco   = bitfield8_t< r_t , addr_acsr, ACO  >;
            using aci   = bitfield8_t< rc_w1_t, addr_acsr, ACI  >;
            using acie  = bitfield8_t< rw_t, addr_acsr, ACIE >;
            using acic  = bitfield8_t< rw_t, addr_acsr, ACIC >;
            using acis  = bitfield8_t< rw_t, addr_acsr, ACIS1, ACIS0 >;

            using value_type = whole::value_type;
        }

        // 27.3.2 DIDR1: Digital Input Disable Register 1
        // exampple: adc::didr1::write( bitmask( AIN1D, AIN0D ) );

        using didr1 = bitfield8_t< rw_t, addr_didr1, AIN1D, AIN0D >;
    } // namespace reg


    namespace input
    {
        namespace select
        {
            enum class bandgap_ref: uint8_t { off, on };

            inline auto current( bandgap_ref )
            {
                return bandgap_ref{ reg::acsr::acbg::read() };
            }

            inline auto set( bandgap_ref e )
            {
                 return reg::acsr::acbg::write_lazy( to_integral(e) );
            }
        }
    }

    namespace output
    {
        struct state{};

        inline auto current( state )
        {
            return reg::acsr::aco::read();
        }

        namespace interrupt
        {
            // output interrupt mode:

            enum class on : uint8_t
            {
                toggle = 0      // comparator interrupt on output toggle
                , reserved1     // -
                , falling_edge  // comparator interrupt on falling output edge
                , rising_edge   // comparator interrupt on rising output edge
            };

            inline auto current( on )
            {
                return on{ reg::acsr::acis::read() };
            }

            inline auto set( on m )
            {
                return reg::acsr::acis::write_lazy( to_integral(m) );
            }
        }
    }

    namespace enable
    {
        enum class interrupt : uint8_t { off, on };

        inline auto current( interrupt )
        {
            return interrupt{ reg::acsr::acie::read() };
        }

        inline auto set( interrupt e )
        {
            return reg::acsr::acie::write_lazy( to_integral(e) );
        }

        namespace input
        {
            enum class capture : uint8_t { off, on };

            inline auto current( capture )
            {
                return capture{ reg::acsr::acic::read() };
            }

            inline auto set( capture e )
            {
                return reg::acsr::acic::write_lazy( to_integral(e) );
            }
        }
    }

    namespace disable
    {
        namespace analog
        {
            enum class comparator: uint8_t { off, on };

            inline auto current( comparator )
            {
                return comparator{ reg::acsr::acd::read() };
            }

            inline auto set( comparator e )
            {
                 return reg::acsr::acd::write_lazy( to_integral(e) );
            }
        }
    }

    namespace interrupt
    {
        struct flag{};

        inline auto current( flag )
        {
            return 0 != reg::acsr::aci::read();
        }

        inline auto clear( flag )
        {
            reg::acsr::aci::clear();
        }
    }
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
    namespace reg
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

        // adc: ADCL and ADCH - The ADC Data Register:

        namespace adc
        {
            // see also admux::adjust()

            using whole = register16_t< r_t, addr_adcl >;   // 16-bit result
            using adchl = register16_t< r_t, addr_adcl >;   // 16-bit result
            using adcl  =  register8_t< r_t, addr_adcl >;   //  8-bit lsb
            using adch  =  register8_t< r_t, addr_adch >;   //  2-bit msb

            using value_type = whole::value_type;
        }

        // admux - ADC Multiplexer Selection Register:

        namespace admux
        {
            using whole = register8_t< rw_t, addr_admux >;
            using refs  = bitfield8_t< rw_t, addr_admux, REFS1, REFS0 >; // voltage reference
            using adlar = bitfield8_t< rw_t, addr_admux, ADLAR        >; // left adjust result
            using mux   = bitfield8_t< rw_t, addr_admux, MUX3 , MUX0  >; // input channel

            using value_type = whole::value_type;
        }

        // adcsra - ADC Control and Status Register A:

        namespace adcsra
        {
            using whole = register8_t< rw_t, addr_adcsra >;
            using aden  = bitfield8_t< rw_t   , addr_adcsra, ADEN >;        // ADC Enable
            using adsc  = bitfield8_t< rw_t   , addr_adcsra, ADSC >;        // ADC Start Conversion
            using adate = bitfield8_t< rw_t   , addr_adcsra, ADATE>;        // ADC Auto Trigger Enable
            using adif  = bitfield8_t< rc_w1_t, addr_adcsra, ADIF >;        // ADC Interrupt Flag
            using adie  = bitfield8_t< rw_t   , addr_adcsra, ADIE >;        // ADC Interrupt Enable
            using adps  = bitfield8_t< rw_t   , addr_adcsra, ADPS2, ADPS0 >;// ADC Prescaler Select Bits

            using value_type = whole::value_type;
        }

        // adcsrb - ADC Control and Status Register B:

        namespace adcsrb
        {
            using whole = register8_t< rw_t, addr_adcsrb >;
            using acme  = bitfield8_t< rw_t, addr_adcsrb, ACME         >;    // ADC Analog Comparator Multiplexer Enabled
            using adts  = bitfield8_t< rw_t, addr_adcsrb, ADTS2, ADTS0 >;    // ADC Auto Trigger Source

            using value_type = whole::value_type;
        }

            // didr0 - Digital Input Disable Register 0:
        // exampple: adc::didr0::write( bitmask( ADC1D, ADC0D ) );

        using didr0 = bitfield8_t< rw_t, addr_didr0, ADC5D, ADC0D >;

        // didr1 - Digital Input Disable Register 1:
        // exampple: adc::didr1::write( bitmask( AIN1D, AIN0D ) );

        using didr1 = ac::reg::didr1;
    }

    namespace conversion
    {
        // adc conversion result type, 10-bit resolution:

        using result_t = reg::adc::value_type;

        constexpr auto result_min = result_t{ 0b0 };
        constexpr auto result_max = result_t{ 0b11'1111'1111 };

        // admux adc result adjustment:

        enum class adjustment : uint8_t
        {
            right = 0
            , left
        };

        inline auto start()
        {
            return reg::adcsra::adsc::write_lazy( true );
        }

        inline auto result()
        {
            return reg::adc::adchl::read();
        }

        inline auto lsb()
        {
            return reg::adc::adcl::read();
        }

        inline auto msb()
        {
            return reg::adc::adch::read();
        }

        inline auto current( adjustment )
        {
            return adjustment{ reg::admux::adlar::read() };
        }

        inline auto set( adjustment a )
        {
            return reg::admux::adlar::write_lazy( to_integral(a) );
        }
    }

    namespace input
    {
        // admux voltage reference:

        enum class reference: uint8_t
        {
            aref = 0
            , avcc
            , reserved
            , intern_1v1
        };

        // admux input channel:

        enum class channel: uint8_t
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

        inline auto current( reference )
        {
            return reference{ reg::admux::refs::read() };
        }

        inline auto set( reference r )
        {
            return reg::admux::refs::write_lazy( to_integral(r) );
        }

        inline auto current( channel )
        {
            return channel{ reg::admux::mux::read() };
        }

        inline auto set( channel c )
        {
            return reg::admux::mux::write_lazy( to_integral(c) );
        }
    }

    namespace enable
    {
        enum class adc : uint8_t { off, on };
        enum class interrupt : uint8_t { off, on };
        enum class comparator: uint8_t { off, on };
        enum class auto_trigger : uint8_t { off, on };

        inline auto current( adc )
        {
            return adc{ reg::adcsra::aden::read() };
        }

        inline auto set( adc e )
        {
            return reg::adcsra::aden::write_lazy( to_integral(e) ) ;
        }

        inline auto current( interrupt )
        {
            return interrupt{ reg::adcsra::adie::read() };
        }

        inline auto set( interrupt e )
        {
            return reg::adcsra::adie::write_lazy( to_integral(e) );
        }

        inline auto current( auto_trigger )
        {
            return auto_trigger{ reg::adcsra::adate::read() };
        }

        inline auto set( auto_trigger e )
        {
            return reg::adcsra::adate::write_lazy( to_integral(e) );
        }

        inline auto current( comparator )
        {
            return comparator{ reg::adcsrb::acme::read() };
        }

        inline auto set( comparator e )
        {
             return reg::adcsrb::acme::write_lazy( to_integral(e) );
        }
    }

    namespace interrupt
    {
        struct flag{};

        inline auto current( flag )
        {
            return 0 != reg::adcsra::adif::read();
        }

        inline auto clear( flag )
        {
            reg::adcsra::adif::clear();
        }
    }

    namespace trigger
    {
        // adcsrb trigger source:

        enum class on : uint8_t
        {
            free_run = 0
            , ac
            , ei0
            , tc0_match_a
            , tc0_overflow
            , tc1_match_b
            , tc1_overflow
            , tc1_capture_event
        };

        inline auto current( on )
        {
            return on{ reg::adcsrb::adts::read() };
        }

        inline auto set( on evt )
        {
            return reg::adcsrb::adts::write_lazy( to_integral(evt) );
        }
    }

    namespace prescale
    {
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

        inline auto current( factor )
        {
            return factor{ reg::adcsra::adps::read() };
        }

        inline auto set( factor f )
        {
            return reg::adcsra::adps::write_lazy( to_integral(f) );
        }
    }
}

// debugWIRE On-chip Debug System:
//
// 25.6.1 - DWDR: debugWire Data Register

namespace dbgw
{
    namespace reg
    {
    } // namespace reg
}

// 30. Boot Loader Support - Read-While-Write Self-programming (BTLDR)
//
// 30.9.1 SPMCSR: Store Program Memory Control and Status Register (SPMCSR)

namespace spf  // self-programming flash
{
    namespace reg
    {
    } // namespace reg
}

// Convenience types:

// LED on port, pin:

template< port portid, uint8_t pin >
struct led
{
    using led_t = typename gpio<portid>::template p<pin>;

    static inline void on()     { led_t::set::set();    }
    static inline void off()    { led_t::set::clear();  }
    static inline void toggle() { led_t::set::toggle(); }
    static inline void enable() { led_t::dir::set();    }
};

// 10-bit PWM on timer 1, output a (tc1, PB1):

struct tc1_pwm_on_a_pb1
{
    using value_type = uint16_t;

    static constexpr auto value_min = value_type{         0b0011 };
    static constexpr auto value_max = value_type{ 0b11'1111'1111 };

    // init timer 1 for PWM-dac timing:

    static inline auto init()
    {
        gpio<port::B>::p<1>::dir::set();

        // register tccr1a (non-inverting mode):
        set( tc1::output::a::mode::clear );

        // register tccr1b:
        set( tc1::output::waveform::fast_pwm_phase_correct_10bit )
        , set( tc1::input::clock::x1 )
            ;
    }

    // output PWM value:

    static inline auto write( value_type value )
    {
        mcu::atomic( OCR1A, value );
    }
};

} // namespace atmega328
} // namespace mcu

#endif // MCU_ATMEGA328_HPP_INCLUDED
