// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef DSP_FILTER_HPP_INCLUDED
#define DSP_FILTER_HPP_INCLUDED

#include "dsp/biquad-design.hpp"

namespace dsp {

/// Low-pass filter

template< typename T >
class Lpf
{
public:
    using value_type = T;
    using BiQuad = dsp::BiQuadT<T>;

    // dbGain not available for lpf, hpf:
    const value_type dbGain = 0;

    Lpf( value_type f3db, value_type fs, value_type Q )
    : bq( make_biquad( dsp::biquad_design< dsp::FilterResponse::low_pass >( dbGain, f3db, fs, Q ) ) )
    {}

    auto operator()( value_type sample )
    {
        return bq.step( sample );
    }

private:
    BiQuad bq;
};

/// High-pass filter

template< typename T >
class Hpf
{
public:
    using value_type = T;
    using BiQuad = dsp::BiQuadT<T>;

    // dbGain not available for lpf, hpf:
    const value_type dbGain = 0;

    Hpf( value_type f3db, value_type fs, value_type Q )
    : bq( make_biquad( dsp::biquad_design< dsp::FilterResponse::high_pass >( dbGain, f3db, fs, Q ) ) )
    {}

    auto operator()( value_type sample )
    {
        return bq.step( sample );
    }

private:
    BiQuad bq;
};

} // namespace dsp

#endif // DSP_FILTER_HPP_INCLUDED
