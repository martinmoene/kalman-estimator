//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef NUM_MATRIX_IO_HPP_INCLUDED
#define NUM_MATRIX_IO_HPP_INCLUDED

#include "matrix.hpp"
#include <iostream>

namespace num {

template< typename T, int N, int M >
std::ostream & operator<<( std::ostream & os, matrix<T,N,M> const & m )
{
    for ( int row = 0; row < m.rows(); ++row )
    {
        for ( int col = 0; col < m.columns(); ++col )
        {
            os << " " << m(row, col);
        }
        os << "\n";
    }
    return os;
}

} // namespace num

#endif // NUM_MATRIX_IO_HPP_INCLUDED
