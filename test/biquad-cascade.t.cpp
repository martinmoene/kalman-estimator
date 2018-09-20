//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "num/biquad-cascade.hpp"
#include "num/biquad-cascade-io.hpp"
#include "lest.hpp"

#define CASE( name ) lest_CASE( specification(), name )

extern lest::tests & specification();

using namespace num;

CASE( "biquad-cascade: Allows to default construct" )
{
    const auto N = 7;

    BiQuadCascadeT<float, N> bqc;

    EXPECT( bqc.size() == 0 );
    EXPECT( bqc.reserved() == N );
}

// -----------------------------------------------------------------------
// Applets

CASE( "biquad-cascade: printing" " [.app][.print]" )
{
}
