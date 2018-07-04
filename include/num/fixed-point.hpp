// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef NUM_FIXED_POINT_HPP_INCLUDED
#define NUM_FIXED_POINT_HPP_INCLUDED

#define fixed_point_MAJOR  0
#define fixed_point_MINOR  0
#define fixed_point_PATCH  0
#define fixed_point_VERSION \
    fixed_point_STRINGIFY(fixed_point_MAJOR) "." \
    fixed_point_STRINGIFY(fixed_point_MINOR) "." \
    fixed_point_STRINGIFY(fixed_point_PATCH)

#define fixed_point_STRINGIFY(  x )  fixed_point_STRINGIFY_( x )
#define fixed_point_STRINGIFY_( x )  #x

#if defined( __AVR ) && __AVR
# include "std/cstdint.hpp"
# include "std/limits.hpp"
# include "std/type_traits.hpp"
# include "std/utility.hpp"
#else
# include <cstdint>
# include <limits>
# include <type_traits>
# include <utility>
#endif

#define num_require(expr) \
    typename = std::enable_if_t<expr>

namespace num {

namespace detail {

// Promote type for multiplication and division:

template< typename T > struct promote;

template<> struct promote<std::int8_t  > { using type = std::int16_t; };
template<> struct promote<std::int16_t > { using type = std::int32_t; };
template<> struct promote<std::int32_t > { using type = std::int64_t; };

template< typename T >
using promote_t = typename promote<T>::type;

// Compile-time power-of-2:

constexpr int power2( int n )
{
    int result = 1;
    while( n-- > 0 )
        result *= 2;
    return result;
}

} // namespace detail

// fixed_point:

template
<
    typename T      // integral implementation type
    , int I         // number of bits for integral part
    , int F = std::numeric_limits<T>::digits - I
                    // number of bits for fractional part
    , num_require( std::is_signed_v<T> && std::is_integral_v<T> )
                    // require signed integral implementation type
>
class fixed_point
{
public:
    // Types:

    using rep = T;

    constexpr static struct construct_t{} construct{};

    // Constants:

    constexpr static int digits = std::numeric_limits<T>::digits;
    constexpr static int integer_digits = I;
    constexpr static int fractional_digits = F;

    // Construction:

#if defined( __AVR ) && __AVR
    constexpr fixed_point() : storage() {}
#else
    constexpr fixed_point() = default;
#endif
    constexpr fixed_point( fixed_point && ) = default;
    constexpr fixed_point( fixed_point const & ) = default;
    constexpr fixed_point & operator=( fixed_point && ) = default;
    constexpr fixed_point & operator=( fixed_point const & ) = default;

    constexpr fixed_point( construct_t, T value )
    : storage( value )
    {}

    template< typename U >
    constexpr fixed_point( U value )
    : storage( static_cast<T>( detail::power2(F) * value ) )
    {}

    constexpr fixed_point( float value )
    : storage( static_cast<T>( detail::power2(F) * value + (value >= 0.0f ? 0.5f : -0.5f) ) )
    {}

    constexpr fixed_point( double value )
    : storage( static_cast<T>( detail::power2(F) * value + (value >= 0.0 ? 0.5 : -0.5) ) )
    {}

    // Observers:

    constexpr auto min() const
    {
        return fixed_point( construct, std::numeric_limits<T>::min() );
    }

    constexpr auto max() const
    {
        return fixed_point( construct, std::numeric_limits<T>::max() );
    }

    constexpr auto underlying_value() const
    {
        return storage;
    }

    template< typename U >
    constexpr auto as() const
    {
        return static_cast<U>( underlying_value() ) / detail::power2(F);
    }

    // Arithmetic:

    constexpr auto operator+() const { return *this; }
    constexpr auto operator-() const { return fixed_point( construct, -storage ); }

    constexpr auto operator+=( fixed_point rhs )
    {
        storage += rhs.storage; return *this;
    }

    constexpr auto operator-=( fixed_point rhs )
    {
        storage -= rhs.storage; return *this;
    }

    constexpr auto operator*=( fixed_point rhs )
    {
        storage = static_cast<T>( (static_cast<detail::promote_t<T>>(storage) * rhs.storage) >> F );
        return *this;
    }

    constexpr auto operator/=( fixed_point rhs )
    {
        storage = static_cast<T>( (static_cast<detail::promote_t<T>>(storage) << F ) / rhs.storage );
        return *this;
    }

    friend constexpr auto operator+( fixed_point a, fixed_point b )
    {
        return a += b;
    }

    friend constexpr auto operator-( fixed_point a, fixed_point b )
    {
        return a -= b;
    }

    friend constexpr auto operator*( fixed_point a, fixed_point b )
    {
        return a *= b;
    }

    friend constexpr auto operator/( fixed_point a, fixed_point b )
    {
        return a /= b;
    }

    // Comparison:

    friend constexpr auto operator==( fixed_point a, fixed_point b )
    {
        return a.storage == b.storage;
    }

    friend constexpr auto operator!=( fixed_point a, fixed_point b )
    {
        return !( a == b );
    }

    friend constexpr auto operator<( fixed_point a, fixed_point b )
    {
        return a.storage < b.storage;
    }

    friend constexpr auto operator<=( fixed_point a, fixed_point b )
    {
        return !(b < a);
    }

    friend constexpr auto operator>( fixed_point a, fixed_point b )
    {
        return b < a;
    }

    friend constexpr auto operator>=( fixed_point a, fixed_point b )
    {
        return !( a < b );
    }

private:
    rep storage;
};

// to_rep - Extract Underlying Value:

template< typename T >
struct to_rep;

template< typename T, int I, int F >
struct to_rep< fixed_point<T,I,F> >
{
    constexpr auto operator()( fixed_point<T,I,F> const & x ) const
    {
        return x.underlying_value();
    }
};

// from_rep - Create from Underlying Value:

template< typename T >
struct from_rep;

template< typename T, int I, int F >
struct from_rep< fixed_point<T,I,F> >
{
    constexpr auto operator()( T value ) const
    {
        return fixed_point<T,I,F>::fixed_point( fixed_point<T,I,F>::construct, value );
    }
};

// from_value - Determine Type from Initial Value

template< typename T, typename Value >
struct from_value;

template< typename T, int I, int F, typename Value >
struct from_value< fixed_point<T,I,F>, Value >
{
    constexpr auto operator()( Value value ) const
    {
        return fixed_point<Value,I,F>::fixed_point( fixed_point<Value,I,F>::construct, value );
    }
};

} // namespace num

#endif // NUM_FIXED_POINT_HPP_INCLUDED
