//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef DSP_BIQUAD_CASCADE_IO_HPP_INCLUDED
#define DSP_BIQUAD_CASCADE_IO_HPP_INCLUDED

#include "biquad-cascade.hpp"
#include "biquad-io.hpp"

namespace dsp { namespace biquad_cascade {

template< typename T, int N >
std::ostream & operator<<( std::ostream & os, BiQuadCascadeT<T,N> const & bqc )

{
    os <<
        "\nFilter cascade occupies " << sizeof(BiQuadCascadeT<T,N>) << " bytes in memory." <<
        "\nfilters: " << bqc.size() << " entries out of " << bqc.reserved() << ", " <<
        ( is_stable( bqc ) ? "stable:":"NOT stable:" ) <<
        "\n";

    for ( auto const & bq : bqc )
    {
        os << bq;
    }
    return os;
}

} // namespace biquad_cascade
} // namespace dsp

#endif // DSP_BIQUAD_CASCADE_IO_HPP_INCLUDED
