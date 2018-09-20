//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "num/biquad.hpp"
#include "num/biquad-io.hpp"
#include "lest.hpp"

#define CASE( name ) lest_CASE( specification(), name )

extern lest::tests & specification();

using namespace num;

CASE( "biquad: Allows to default construct" )
{
    BiQuadT<float> bq;

    EXPECT( bq.state().v1 == 0.0f );
    EXPECT( bq.state().v2 == 0.0f );
}

// -----------------------------------------------------------------------
// Applets

CASE( "biquad: printing" " [.app][.print]" )
{
}
