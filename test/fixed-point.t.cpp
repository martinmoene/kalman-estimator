// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "num/fixed-point.hpp"
#include "num/fixed-point-io.hpp"
#include "lest.hpp"
#include <algorithm>

// Configuration:

#ifndef  KE_USE_STATIC_EXPECT
# define KE_USE_STATIC_EXPECT  0
#endif

// Suppress:
// - unused parameter, for cases without assertions such as [.std...]
#if defined __clang__
# pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined __GNUC__
# pragma GCC   diagnostic ignored "-Wunused-parameter"
#endif

#if defined( KE_USE_STATIC_EXPECT ) && KE_USE_STATIC_EXPECT
# define STATIC_EXPECT(     expr )  static_assert(   expr  )
# define STATIC_EXPECT_NOT( expr )  static_assert( !(expr) )
#else
# define STATIC_EXPECT(     expr )  EXPECT(     expr )
# define STATIC_EXPECT_NOT( expr )  EXPECT_NOT( expr )
#endif

#define CASE( name ) lest_CASE( specification(), name )

extern lest::tests & specification();

struct approx
{
    bool operator()( double a, double b ) const
    {
        return a == lest::approx(b);
    }
};

using namespace num;

using fp32_t = fixed_point<std::int32_t, 15>;

CASE( "fixed_point: Allows to default construct uninitialized fixed_point value" )
{
    fixed_point<int, 15> a;

    // no expectations
    EXPECT( &a != nullptr );
}

CASE( "fixed_point: Allows to construct from an integral value" )
{
    constexpr fixed_point<int, 15> a( 42 );

    STATIC_EXPECT( a.as<int>() == 42 );
}

CASE( "fixed_point: Allows to construct from a float" )
{
    constexpr fixed_point<int, 15> a( 1.23f );

    EXPECT( a.as<float>() == lest::approx( 1.23f ) );
}

CASE( "fixed_point: Allows to construct from a double" )
{
    constexpr fixed_point<int, 15> a( 3.14 );

    /*STATIC_*/EXPECT( a.as<double>() == lest::approx( 3.14 ) );
}

CASE( "fixed_point: Allows to copy-construct from another fixed_point of the same type" )
{
    constexpr fixed_point<int, 15> a( 42 );
    constexpr fixed_point<int, 15> b( a  );

    STATIC_EXPECT( b.as<int>() == 42 );
}

CASE( "fixed_point: Allows to move-construct from another fixed_point of the same type" )
{
    constexpr fixed_point<int, 15> a( fixed_point<int, 15>( 42 ) );

    STATIC_EXPECT( a.as<int>() == 42 );
}

CASE( "fixed_point: Allows to copy-assign from another fixed_point of the same type" )
{
    fixed_point<int, 15> a( 42 );
    fixed_point<int, 15> b;

    b = a;

    EXPECT( b.as<int>() == 42 );
}

CASE( "fixed_point: Allows to move-assign from another fixed_point of the same type" )
{
    fixed_point<int, 15> a;

    a = fixed_point<int, 15>( 42 );

    EXPECT( a.as<int>() == 42 );
}

CASE( "fixed_point: Allows to observe the minimum value" )
{
    constexpr fixed_point<char, 3> a( 0 );

    STATIC_EXPECT( a.min() == -8 );
}

CASE( "fixed_point: Allows to observe the maximum value" )
{
    constexpr fixed_point<char, 3> a( 0 );

           EXPECT( a.max().as<double>() == lest::approx( 7.9375 ) );
    STATIC_EXPECT( a.max()    == (fixed_point<char, 3>)( 7.9375 ) );
}

CASE( "fixed_point: Allows to observe the underlying value" )
{
    constexpr fixed_point<std::int16_t, 15> a( 42 );
    constexpr fixed_point<std::int16_t, 14> b( 42 );
    constexpr fixed_point<std::int16_t,  3> c(  4 );

    STATIC_EXPECT( a.underlying_value() == (42 << ((16-1)-15)) );
    STATIC_EXPECT( b.underlying_value() == (42 << ((16-1)-14)) );
    STATIC_EXPECT( c.underlying_value() == ( 4 << ((16-1)- 3)) );
}

CASE( "fixed_point: Allows equality comparison" )
{
    constexpr fp32_t a(4);
    constexpr fp32_t b(2);

    STATIC_EXPECT( a == a );
    STATIC_EXPECT( a == 4 );

    STATIC_EXPECT( a != b );
    STATIC_EXPECT( a != 2 );
}

CASE( "fixed_point: Allows less-than comparison" )
{
    constexpr fp32_t a(2);
    constexpr fp32_t b(4);

    STATIC_EXPECT( a <  b );
    STATIC_EXPECT( a <= b );
    STATIC_EXPECT( a <= a );

    STATIC_EXPECT( b >  a );
    STATIC_EXPECT( b >= a );
    STATIC_EXPECT( b >= b );

    STATIC_EXPECT_NOT( b <  a );
    STATIC_EXPECT_NOT( b <= a );

    STATIC_EXPECT_NOT( a >  b );
    STATIC_EXPECT_NOT( a >= b );
}

CASE( "fixed_point: Allows unary plus" )
{
    constexpr fixed_point<int, 15> a( 42 );

    STATIC_EXPECT( +a == +42 );
}

CASE( "fixed_point: Allows unary minus" )
{
    constexpr fixed_point<int, 15> a( 42 );

    STATIC_EXPECT( -a == -42 );
}

CASE( "fixed_point: Allows 'a op= b' arithmetic" )
{
    SETUP("")
    {
        fp32_t a(4);
        fp32_t b(2);

        SECTION("") { EXPECT( (a+=b) == 4+2 ); }
        SECTION("") { EXPECT( (a-=b) == 4-2 ); }
        SECTION("") { EXPECT( (a*=b) == 4*2 ); }
        SECTION("") { EXPECT( (a/=b) == 4/2 ); }
    }
}

CASE( "fixed_point: Allows 'a op b' arithmetic" )
{
    constexpr fp32_t a(4);
    constexpr fp32_t b(2);

    STATIC_EXPECT( (a+b) == 4+2 );
    STATIC_EXPECT( (a-b) == 4-2 );
    STATIC_EXPECT( (a*b) == 4*2 );
    STATIC_EXPECT( (a/b) == 4/2 );
}

CASE( "fixed_point: Allows to extract the fixed_point underlying value via to_rep" )
{
    constexpr fixed_point<int, 15> a( 123 ) ;
    constexpr auto v = to_rep< fixed_point<int, 15> >()( a );

    STATIC_EXPECT( v == a.underlying_value() );
}

CASE( "fixed_point: Allows to create fixed_point from underlying value via from_rep" )
{
    constexpr auto a = from_rep< fixed_point<int, 15> >()( 123 );

    STATIC_EXPECT( a.underlying_value() == 123 );
}

CASE( "fixed_point: Allows to determine fixed_point numeric type from another type via from_value" )
{
    constexpr auto a = from_value< fixed_point<std::int16_t, 8>, std::int32_t>()( 123 );

    STATIC_EXPECT( (std::is_same_v< decltype(a.underlying_value()), std::int32_t >) );
    STATIC_EXPECT( a.underlying_value() == 123 );
}

CASE( "fixed_point: detail::power2() returns power of 2 at compile-time" )
{
    STATIC_EXPECT( detail::power2(0) == 1 );
    STATIC_EXPECT( detail::power2(1) == 2 );
    STATIC_EXPECT( detail::power2(2) == 4 );
    STATIC_EXPECT( detail::power2(3) == 8 );
    STATIC_EXPECT( detail::power2(4) == 16);
}

// -----------------------------------------------------------------------
// Applets

CASE( "fixed_point: printing" " [.app][print]" )
{
    std::cout
        << "\nfp32_t{1.23}: " << fp32_t{1.23}
        ;
}

CASE( "detail::power2: returns power of 2 at compile-time" "[.app][power]" )
{
    std::cout
        << "\n"
        << "\npower2<0>:" << detail::power2(0)
        << "\npower2<1>:" << detail::power2(1)
        << "\npower2<2>:" << detail::power2(2)
        << "\npower2<3>:" << detail::power2(3)
        << "\npower2<4>:" << detail::power2(4)
        ;

}

CASE( "fixed_point: xxxxx" "[.app][constants]" )
{
    std::cout
        << "\n"
        << "\nfp32_t::digits: "            << fp32_t::digits
        << "\nfp32_t::integer_digits: "    << fp32_t::integer_digits
        << "\nfp32_t::fractional_digits: " << fp32_t::fractional_digits
        ;
}

CASE( "fixed_point: xxxxx" "[.app][observers]" )
{
    fp32_t x{ 123 };
    fp32_t y{ 0.123 };

    std::cout
        << "\n"
        << "\nfp32_t x{123}: "        << x
        << "\nx.min(): "              << x.min().as<double>()
        << "\nx.max(): "              << x.max().as<double>()
        << "\nx.underlying_value(): " << x.underlying_value()
        << "\n"
        << "\nfp32_t y{0.123}: "      << y
        << "\ny.min(): "              << y.min().as<double>()
        << "\ny.max(): "              << y.max().as<double>()
        << "\ny.underlying_value(): " << y.underlying_value()
        ;
}

CASE( "fixed_point: ==, !=" "[.app][comparison][equality]" )
{
    fp32_t x{ 123 };
    fp32_t y{ 0.123 };

    std::cout
        << "\n" << std::boolalpha
        << "\nfp32_t x{123}: "   << x
        << "\nfp32_t y{0.123}: " << y
        << "\nx == x: " << (x == x)
        << "\nx != x: " << (x != x)
        << "\nx == y: " << (x == y)
        << "\nx != y: " << (x != y)
        ;
}

CASE( "fixed_point: <, <= etc." "[.app][comparison][less-than]" )
{
    fp32_t x{ 123 };
    fp32_t y{ 0.123 };

    std::cout
        << "\n" << std::boolalpha
        << "\nfp32_t x{123}: "   << x
        << "\nfp32_t y{0.123}: " << y
        << "\nx <  x: " << (x <  x)
        << "\nx <= x: " << (x <= x)
        << "\nx <  y: " << (x <  y)
        << "\nx <= y: " << (x <= y)
        << "\ny <  x: " << (y <  x)
        << "\ny <= x: " << (y <= x)
        ;
}

CASE( "fixed_point: unary + and -" "[.app][unary]" )
{
    fp32_t x{ 123.123 };

    std::cout
        << "\n"
        << "\nfp32_t x{123.123}: " << x
        << "\n+x    : " << +x
        << "\n-x    : " << -x
        ;
}

CASE( "fixed_point: binary +, - etc." "[.app][arithmetic]" )
{
    fp32_t x{ 123 };
    fp32_t y{ 0.123 };
    auto z = x + y;

    std::cout
        << "\n"
        << "\nfp32_t x{123}: "   << x
        << "\nfp32_t y{0.123}: " << y
        << "\nfp32_t z{a + b}: " << z
        << "\nx    : " << ( x   )
        << "\ny    : " << ( y   )
        << "\n+z   : " << (+z   )
        << "\n-z   : " << (-z   )
        << "\nx + y: " << (x + y)
        << "\ny + x: " << (y + x)
        << "\n1 * x: " << (1 * x)
        << "\n3 * x: " << (3 * x)
        << "\nx * 3: " << (x * 3)
        << "\n3 * y: " << (3 * y)
        << "\nx * y: " << (x * y)
        << "\ny * x: " << (y * x)
        << "\nx / y: " << (x / y)
        << "\ny / x: " << (y / x)
        << "\ny / 2: " << (y / 2)
        ;
}

CASE( "fixed_point: conversion via to_rep, from_rep, from_value" "[.app][conversion]" )
{
    fp32_t x{ 123 };
    fp32_t y{ 0.123 };
    auto z = x + y;

    std::cout
        << "\n"
        << "\nfp32_t x{123}: "        << x
        << "\nfp32_t y{0.123}: "      << y
        << "\nfp32_t z{a + b}: "      << z
        << "\nto_rep<fp32_t>()(z): "  << to_rep<fp32_t>()( z)
        << "\nto_rep<fp32_t>()(+z): " << to_rep<fp32_t>()(+z)
        << "\nto_rep<fp32_t>()(-z): " << to_rep<fp32_t>()(-z)
        ;
}
