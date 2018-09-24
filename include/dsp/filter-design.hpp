// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef DSP_FILTER_DESIGN_HPP_INCLUDED
#define DSP_FILTER_DESIGN_HPP_INCLUDED

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

} // namespace dsp

#endif // DSP_FILTER_DESIGN_HPP_INCLUDED
