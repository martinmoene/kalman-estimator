//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef DSP_ONEPOLE_IO_HPP_INCLUDED
#define DSP_ONEPOLE_IO_HPP_INCLUDED

#include "onepole.hpp"
#include <iostream>

namespace dsp { namespace onepole {

template< typename T >
std::ostream & operator<<( std::ostream & os, AT<T> a )
{
    return os << "[A " << a.a1 << " ]";
}

template< typename T >
std::ostream & operator<<( std::ostream & os, BT<T> b )
{
    return os << "[B " << b.b0 << " ]";
}

template< typename T >
std::ostream & operator<<( std::ostream & os, BAT<T> ba )
{
    return os << static_cast<BT<T>>( ba ) << static_cast<AT<T>>( ba );
}

template< typename T >
std::ostream & operator<<( std::ostream & os, WT<T> w )
{
    return os << "[W " << w.v1 << " ]";
}

template< typename T >
std::ostream & operator<<( std::ostream & os, SolutionT<T> const & s )
{
    return os << "[S " << s.s0 << " ]";
}

template< typename T, typename S >
std::ostream & operator<<( std::ostream & os, OnePoleT<T,S> const & bq )
{
    return os <<
        "\nOnePole is always stable" <<
        "\na: " << bq.coeff_a() <<
        "\nb: " << bq.coeff_b() <<
        "\nw: " << bq.state() <<
        "\np: " << bq.poles() <<
        "\nz: " << bq.zeros() <<
        "\n";
}

} // namespace onepole
} // namespace dsp


#endif // DSP_ONEPOLE_IO_HPP_INCLUDED
