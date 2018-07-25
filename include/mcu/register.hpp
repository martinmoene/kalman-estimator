//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//
// Types bitfield and register are inspired on the work by:
// - Ken Smith. C++ Hardware Register Access. 2010. [1]
// - Mike Franklin. A memory-mapped IO library written in the D programming language. 2014. [2]
//
// Both are mentioned by Niklas Hauser in Typesafe Register Access in C++. 2015. [3]
//
// [1] https://github.com/kensmith/cppmmio
// [2] https://github.com/JinShil/memory_mapped_io
// [3] https://blog.salkinium.com/typesafe-register-access-in-c++/

#ifndef MCU_REGISTER_HPP_INCLUDED
#define MCU_REGISTER_HPP_INCLUDED

#include "std/limits.hpp"
#include "std/type_traits.hpp"

#include <avr/io.h>
#include <avr/interrupt.h>

namespace mcu {

// Types:

using address_t = uint8_t;
using index_t   = uint8_t;
using size_t    = uint8_t;

// A bit mask, e.g. bitmask<uint8_t>( 6, 5, 0 ):

template< typename T >
constexpr uint8_t bits()
{
    return __CHAR_BIT__ * sizeof(T);
}


// A bit mask, e.g. bitmask<uint8_t>( 6, 5, 0 ):

template< typename T, typename ...Args >
constexpr T bitmask( Args&&... args )
{
    return static_cast<T>( ((1 << args) | ...) );
}

// An inverted bit mask, e.g. bitmask_n<uint8_t>( 6, 5, 0 ):

template< typename T, typename ...Args >
constexpr T bitmask_n( Args&&... args )
{
    return ~bitmask<T>( args...);
}

// A bit mask created from a range, e.g. rngmask<uint8_t>( 3, 0 ):

template< typename T >
constexpr T rngmask( index_t hi, index_t lo )
{
    T result = 0;
    for( index_t i = lo; i <= hi; ++i )
    {
        result |= bitmask<T>( i );
    }
    return result;
}

// An inverted bit mask created from a range, e.g. rngmask_n<uint8_t>( 3, 0 ):

template< typename T >
constexpr T rngmask_n( index_t hi, index_t lo )
{
    return ~rngmask<T>( hi, lo );
}

// T shift-left:

template< typename T >
constexpr T shl( T value, size_t n )
{
    return value << n;
}

// T shift-right:

template< typename T >
constexpr T shr( T value, size_t n )
{
    return value >> n;
}

// Address as volatile pointer:

template< typename T >
auto to_pointer( address_t address )
{
    return reinterpret_cast<T volatile *>( address );
}

// Variations how a bit can be mutated:

enum class Mutability
{
    rw,         // read and write bit
    r,          // read bit
    w,          // write bit, read returns reset value
    rc_w1,      // read bit, clear bit by write 1
    rc_w0,      // read bit, clear bit by write 0
    rc_r,       // read bit, clear bit by read
    rs,         // read bit, set bit
    rt_w,       // read bit, trigger event buwrite bit triggers event
};

//

constexpr auto can_read( Mutability m )
{
    return m == Mutability::r     || m == Mutability::rw
        || m == Mutability::rt_w  || m == Mutability::rs
        || m == Mutability::rc_r  || m == Mutability::rc_w0
        || m == Mutability::rc_w1;
}

//

constexpr auto can_write( Mutability m )
{
    return m == Mutability::w     || m == Mutability::rw
        || m == Mutability::rc_w0 || m == Mutability::rc_w1
        || m == Mutability::rs;
}

//

constexpr auto can_only_set_or_clear( Mutability m )
{
    return m == Mutability::rc_w0 || m == Mutability::rc_w1
        || m == Mutability::rs;
}

//

constexpr auto is_for_bits_only( Mutability m )
{
    return m == Mutability::rc_w0 || m == Mutability::rc_w1
        || m == Mutability::rs    || m == Mutability::rc_r
        || m == Mutability::rt_w;
}

//

constexpr auto is_clear( Mutability m )
{
    return m == Mutability::rc_w0 || m == Mutability::rc_w1;
}

// Bit-access implementations:

namespace detail {

// Read bit:

template< typename T >
struct getbit_t
{
    static auto get( address_t address, index_t bit )
    {
        return static_cast<T>( *to_pointer<T>(address) & bitmask<T>( bit ) );
    }
};

// Set bit:

template< typename T >
struct setbit_t
{
    static void set( address_t address, index_t bit )
    {
       *to_pointer<T>(address) = static_cast<T>( *to_pointer<T>(address) | bitmask<T>( bit ) );
    }
};

// Toggle bit:

template< typename T >
struct togglebit_t
{
    static void toggle( address_t address, index_t bit )
    {
       *to_pointer<T>(address) = static_cast<T>( *to_pointer<T>(address) ^ bitmask<T>( bit ) );
    }
};

// Clear bit by write 1:

template< typename T >
struct clearbit_w1_t
{
    static void clear( address_t address, index_t bit )
    {
        *to_pointer<T>(address) = static_cast<T>( *to_pointer<T>(address) | bitmask<T>( bit ) );
    }
};

// Clear bit by write 0:

template< typename T >
struct clearbit_w0_t
{
    static void clear( address_t address, index_t bit )
    {
        *to_pointer<T>(address) = *to_pointer<T>(address) & bitmask_n<T>( bit );
    }
};

// Trigger bit by write:

template< typename T >
struct trigger_t
{
    static void trigger( address_t address, index_t bit )
    {
       *to_pointer<T>(address) = *to_pointer<T>(address) | bitmask<T>( bit );
    }
};

} // namespace detail

// Read bits:

template< typename T >
struct r_t : detail::getbit_t<T>
{
    static constexpr Mutability mutability = Mutability::r;

    static auto read( address_t address, T mask )
    {
        return static_cast<T>( *to_pointer<T>(address) & mask );
    }
};

// Write bits:

template< typename T >
struct w_t : detail::setbit_t<T>, detail::clearbit_w0_t<T>
{
    static constexpr Mutability mutability = Mutability::w;

    static void write( address_t address, T value )
    {
        *to_pointer<T>(address) = value;
    }

    static void write( address_t address, T mask, T value )
    {
        *to_pointer<T>(address) = ( *to_pointer<T>(address) & ~mask ) | ( value & mask );
    }
};

// Read and write bits:

template< typename T >
struct rw_t : r_t<T>, w_t<T>, detail::togglebit_t<T>
{
    static constexpr Mutability mutability = Mutability::rw;
};

// Read bit, clear bit by write 1:

template< typename T >
struct rc_w1_t : detail::getbit_t<T>, detail::clearbit_w1_t<T>
{
    static constexpr Mutability mutability = Mutability::rc_w1;
};

// Read bit, clear bit by write 0:

template< typename T >
struct rc_w0_t : detail::getbit_t<T>, detail::clearbit_w0_t<T>
{
    static constexpr Mutability mutability = Mutability::rc_w0;
};

// Read bit, clear bit by read:

template< typename T >
struct rc_r_t : detail::getbit_t<T>
{
    static constexpr Mutability mutability = Mutability::rc_r;
};

// Read bit, set bit:

template< typename T >
struct rs_t : detail::getbit_t<T>, detail::setbit_t<T>
{
    static constexpr Mutability mutability = Mutability::rs;
};

// Read bit, trigger event by write:

template< typename T >
struct rt_w : detail::getbit_t<T>, detail::trigger_t<T>
{
    static constexpr Mutability mutability = Mutability::rt_w;
};

// Collect and write bits:

template< typename T, typename io, address_t address, T clear_mask, T set_mask >
struct write_proxy
{
    const T value;
    bool  apply = true;

    // output data, unless discarded:

    ~write_proxy()
    {
        if ( apply )
        {
            io::write( address, clear_mask, value );
        }
    }

    // inhibit writing data:

    void discard()
    {
        apply = false;
    }
};

// Combine lazy writes via comma operator:

template
<
    typename A
    , typename B
    , typename PA
    , typename PB
    , address_t address_a
    , address_t address_b
    , A clear_mask_a
    , B clear_mask_b
    , A set_mask_a
    , B set_mask_b
>
auto operator,
(
    write_proxy< A, PA, address_a, clear_mask_a, set_mask_a > a,
    write_proxy< B, PB, address_b, clear_mask_b, set_mask_b > b
)
{
    static_assert(   address_a == address_b, "write: Only combine writes to the same register.");
    static_assert( std20::is_same_v< A,  B>, "write: Only combine writes with the same data type.");
    static_assert( std20::is_same_v<PA, PB>, "write: Only combine writes with the same io policy.");

    a.discard();
    b.discard();

    return write_proxy
    <
        A, PA, address_a, clear_mask_a | clear_mask_b, set_mask_a | set_mask_b
    >
    {
        static_cast<A>( a.value | b.value )
    };
}

// A field of bits:

template< typename T, template <typename> typename io_t, address_t address, index_t hi, index_t lo = hi >
class bitfield_t
{
    using io = io_t<T>;

public:
    using value_type = T;

    // Read a field, special case single bit read:

    static auto read()
    {
        // static_assert( can_read( io::mutability ), "field: read not supported." );

        if constexpr( is_single_bit() )
        {
            return io::get( address, hi );
        }
        else
        {
            return shr( io::read( address, rngmask<T>(hi,lo) ), lo );
        }
    }

    // Write a field, special case single bit write:

    static void write( value_type value )
    {
        // static_assert(  can_write( io::mutability ), "field: write not supported." );
        // static_assert( !can_only_set_or_clear( io::mutability ), "field: write not supported for single bits." );

        if constexpr( is_single_bit() )
        {
            if ( value ) set();
            else         clear();
        }
        else if constexpr( is_all_bits() )
        {
            io::write( address, value );
        }
        else
        {
            io::write( address, rngmask<T>(hi,lo), shl(value, lo) );
        }
    }

    // Deferred write a field:

    static auto write_lazy( value_type value )
    {
        return write_proxy< T, io, address, rngmask<T>(hi,lo), 0 >{ shl(value, lo) };
    }

    // Set a single bit:

    static void set()
    {
        // static_assert( is_single_bit(), "field: set() only supported for a single bit.");

        io::set( address, hi );
    }

    // Clear single bit by writing 0 or 1, according to io-policy:

    static void clear()
    {
        // static_assert( is_single_bit(), "field: clear() only supported for a single bit.");

        io::clear( address, hi );
    }

    // Toggle single bit:

    static void toggle()
    {
        // static_assert( is_single_bit(), "field: toggle() only supported for a single bit.");

        io::toggle( address, hi );
    }

    // Trigger a single bit:

    static void trigger()
    {
        // static_assert( is_single_bit(), "field: trigger() only supported for a single bit.");

        io::trigger( address, hi );
    }

private:
    static constexpr auto is_single_bit()
    {
        return hi == lo;
    }

    static constexpr auto is_all_bits()
    {
        return hi - lo + 1 == bits();
    }

    static constexpr uint8_t bits()
    {
        return ::mcu::bits<T>();
    }
};

// A bit field in an 8-bit register:

template< template<typename> typename io, address_t address, index_t hi, index_t lo = hi >
using bitfield8_t = bitfield_t< uint8_t, io, address, hi, lo >;

// A bit field in an 16-bit register:

template< template<typename> typename io, address_t address, index_t hi, index_t lo = hi >
using bitfield16_t = bitfield_t< uint16_t, io, address, hi, lo >;

// A 8-bit register type:

template< template<typename> typename io, address_t address >
using register8_t = bitfield_t< uint8_t, io, address, 7, 0>;

// A 16-bit register type:

template< template<typename> typename io, address_t address >
using register16_t = bitfield_t< uint16_t, io, address, 15, 0>;

// A register type of type to be defined:

template< typename data_t, template<typename> typename io, address_t address >
using register_t = bitfield_t< data_t, io, address, bits<data_t>() - 1, 0>;


// scoped interrupt guard;
// simplification: interrupts are assumed to be on

struct scoped_interrupt_guard
{
     ~scoped_interrupt_guard() { sei(); }
     scoped_interrupt_guard()  { cli(); }
};

// read multiple-byte values atomic by interrupts off

template< typename T >
T atomic( T const & value )
{
    if constexpr( sizeof(T) > 1 ) { scoped_interrupt_guard _; return value; }
    else                          {                           return value; }
}

template< typename T, typename U >
void atomic( T & dest, U value )
{
    if constexpr( sizeof(T) > 1 ) { scoped_interrupt_guard _; dest = value; }
    else                          {                           dest = value; }
}

} // namespace mcu

#endif // MCU_REGISTER_HPP_INCLUDED
