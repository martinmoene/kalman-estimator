// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Contains C++ implementations of code from the book:
// Sophocles J. Orfanidis. Introduction to Signal Processing,
// http://www.ece.rutgers.edu/~orfanidi/intro2sp/

#ifndef DSP_CHEBYSHEV_DESIGN_IO_HPP_INCLUDED
#define DSP_CHEBYSHEV_DESIGN_IO_HPP_INCLUDED

#include "dsp/chebyshev-design.hpp"
#include "core/vector-io.hpp"

namespace dsp { namespace chebyshev_design {

// output: stream filter design algorithm internal information:

template< typename T >
inline std::ostream & operator<<( std::ostream & os, AlgoInfoT<T> ai )
{
    return os <<
        "[I Wpass:" << ai.Wpass <<
        " Wstop:"   << ai.Wstop <<
        " epass:"   << ai.epass <<
        " estop:"   << ai.estop <<
        " Nex:"     << ai.Nex   <<
        " N:"       << ai.N     <<
        " f3:"      << ai.f3    <<
        " a:"       << ai.a     <<
        " ]";
}

} // namespace chebyshev_design
} // namespace dsp

#endif // DSP_CHEBYSHEV_DESIGN_IO_HPP_INCLUDED
