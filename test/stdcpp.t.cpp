// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef __INT8_TYPE__
#define __CHAR_BIT__    8
#define __INT8_TYPE__   signed char
#define __INT16_TYPE__  short
#define __INT32_TYPE__  int
#define __INT64_TYPE__  long long
#define __INT8_MAX__    0x7f
#define __INT16_MAX__   0x7fff
#define __INT32_MAX__   0x7fffffffL
#define __INT64_MAX__   0x7fffffffffffffffLL
#endif // __INT8_TYPE__

#include "std/algorithm.hpp"
#include "std/cstdint.hpp"
#include "std/limits.hpp"
#include "std/type_traits.hpp"
#include "std/utility.hpp"

#include <cassert>

// Note cannot use lest as above headers then clash with the ones of the compiler.

struct caller
{
    template<typename T>
    caller( T fun ) { fun(); }
};

#define CAT2(X, Y)  X ## Y
#define CAT(X, Y)   CAT2(X, Y)

//#define TEST(NAME) caller CAT(NAME, __LINE__) = []
#define TEST(NAME) caller CAT(__VAR, __LINE__) = []

TEST( algorithm )
{
    int a[] = { 1, 2, 3, };
    int b[] = { 0, 0, 0, };

    std20::copy( &a[0], &a[3], &b[0] );

    assert( b[0] == a[0] );
    assert( b[1] == a[1] );
    assert( b[2] == a[2] );
};

TEST( cstdint )
{
    using i8  = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;

    assert( sizeof( i8  ) == 1 );
    assert( sizeof( i16 ) == 2 );
    assert( sizeof( i32 ) == 4 );
    assert( sizeof( i64 ) == 8 );

};

TEST( limits )
{
    assert( std::numeric_limits<float  >::is_signed );
    assert( std::numeric_limits<double >::is_signed );
//  assert( std::numeric_limits<int    >::is_signed );
    assert( std::numeric_limits<std::int8_t  >::is_signed );
    assert( std::numeric_limits<std::int16_t >::is_signed );
    assert( std::numeric_limits<std::int32_t >::is_signed );
    assert( std::numeric_limits<std::int64_t >::is_signed );

    assert(!std::numeric_limits<float  >::is_integer );
    assert(!std::numeric_limits<double >::is_integer );
//  assert( std::numeric_limits<int    >::is_integer );
    assert( std::numeric_limits<std::int8_t  >::is_integer );
    assert( std::numeric_limits<std::int16_t >::is_integer );
    assert( std::numeric_limits<std::int32_t >::is_integer );
    assert( std::numeric_limits<std::int64_t >::is_integer );

//  assert(  x == std::numeric_limits<float  >::digits );
//  assert(  x == std::numeric_limits<double >::digits );
//  assert(  x == std::numeric_limits<int    >::digits );
    assert(  7 == std::numeric_limits<std::int8_t  >::digits );
    assert( 15 == std::numeric_limits<std::int16_t >::digits );
    assert( 31 == std::numeric_limits<std::int32_t >::digits );
    assert( 63 == std::numeric_limits<std::int64_t >::digits );

//  assert(  x == std::numeric_limits<float  >::min() );
//  assert(  x == std::numeric_limits<double >::min() );
//  assert(  x == std::numeric_limits<int    >::min() );
    assert(              -128   == std::numeric_limits<std::int8_t  >::min() );
    assert(            -32768   == std::numeric_limits<std::int16_t >::min() );
    assert(       -2147483648LL == std::numeric_limits<std::int32_t >::min() );
    assert(0x8000000000000000LL == std::numeric_limits<std::int64_t >::min() );

//  assert(  x == std::numeric_limits<float  >::max() );
//  assert(  x == std::numeric_limits<double >::max() );
//  assert(  x == std::numeric_limits<int    >::max() );
    assert(              +127   == std::numeric_limits<std::int8_t  >::max() );
    assert(            +32767   == std::numeric_limits<std::int16_t >::max() );
    assert(       +2147483647L  == std::numeric_limits<std::int32_t >::max() );
    assert(0x7fffffffffffffffLL == std::numeric_limits<std::int64_t >::max() );
};

TEST( type_traits )
{
//    assert( 0 );
};

TEST( utility )
{
    int a = 7;
    int b = 42;

    std::swap( a, b );

    assert( a == 42 );
    assert( b ==  7 );

    std::initializer_list<int> il; // = { 1, 2, 3, };

    assert( il.size()  == 0 );
    assert( il.begin() == nullptr );
};
