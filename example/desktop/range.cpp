//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "core/range.hpp"
#include <initializer_list>
#include <iostream>
#include <vector>

int main()
{
    using namespace core;

    std::cout << "\nrange( 20 ), infinite, break at 42: ";

    for ( auto i : range( 20 ) )
    {
        std::cout << i << " ";

        if ( i == 42 )
            break;
    }

    std::cout << "\nrange( 0 ).step( 2 ), infinite, break at 7: ";

    for ( auto i : range( 0 ).step( 2 ) )
    {
        std::cout << i << " ";

        if ( i >= 7 )
            break;
    }

    std::cout << "\nrange( 0, 10 ): ";

    for ( auto i : range( 0, 10 ) )
    {
        std::cout << i << " ";
    }

    std::cout << "\nrange( 0, 10, 3 ): ";

    for ( auto i : range( 0, 10, 3 ) )
    {
        std::cout << i << " ";
    }

    std::cout << "\nrange( -5, 5.5, 0.5 ): ";

    for ( auto i : range( -5, 5.5, 0.5 ) )
    {
        std::cout << i << " ";
    }

    // not supported:
//    std::cout << "\n";
//
//    for ( auto i : range( 0, -10, -0.5 ) )
//    {
//        std::cout << i << " ";
//    }

    std::cout << "\nindices( c_arr ): ";

    int arr[] = { 6,7,8, };

    for ( auto i : indices( arr ) )
    {
        std::cout << i << " ";
    }

    std::cout << "\nindices( std_vec ): ";

    std::vector<int> vec = { 6,7,8,9 };

    for ( auto i : indices( vec ) )
    {
        std::cout << i << " ";
    }

    std::cout << "\nindices({ 1,2,4,6,8 }): ";

    for ( auto i : indices({ 1,2,4,6,8 }) )
    {
        std::cout << i << " ";
    }

    std::cout << "\n";
}

// cl -EHsc -std:c++17 -O2 -I../../include range.cpp&& range.exe
// g++ -Wall -std=c++17 -O2 -I../../include -o range.exe range.cpp&& range.exe
