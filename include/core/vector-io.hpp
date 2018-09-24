//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef NUM_VECTOR_IO_HPP_INCLUDED
#define NUM_VECTOR_IO_HPP_INCLUDED

#include <vector>
#include <iostream>

// Print a vector:
template< typename T >
std::ostream & operator<<( std::ostream & os, std::vector<T> const & x )
{
    os << "[";
    for ( auto v : x )
    {
        os << " " << v;
    }
    return os << "]";
}

#endif // NUM_VECTOR_IO_HPP_INCLUDED
