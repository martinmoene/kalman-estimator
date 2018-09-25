// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "dsp/biquad.hpp"
#include "dsp/biquad-io.hpp"
#include "lest.hpp"

#define CASE( name ) lest_CASE( specification(), name )

extern lest::tests & specification();

using lest::approx;
using namespace dsp;

CASE( "biquad: Allows to default construct a bi-quad with zero-initialized coefficients" )
{
    BiQuadT<float> bq;

    EXPECT( bq.coeff_a().a1 == approx( 0.0f ) );
    EXPECT( bq.coeff_a().a2 == approx( 0.0f ) );
    EXPECT( bq.coeff_b().b0 == approx( 0.0f ) );
    EXPECT( bq.coeff_b().b1 == approx( 0.0f ) );
    EXPECT( bq.coeff_b().b2 == approx( 0.0f ) );
    EXPECT( bq.state().v1   == approx( 0.0f ) );
    EXPECT( bq.state().v2   == approx( 0.0f ) );
}

CASE( "biquad: Allows to construct a bi-quad from b-coefficients" )
{
    BiQuadT<float> bq{{ 1, 2, 3 }};

    EXPECT( bq.coeff_b().b0 == approx( 1.0f ) );
    EXPECT( bq.coeff_b().b1 == approx( 2.0f ) );
    EXPECT( bq.coeff_b().b2 == approx( 3.0f ) );
}

CASE( "biquad: Allows to construct a bi-quad from b- and a-coefficients" )
{
    BiQuadT<float> bq{{ 1, 2, 3 }, { 4, 5 }};

    EXPECT( bq.coeff_b().b0 == approx( 1.0f ) );
    EXPECT( bq.coeff_b().b1 == approx( 2.0f ) );
    EXPECT( bq.coeff_b().b2 == approx( 3.0f ) );
    EXPECT( bq.coeff_a().a1 == approx( 4.0f ) );
    EXPECT( bq.coeff_a().a2 == approx( 5.0f ) );
}

CASE( "biquad: Allows to construct a bi-quad from zeros, poles and gain (z, p, k)" )
{
    using BiQuad  = BiQuadT<float>;

    BiQuad bq{ { 0.7f, 0.5f }, { 0.4f, -0.3f }, 10.0f };

    EXPECT( bq.zeros().s0.real() == approx(  0.7 ) );
    EXPECT( bq.zeros().s1.real() == approx(  0.5 ) );
    EXPECT( bq.poles().s0.real() == approx(  0.4 ) );
    EXPECT( bq.poles().s1.real() == approx( -0.3 ) );
    EXPECT( bq.coeff_b().b0      == approx( 10.0 ) );
}

CASE( "biquad: Allows to construct a bi-quad using a user-defined arithmetic type" )
{
    using udt = std::complex<float>;

    using BiQuad = BiQuadT<udt>;

    BiQuad bq{{ 1, 11, 111 }};

    bq.step( 1.23f );

    EXPECT( true );
}

CASE( "biquad: Allows to obtain the a-coefficients of the bi-quad via coeff_a()" )
{
    BiQuadT<float> bq{{ 1, 2, 3 }, { 4, 5 }};

    EXPECT( bq.coeff_a().a1 == approx( 4.0 ) );
    EXPECT( bq.coeff_a().a2 == approx( 5.0 ) );
}

CASE( "biquad: Allows to obtain the b-coefficients of the bi-quad via coeff_b()" )
{
    BiQuadT<float> bq{{ 1, 2, 3 }, { 4, 5 }};

    EXPECT( bq.coeff_b().b0 == approx( 1.0 ) );
    EXPECT( bq.coeff_b().b1 == approx( 2.0 ) );
    EXPECT( bq.coeff_b().b2 == approx( 3.0 ) );
}

CASE( "biquad: Allows to obtain the state of the bi-quad via state()" )
{
    BiQuadT<float> bq{{ 7, 0, 0 }};

    EXPECT( bq.state().v1 == approx( 0.0 ) );
    EXPECT( bq.state().v2 == approx( 0.0 ) );

    for ( auto i = 0; i < 7; ++i )
    {
        bq.step( 1.23f );
    }

    EXPECT( bq.step(1.23f) == approx( 7.0 * 1.23 ) );
    EXPECT( bq.state().v1  == approx( 1.23 ) );
    EXPECT( bq.state().v2  == approx( 1.23 ) );
}

CASE( "biquad: Allows to obtain the poles of the bi-quad via poles()" )
{
    using BiQuad  = BiQuadT<float>;

    BiQuad bq{ { 0.7f, 0.5f }, { 0.4f, -0.3f }, 10.0f };

    EXPECT( bq.poles().s0.real() == approx(  0.4 ) );
    EXPECT( bq.poles().s1.real() == approx( -0.3 ) );
}

CASE( "biquad: Allows to obtain the zeros of the bi-quad via zeros()" )
{
    using BiQuad  = BiQuadT<float>;

    BiQuad bq{ { 0.7f, 0.5f }, { 0.4f, -0.3f }, 10.0f };

    EXPECT( bq.zeros().s0.real() == approx(  0.7 ) );
    EXPECT( bq.zeros().s1.real() == approx(  0.5 ) );
}

CASE( "biquad: Allows to determine if the bi-quad is stable, via is_stable()" )
{
    using BiQuad  = BiQuadT<float>;

    BiQuad bqs{ { 1.0f, 0.0f }, { 0.2f, -0.2f }};
    BiQuad bqns{{ 1.0f, 0.0f }, { 1.2f, -0.9f }};

    EXPECT(      bqs.is_stable() );
    EXPECT_NOT( bqns.is_stable() );
}

CASE( "biquad: Allows to clear the state the bi-quad via reset()" )
{
    BiQuadT<float> bq{{ 7, 0, 0 }};

    for ( auto i = 0; i < 7; ++i )
    {
        bq.step( 1.23f );
    }

    EXPECT( bq.state().v1  != approx( 0 ) );
    EXPECT( bq.state().v2  != approx( 0 ) );

    bq.reset();

    EXPECT( bq.state().v1  == approx( 0 ) );
    EXPECT( bq.state().v2  == approx( 0 ) );
}

CASE( "biquad: Allows to obtains the bi-quad's complex response for the given normalized frequency via response()" )
{
    BiQuadT<float> bq{{ 7, 0, 0 }};

    EXPECT( abs( response( bq, 0.0f ) ) == approx( 7 ) );
}

CASE( "biquad: Allows to obtain the new output of the bi-quad for the given input via step() (using an unspecified form IIR)" )
{
    BiQuadT<float> bq{{ 7, 0, 0 }};

    EXPECT( bq.step( 0.0f ) == approx( 0 ) );
    EXPECT( bq.step( 1.0f ) == approx( 7 ) );
}

CASE( "biquad: Allows to obtain the new output of the bi-quad for the given input via step_df2()" )
{
    BiQuadT<float> bq{{ 7, 0, 0 }};

    EXPECT( bq.step_df2( 0.0f ) == approx( 0 ) );
    EXPECT( bq.step_df2( 1.0f ) == approx( 7 ) );
}

CASE( "biquad: Allows to obtain the new output of the bi-quad for the given input via step_df2_transposed()" )
{
    BiQuadT<float> bq{{ 7, 0, 0 }};

    EXPECT( bq.step_df2_transposed( 0.0f ) == approx( 0 ) );
    EXPECT( bq.step_df2_transposed( 1.0f ) == approx( 7 ) );
}


CASE( "biquad: solve() solves a quadratic polynomial: ax^2 + bx + c -> k(x - z1)(x - z2)" )
{
//    BiQuad is stable:
//    a: [A -0.1, -0.12 ]
// => b: [B 10, -12, 3.5 ]
//    w: [W 0, 0 ]
//    p: [S (0.4,0), (-0.3,0) ]
// => z: [S (0.7,0), (0.5,0) ]

    auto s = solve( 10.0, -12.0, 3.5 );

    EXPECT( s.s0.real() == approx( 0.7 ) );
    EXPECT( s.s0.imag() == approx( 0.0 ) );
    EXPECT( s.s1.real() == approx( 0.5 ) );
    EXPECT( s.s1.imag() == approx( 0.0 ) );
}

// -----------------------------------------------------------------------
// Applets

namespace{

using Text = std::string;

template< typename T >
auto print( Text text, Text design, BiQuadT<T> && bq )
{
    std::cout << "\n" << text << " " << design << ":\n" << bq << "\n";
}
} // anonymous namespace

CASE( "biquad: printing" " [.app][.print][biquad][2]" )
{
    using BiQuad = BiQuadT<double>;

    print
    (
        "Bi-quad filter design", "[b,a] = cheby2(2,20,0.4)",
        BiQuad{{ 0.1333591, 0.0072304, 0.1333591 }, { -1.1604647, 0.4344132 }}
    );

    EXPECT( true );
}
