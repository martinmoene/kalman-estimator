
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This code is based on cpp11-range by Konrad Rudolph,
// https://github.com/klmr/cpp11-range

#ifndef CORE_SHIMS_HPP_INCLUDED
#define CORE_SHIMS_HPP_INCLUDED

namespace core {

// cast: accept any type U convertible to T

template< typename T >
struct cast
{
    T value;

    template< typename U >
    cast( U value_)
    : value( static_cast<T>( value_) )
    {}

    operator T() const
    {
        return value;
    }
};

template< typename T >
inline auto as( cast<T> value )
{
    return value;
}

} // namespace core

#endif // CORE_SHIMS_HPP_INCLUDED
