//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "core/range.hpp"
#include "lest.hpp"
#include <vector>

// Configuration:

// none

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

using namespace core;

CASE( "core: range(0)" )
{
    std::vector<int> vec;

    for ( auto i : range( 0 ) )
    {
        vec.push_back( i );

        if ( i == 42 )
            break;
    }

    EXPECT( vec.size() ==  43 ); // [0..42]
    EXPECT( vec[ 0] ==  0 );
    EXPECT( vec[42] == 42 );
}

CASE( "core: range(0, 4)" )
{
    auto it = range(0, 4);

    std::vector<int> vec( it.begin(), it.end() );

    EXPECT( vec == std::vector<int>({0,1,2,3}) );
}

CASE( "core: range(0, 10, 3)" )
{
    auto it = range(0, 10, 3);

    std::vector<int> vec( it.begin(), it.end() );

    EXPECT( vec == std::vector<int>({0,3,6,9}) );
}

CASE( "core: range(-2, 2.5, 0.5)" )
{
    auto it = range(-2, 2.5, 0.5);

    std::vector<double> vec( it.begin(), it.end() );

    EXPECT( vec == std::vector<double>({-2,-1.5,-1.0,-0.5, 0, 0.5, 1.0, 1.5, 2.0}) );
}

CASE( "core: indices(c_arr)" )
{
    int c_arr[] = { 6,7,8, };

    auto it = indices( c_arr );

    std::vector<double> vec( it.begin(), it.end() );

    EXPECT( vec == std::vector<double>({0, 1, 2}) );
}

CASE( "core: indices(std_vector)" )
{
    std::vector<int> std_vec = { 6,7,8,9 };

    auto it = indices( std_vec );

    std::vector<double> vec( it.begin(), it.end() );

    EXPECT( vec == std::vector<double>({0, 1, 2, 3}) );
}

CASE( "core: indices({1,2,4,6,8})" )
{
    auto it = indices({ 1, 2, 4, 6, 8 });

    std::vector<double> vec( it.begin(), it.end() );

    EXPECT( vec == std::vector<double>({0, 1, 2, 3, 4}) );
}


