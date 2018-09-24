// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef STD_TUPLE_INCLUDED
#define STD_TUPLE_INCLUDED

#if !( defined( __AVR ) && __AVR )

#include <tuple>

namespace std20 {

using std::tuple;
using std::ignore;

} // namespace std20

#else // __AVR

namespace std20 {

// (std::) ignore for structured bindings:

struct ignore
{
    template< typename T >
    ignore & operator=(T const &) { return *this; }
};

} // namespace std20

#endif // STD_TUPLE_INCLUDED
