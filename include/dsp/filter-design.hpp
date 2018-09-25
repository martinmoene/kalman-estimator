// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef DSP_FILTER_DESIGN_HPP_INCLUDED
#define DSP_FILTER_DESIGN_HPP_INCLUDED

#include "core/shims.hpp"
#include "core/traits.hpp"
#include "dsp/filter.hpp"
#include "num/constants.hpp"

namespace dsp {

// algorithm: pre-warp frequency:

template< typename P, typename Q >
inline auto prewarp( P f, Q fs )
{
    using T = std::common_type_t<float, P, Q>;

    return std::tan( num::pi<T> * f / fs );
}

// factor as dbP

template< typename T >
auto dbP( T vout, core::identity_t<T> vin )
{
    using U = std::common_type_t<float, T>;

    return U{10} * std::log10( core::as<U>(vout) / vin );
}

// dbP as factor

template< typename T >
auto dbP_factor( T dbv )
{
    return std::pow( T{10}, std::abs( dbv ) / T{10} );
}

// factor as dbV

template< typename T, typename U >
auto dbV( T vout, U vin )
{
    return T{2} * dbP( vout, vin );
}

// dbV as factor

template< typename T >
auto dbV_factor( T dbv )
{
    return dbP_factor( dbv ) / T{2};
}

} // namespace dsp

#endif // DSP_FILTER_DESIGN_HPP_INCLUDED
