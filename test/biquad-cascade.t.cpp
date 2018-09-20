//
// Copyright 2018 by Martin Moene
//
// https://gi-thub.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "num/biquad-cascade.hpp"
#include "num/biquad-cascade-io.hpp"
#include "lest.hpp"

#define CASE( name ) lest_CASE( specification(), name )

extern lest::tests & specification();

using namespace num;

CASE( "biquad-cascade: Allows to default construct an empty cascade" )
{
    const auto N = 7;

    BiQuadCascadeT<float, N> bqc;

    EXPECT( bqc.size() == 0 );
    EXPECT( bqc.reserved() == N );
}

CASE( "biquad-cascade: Allows to construct from 1 up to Nmax bi-quads" )
{
    const auto N = 3;

    using BiQuad = BiQuadT<float>;

    BiQuadCascadeT<float, N> bqc
    (
        BiQuad{  { 1, 0, 0 }, { 0, 0 }}
        , BiQuad{{ 2, 0, 0 }, { 0, 0 }}
        , BiQuad{{ 3, 0, 0 }, { 0, 0 }}
    );

    EXPECT( bqc.size() == 3 );
    EXPECT( bqc.reserved() == N );
    EXPECT( bqc.coeff_b( 0 ).b0 == 1 );
    EXPECT( bqc.coeff_b( 1 ).b0 == 2 );
    EXPECT( bqc.coeff_b( 2 ).b0 == 3 );
}

CASE( "biquad-cascade: Allows to obtain the maximum number of bi-quads the cascade can hold" )
{
    const auto N = 3;

    BiQuadCascadeT<float, N> bqc;

    EXPECT(  bqc.reserved() == N );
    EXPECT( (BiQuadCascadeT<float, N>::reserved()) == N );
}

CASE( "biquad-cascade: Allows to obtain the number of active bi-quads in the cascade" )
{
    BiQuadT<float> bq{};
    BiQuadCascadeT<float, 7> bqc( bq, bq, bq, bq, bq );

    EXPECT( bqc.size() == 5 );
}

CASE( "biquad-cascade: Allows to append a bi-quad" )
{
    using BiQuad = BiQuadT<float>;

    BiQuadCascadeT<float, 3> bqc;

    bqc.append( BiQuad{{ 1, 0, 0 }} );

    EXPECT( bqc.size() == 1 );
    EXPECT( bqc.coeff_b( 0 ).b0 == 1 );

    bqc.append( BiQuad{{ 2, 0, 0 }} );

    EXPECT( bqc.size() == 2 );
    EXPECT( bqc.coeff_b( 0 ).b0 == 1 );
    EXPECT( bqc.coeff_b( 1 ).b0 == 2 );
}

CASE( "biquad-cascade: Allows to insert a bi-quad at position i" )
{
    using BiQuad = BiQuadT<float>;

    BiQuadCascadeT<float, 4> bqc
    (
        BiQuad{  { 1, 0, 0 }}
        , BiQuad{{ 2, 0, 0 }}
        , BiQuad{{ 3, 0, 0 }}
    );

    EXPECT( bqc.size() == 3 );

    bqc.insert( 1, BiQuad{{ 7, 0, 0 }} );

    EXPECT( bqc.size() == 4 );
    EXPECT( bqc.coeff_b( 0 ).b0 == 1 );
    EXPECT( bqc.coeff_b( 1 ).b0 == 7 );
    EXPECT( bqc.coeff_b( 2 ).b0 == 2 );
}

CASE( "biquad-cascade: Allows to replace the bi-quad at position i" )
{
    using BiQuad = BiQuadT<float>;

    BiQuadCascadeT<float, 3> bqc
    (
        BiQuad{  { 1, 0, 0 }}
        , BiQuad{{ 2, 0, 0 }}
        , BiQuad{{ 3, 0, 0 }}
    );

    auto prv = bqc.replace( 1, BiQuad{{ 7, 0, 0 }} );

    EXPECT( prv.coeff_b(   ).b0 == 2 );
    EXPECT( bqc.coeff_b( 0 ).b0 == 1 );
    EXPECT( bqc.coeff_b( 1 ).b0 == 7 );
    EXPECT( bqc.coeff_b( 2 ).b0 == 3 );
}

CASE( "biquad-cascade: Allows to remove the bi-quad at position i from the cascade" )
{
    using BiQuad = BiQuadT<float>;

    BiQuadCascadeT<float, 3> bqc
    (
        BiQuad{  { 1, 0, 0 }}
        , BiQuad{{ 2, 0, 0 }}
        , BiQuad{{ 3, 0, 0 }}
    );

    EXPECT( bqc.size() == 3 );

    bqc.remove( 0 );

    EXPECT( bqc.size() == 2 );
    EXPECT( bqc.coeff_b( 0 ).b0 == 2 );
    EXPECT( bqc.coeff_b( 1 ).b0 == 3 );
}

CASE( "biquad-cascade: Allows to remove all bi-quads from the cascade via clear()" )
{
    using BiQuad = BiQuadT<float>;

    BiQuadCascadeT<float, 3> bqc
    (
        BiQuad{  { 1, 0, 0 }}
        , BiQuad{{ 2, 0, 0 }}
        , BiQuad{{ 3, 0, 0 }}
    );

    EXPECT( bqc.size() == 3 );

    bqc.clear();

    EXPECT( bqc.size() == 0 );
}

CASE( "biquad-cascade: Allows to obtain the a-coefficients of the bi-quad at position i" )
{
    using BiQuad = BiQuadT<float>;

    BiQuadCascadeT<float, 3> bqc
    (
        BiQuad{  { 0, 0, 0 }, {-1, -11}}
        , BiQuad{{ 0, 0, 0 }, {-2, -22}}
        , BiQuad{{ 0, 0, 0 }, {-3, -33}}
    );

    EXPECT( bqc.coeff_a( 0 ).a1 == -1 ); EXPECT( bqc.coeff_a( 0 ).a2 == -11 );
    EXPECT( bqc.coeff_a( 1 ).a1 == -2 ); EXPECT( bqc.coeff_a( 1 ).a2 == -22 );
    EXPECT( bqc.coeff_a( 2 ).a1 == -3 ); EXPECT( bqc.coeff_a( 2 ).a2 == -33 );
}

CASE( "biquad-cascade: Allows to obtain the b-coefficients of the bi-quad at position i" )
{
    using BiQuad = BiQuadT<float>;

    BiQuadCascadeT<float, 3> bqc
    (
        BiQuad{  { 1, 11, 111 }}
        , BiQuad{{ 2, 22, 222 }}
        , BiQuad{{ 3, 33, 333 }}
    );

    EXPECT( bqc.coeff_b( 0 ).b0 == 1 ); EXPECT( bqc.coeff_b( 0 ).b1 == 11 ); EXPECT( bqc.coeff_b( 0 ).b2 == 111 );
    EXPECT( bqc.coeff_b( 1 ).b0 == 2 ); EXPECT( bqc.coeff_b( 1 ).b1 == 22 ); EXPECT( bqc.coeff_b( 1 ).b2 == 222 );
    EXPECT( bqc.coeff_b( 2 ).b0 == 3 ); EXPECT( bqc.coeff_b( 2 ).b1 == 33 ); EXPECT( bqc.coeff_b( 2 ).b2 == 333 );
}

CASE( "biquad-cascade: Allows to obtain the state of the bi-quad at position i" )
{
    using BiQuad = BiQuadT<float>;

    BiQuadCascadeT<float, 3> bqc
    (
        BiQuad{  { 1, 0, 0 }}
        , BiQuad{{ 2, 0, 0 }}
        , BiQuad{{ 3, 0, 0 }}
    );

    EXPECT( bqc.state( 1 ).v1 == 0 );
    EXPECT( bqc.state( 1 ).v2 == 0 );

    for ( auto i = 0; i < 7; ++i )
    {
        step( bqc, 1.23f );
    }

    EXPECT( bqc.state( 1 ).v1 != 0 );
    EXPECT( bqc.state( 1 ).v2 != 0 );
}

CASE( "biquad-cascade: Allows to iterate over the cascade" )
{
    using BiQuad = BiQuadT<float>;

    BiQuadCascadeT<float, 3> bqc
    (
        BiQuad{  { 1, 0, 0 }}
        , BiQuad{{ 2, 0, 0 }}
        , BiQuad{{ 3, 0, 0 }}
    );

    auto cnt = 0;
    for ( auto & bq : bqc )
    {
        ++cnt;
    }

    EXPECT( cnt == bqc.size() );
}

CASE( "biquad-cascade: Allows to use a user-defined arithmetic type for the bi-quads" )
{
    // std::complex stands-in for a user-defined arithmetic type:

    using udt = std::complex<float>;

    using BiQuad = BiQuadT<udt>;

    BiQuadCascadeT<udt, 3> bqc
    (
        BiQuad{  { 1, 11, 111 }}
        , BiQuad{{ 2, 22, 222 }}
        , BiQuad{{ 3, 33, 333 }}
    );

    step( bqc, 1.23f );
}

CASE( "biquad-cascade: is_stable() is true if the all bi-quads of the cascade are stable" )
{
    using BiQuad = BiQuadT<float>;

    {
        BiQuadCascadeT<float, 3> bqc
        (
            BiQuad{  { 1, 0, 0 }}
            , BiQuad{{ 2, 0, 0 }}
        );

        EXPECT( is_stable( bqc) );
    }{
        BiQuadCascadeT<float, 3> bqc
        (
            BiQuad{  { 1, 0, 0 }, { 1.2f, -1.2f }}
            , BiQuad{{ 2, 0, 0 }}
        );

        EXPECT_NOT( is_stable( bqc) );
    }
}

CASE( "biquad-cascade: reset() clears the state of all bi-quads of the cascade" )
{
    using BiQuad = BiQuadT<float>;

    BiQuadCascadeT<float, 3> bqc
    (
        BiQuad{  { 1, 0, 0 }}
        , BiQuad{{ 2, 0, 0 }}
        , BiQuad{{ 3, 0, 0 }}
    );

    for ( auto i = 0; i < 7; ++i )
    {
        step( bqc, 1.23f );
    }

    EXPECT( bqc.state( 1 ).v1 != 0 );
    EXPECT( bqc.state( 1 ).v2 != 0 );

    reset( bqc );

    EXPECT( bqc.state( 1 ).v1 == 0 );
    EXPECT( bqc.state( 1 ).v2 == 0 );
}


CASE( "biquad-cascade: response() yields the cascade's complex response for the given normalized frequency" )
{
    using BiQuad = BiQuadT<float>;

    BiQuadCascadeT<float, 3> bqc
    (
        BiQuad{  { 1, 0, 0 }}
        , BiQuad{{ 2, 0, 0 }}
        , BiQuad{{ 3, 0, 0 }}
    );

    EXPECT( abs( response( bqc, 0.0f ) ) == 6 );
}

CASE( "biquad-cascade: step() yields the new output of the cascade for the given input (using an unspecified form IIR)" )
{
    using BiQuad = BiQuadT<float>;

    BiQuadCascadeT<float, 3> bqc
    (
        BiQuad{  { 1, 0, 0 }}
        , BiQuad{{ 2, 0, 0 }}
        , BiQuad{{ 3, 0, 0 }}
    );

    EXPECT( step( bqc, 0.0f ) == 0 );
    EXPECT( step( bqc, 1.0f ) == 6 );
}

CASE( "biquad-cascade: step_df2() yields the new output of the cascade for the given input (using direct form 2 IIR" )
{
    using BiQuad = BiQuadT<float>;

    BiQuadCascadeT<float, 3> bqc
    (
        BiQuad{  { 1, 0, 0 }}
        , BiQuad{{ 2, 0, 0 }}
        , BiQuad{{ 3, 0, 0 }}
    );

    EXPECT( step_df2( bqc, 0.0f ) == 0 );
    EXPECT( step_df2( bqc, 1.0f ) == 6 );
}

CASE( "biquad-cascade: step_df2_transposed() yields the new output of the cascade for the given input (using direct form 2, transposed IIR" )
{
    using BiQuad = BiQuadT<float>;

    BiQuadCascadeT<float, 3> bqc
    (
        BiQuad{  { 1, 0, 0 }}
        , BiQuad{{ 2, 0, 0 }}
        , BiQuad{{ 3, 0, 0 }}
    );

    EXPECT( step_df2_transposed( bqc, 0.0f ) == 0 );
    EXPECT( step_df2_transposed( bqc, 1.0f ) == 6 );
}

// -----------------------------------------------------------------------
// Applets

namespace{

using Count= int;
using Text = std::string;

template< typename T, Count BqMax >
auto print( Text text, Text design, BiQuadCascadeT<T,BqMax> && bqc )
{
    std::cout << "\n" << text << " " << design << ":\n" << bqc << "\n";
}
} // anonymous namespace

CASE( "biquad-cascade: printing" " [.app][biquad][cascade]" )
{
    const auto BqMax = 7;

    using BiQuad = BiQuadT<double>;
    using BiQuadCascade = BiQuadCascadeT<double, BqMax>;

    print
    (
        "Bi-quad filter design", "[b,a] = cheby2(6,20,0.4)",
        BiQuadCascade
        (
            BiQuad{   { 0.158682, 0.245885, 0.158682 }, {  0.239123, 0.084944 } }
            , BiQuad{ { 1.000000, 0.054217, 1.000000 }, { -0.264548, 0.416971 } }
            , BiQuad{ { 1.000000,-0.554665, 1.000000 }, { -0.701098, 0.797053 } }
        )
    );
}
