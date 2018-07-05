//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef NUM_FIXED_POINT_IO_HPP_INCLUDED
#define NUM_FIXED_POINT_IO_HPP_INCLUDED

#include "fixed-point.hpp"
#include <iostream>

namespace num {

template< typename T, int I, int F >
std::ostream & operator<<( std::ostream & os, fixed_point<T,I,F> const & x )
{
    return os << x.template as<double>();
}

} // namespace num

#endif // NUM_FIXED_POINT_IO_HPP_INCLUDED
