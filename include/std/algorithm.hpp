// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef STD_ALGORITHM_INCLUDED
#define STD_ALGORITHM_INCLUDED

namespace std {

template< class InputIt, class OutputIt >
constexpr OutputIt copy( InputIt first, InputIt last, OutputIt d_first )
{
    while ( first != last )
    {
        *d_first++ = *first++;
    }
    return d_first;
}

} // namespace std

#endif // STD_ALGORITHM_INCLUDED
