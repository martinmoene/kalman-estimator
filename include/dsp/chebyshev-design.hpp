//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Contains C++ implementations of code from the book:
// Sophocles J. Orfanidis. Introduction to Signal Processing,
// http://www.ece.rutgers.edu/~orfanidi/intro2sp/

#ifndef DSP_CHEBYSHEV_DESIGN_HPP_INCLUDED
#define DSP_CHEBYSHEV_DESIGN_HPP_INCLUDED

#include "dsp/biquad-cascade.hpp"
#include "num/constants.hpp"
#include "core/range.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <complex>
#include <numeric>
#include <type_traits>
#include <vector>

// DSP types and algorithms:

namespace dsp { namespace chebyshev_design {

// a less-distractive way to write a static_cast:

template< typename T >
inline auto to_int( T x )
{
    return static_cast<int>( x );
}

// ToDo
using biquad::AT;
using biquad::BT;

// provide constructor to AT:

template< typename T >
struct AAT : biquad::AT<T>
{
    template< typename... Args >
    AAT( Args... args )
    : biquad::AT<T>{ T(args)...}
    {}
};

// provide constructor to AB:

template< typename T >
struct BBT : biquad::BT<T>
{
    template< typename... Args >
    BBT( Args... args )
    : biquad::BT<T>{ T(args)...}
    {}
};

// internal information from filter design algorithms:

template< typename T >
struct AlgoInfoT
{
    T Wpass, Wstop, epass, estop, Nex; int N; T f3, a;
};

// digital filter bi-quad coefficients from filter design:

template< typename T >
struct DigitalCoeffT
{
    template< typename... Args >
    DigitalCoeffT( Args... args )
    : info{ args...}
    {}

    auto size() const
    {
        return b.size();
    }

    std::vector<BBT<T>> b;
    std::vector<AAT<T>> a;

    AlgoInfoT<T> info;
};

// filter type selection (lp, hp, ...)

enum class FilterResponse
{
    lowpass,
    highpass,
};

// algorithm: pre-warp frequency:

template< typename P, typename Q >
inline auto prewarp( P f, Q fs )
{
    using T = std::common_type_t<float, P, Q>;

    return std::tan( num::pi<T> * f / fs );
}

// Chebyshev 1 lowpass, highpass
//
// Lowpass/Highpass Chebyshev Type 1 Filter Design, lhcheb1.m
// From: Sophocles J. Orfanidis. Introduction to Signal Processing, p.753
//
// http://www.ece.rutgers.edu/~orfanidi/intro2sp/
// http://www.ece.rutgers.edu/~orfanidi/intro2sp/mdir/lhcheb1.m

template< typename T >
auto chebyshev1_lp_hp_impl( FilterResponse kind, T fs, T fpass, T fstop, T Apass, T Astop )
{
    assert( kind == FilterResponse::lowpass || kind == FilterResponse::highpass );
    assert( fpass < fs / 2 );
    assert( fstop < fs / 2 );

    using complex_t = std::complex<T>;

    const auto s = kind == FilterResponse::lowpass ? T{1} : T{-1};

    const auto Wpass = std::pow( prewarp( fpass, fs ), s );
    const auto Wstop = std::pow( prewarp( fstop, fs ), s );

    const auto epass = std::sqrt( std::pow<T>( 10, Apass / 10 ) - 1 );
    const auto estop = std::sqrt( std::pow<T>( 10, Astop / 10 ) - 1 );

    const auto Nex   = std::acosh( estop / epass ) / std::acosh( Wstop / Wpass );
    const auto N     = to_int( std::ceil( Nex ) );
    const auto r     = N % 2;
    const auto K     = ( N - r ) / 2;

    const auto a     = std::asinh( estop ) / N;
    const auto W3    = real( Wpass * std::cosh( std::acosh( complex_t{1} / complex_t{epass} ) / complex_t(N) ) );
    const auto f3    = (fs / num::pi<T> ) * std::atan( std::pow( W3, s ) );
    const auto W0    = std::sinh( a ) * Wpass;

    DigitalCoeffT<T> coeff{ Wpass, Wstop, epass, estop, Nex, N, f3, a };

    auto & A = coeff.a;
    auto & B = coeff.b;

    if ( r == 1 )
    {
        const auto G = 1 / (1 + W0);

        A.emplace_back( /*1,*/ s * ( 2 * G - 1 ), T{0} );
        B.emplace_back(   G,   s * G            , T{0} );
    }

    for ( auto i : core::range( 1, K+1 ) )        // [1..K+1)
    {
        const auto th = num::pi<T> * (N - 1 + 2 * i) / (2 * N);
        const auto Wi = Wpass * sin(th);
        const auto D  =  1 - 2 * W0 * cos( th ) + W0 * W0 + Wi * Wi;
        const auto G  = (W0 * W0 + Wi * Wi ) / D;
        const auto a1 =  2 * (W0 * W0 + Wi * Wi - 1) / D;
        const auto a2 = (1 + 2 * W0 * cos(th) + W0 * W0 + Wi * Wi) / D;

        A.emplace_back( /*1,*/ s * a1, a2 );
        B.emplace_back( G, G * s * 2, G  );
    }

    return coeff;
}

// Chebyshev 1 lowpass, highpass: allow for parameters of mixed type:

template< typename FS, typename FP, typename FT, typename AP, typename AS  >
inline auto chebyshev1_lp_hp( FilterResponse kind, FS fs, FP fpass, FT fstop, AP Apass, AS Astop )
{
    using T = std::common_type_t<float, FS, FP, FT, AP, AS>;

    return chebyshev1_lp_hp_impl<T>(
        kind, fs, fpass, fstop, (std::max<T>)( Apass, 0.01 ), (std::max<T>)( Astop, 0.01 ) );
}

// Chebyshev 2 lowpass, highpass
//
// Lowpass/Highpass Chebyshev Type 2 Filter Design, lhcheb2.m
// From: Sophocles J. Orfanidis. Introduction to Signal Processing, p.754
//
// http://www.ece.rutgers.edu/~orfanidi/intro2sp/
// http://www.ece.rutgers.edu/~orfanidi/intro2sp/mdir/lhcheb2.m

template< typename T >
auto chebyshev2_lp_hp_impl( FilterResponse kind, T fs, T fpass, T fstop, T Apass, T Astop )
{
    assert( kind == FilterResponse::lowpass || kind == FilterResponse::highpass );
    assert( fpass < fs / 2 );
    assert( fstop < fs / 2 );

    using core::range;
    using complex_t = std::complex<T>;

    const auto s = kind == FilterResponse::lowpass ? T{1} : T{-1};

    const auto Wpass = std::pow( prewarp( fpass, fs ), s );
    const auto Wstop = std::pow( prewarp( fstop, fs ), s );

    const auto epass = std::sqrt( std::pow<T>( 10, Apass / 10 ) - 1 );
    const auto estop = std::sqrt( std::pow<T>( 10, Astop / 10 ) - 1 );

    const auto Nex   = std::acosh( estop / epass ) / std::acosh( Wstop / Wpass );
    const auto N     = to_int( std::ceil( Nex ) );
    const auto r     = N % 2;
    const auto K     = ( N - r ) / 2;

    const auto a     = std::asinh( estop ) / N;
    const auto W3    = real( Wstop / std::cosh( std::acosh( complex_t{estop} ) / complex_t(N) ) );
    const auto f3    = (fs / num::pi<T> ) * std::atan( std::pow( W3, s ) );
    const auto W0    = std::sinh( a ) / Wstop;      // reciprocal of text

    DigitalCoeffT<T> coeff{ Wpass, Wstop, epass, estop, Nex, N, f3, a };

    auto & A = coeff.a;
    auto & B = coeff.b;

    if ( r == 1 )
    {
        const auto G = 1 / (1 + W0);

        A.emplace_back( /*1,*/ s * ( 2 * G - 1 ), T{0} );
        B.emplace_back(   G,   s * G            , T{0} );
    }

    for ( auto i : core::range( 1, K+1 ) )        // [1..K+1)
    {
        const auto th = num::pi<T> * (N - 1 + 2 * i) / (2 * N);
        const auto Wi = sin(th) / Wstop;        // reciprocal of text
        const auto D  =  1 - 2 * W0 * cos( th ) + W0 * W0 + Wi * Wi;
        const auto G  = (1 + Wi * Wi ) / D;
        const auto b1 =  2 * (1 - Wi * Wi) / (1 + Wi * Wi);
        const auto a1 =  2 * (1 - W0 * W0 - Wi * Wi) / D;
        const auto a2 = (1 + 2 * W0 * cos(th) + W0 * W0 + Wi * Wi) / D;

        A.emplace_back( /*1,*/ s * a1, a2 );
        B.emplace_back( G, G * s * b1, G  );
    }

    return coeff;

//    return DigitalCoeffT<T>
//    (
//        B,
//        , A
//        , { Wpass, Wstop, epass, estop, Nex, N, f3, a }
//    );
}

// Chebyshev 2 lowpass, highpass: allow for parameters of mixed type:

template< typename FS, typename FP, typename FT, typename AP, typename AS  >
inline auto chebyshev2_lp_hp( FilterResponse kind, FS fs, FP fpass, FT fstop, AP Apass, AS Astop )
{
    using T = std::common_type_t<float, FS, FP, FT, AP, AS>;

    return chebyshev2_lp_hp_impl<T>(
        kind, fs, fpass, fstop, (std::max<T>)( Apass, 0.01 ), (std::max<T>)( Astop, 0.01 ) );
}

// Chebyshev 2 band stop
//
// Lowpass/Highpass Chebyshev Type 2 Filter Design, bscheb2.m
// From: Sophocles J. Orfanidis. Introduction to Signal Processing, p.756
//
// http://www.ece.rutgers.edu/~orfanidi/intro2sp/
// http://www.ece.rutgers.edu/~orfanidi/intro2sp/mdir/lhcheb2.m

template< typename T >
auto chebyshev2_bs_impl( T fs, T fpa, T fpb, T fsa, T fsb, T Apass, T Astop )
{
    DigitalCoeffT<T> coeff;

    return coeff;

#if 0
    function [A, B, P] = bscheb2(fs, fpa, fpb, fsa, fsb, Apass, Astop)

    c = sin(2*pi*(fpa + fpb)/fs)/(sin(2*pi*fpa/fs) + sin(2*pi*fpb/fs));
    Wpass = abs(sin(2*pi*fpb/fs) / (cos(2*pi*fpb/fs) - c));
    Wsa = sin(2*pi*fsa/fs) / (cos(2*pi*fsa/fs) - c);
    Wsb = sin(2*pi*fsb/fs) / (cos(2*pi*fsb/fs) - c);
    Wstop = min(abs(Wsa), abs(Wsb));

    epass = sqrt(10^(Apass/10) - 1);
    estop = sqrt(10^(Astop/10) - 1);

    Nex = acosh(estop/epass) / acosh(Wstop/Wpass);
    N = ceil(Nex);  r = rem(N,2);  K = (N - r) / 2;

    a = asinh(estop) / N;
    W3 = Wstop / cosh(acosh(estop)/N);
    f3a = (fs/pi) * atan((sqrt(1 + W3^2 *(1 - c^2)) - 1)/(W3*(c+1)));
    f3b = (fs/pi) * atan((sqrt(1 + W3^2 *(1 - c^2)) + 1)/(W3*(c+1)));
    P = [Wpass, Wstop, Wsa, Wsb, c, epass, estop, Nex, N, f3a, f3b, a];
    W0 = sinh(a) / Wstop;                                   % reciprocal of text

    if r==1,
        G = 1 / (1 + W0);
        a1 = -2 * c / (1 + W0);
        a2 = (1 - W0) / (1 + W0);
        A(1,:) = [1, a1, a2, 0, 0];
        B(1,:) = G * [1, -2*c, 1, 0, 0];
    else
        A(1,:) = [1, 0, 0, 0, 0];
        B(1,:) = [1, 0, 0, 0, 0];
    end

    for i=1:K,
        th = pi * (N - 1 + 2 * i) / (2 * N);
        Wi = sin(th) / Wstop;                               % reciprocal of text
        D = 1 - 2 * W0 * cos(th) + W0^2 + Wi^2;
        G = (1 + Wi^2) / D;
        b1 = -4 * c / (1 + Wi^2);
        b2 = 2 * ((2*c^2 + 1) - Wi^2) / (1 + Wi^2);
        a1 = -4 * c * (1 - W0 * cos(th)) / D;
        a2 = 2 * (2*c^2 + 1 - W0^2 - Wi^2) / D;
        a3 = -4 * c * (1 + W0 * cos(th)) / D;
        a4 = (1 + 2 * W0 * cos(th) + W0^2 + Wi^2) / D;
        A(i+1,:) = [1, a1, a2, a3, a4];
        B(i+1,:) = G * [1, b1, b2, b1, 1];
    end
#endif
}

// Chebyshev 2 band stop: allow for parameters of mixed type:

template< typename FS, typename FPA, typename FPB, typename FSA, typename FSB, typename AP, typename AS  >
inline auto chebyshev2_bs( FS fs, FPA fpa, FPB fpb, FSA fsa, FSB fsb, AP Apass, AS Astop )
{
    using T = std::common_type_t<float, FS, FPA, FPB, FSA, FSB, AP, AS>;

    return chebyshev2_bs_impl<T>(
        fs, fpa, fpb, fsa, fsb, (std::max<T>)( Apass, 0.01 ), (std::max<T>)( Astop, 0.01 ) );
}

// algorithm: create a bi-quad cascade from digital filter coefficients:

template< typename T, int N >
inline auto make_biquad_cascade( DigitalCoeffT<T> const & coeff )
{
    BiQuadCascadeT<T, N> bqc;

    for ( auto i : core::indices( coeff ) )
    {
        bqc.append({ { coeff.b[i] }, { coeff.a[i] } });
    }

    return bqc;
}

} // namespace chebyshev_design

using chebyshev_design::FilterResponse;
using chebyshev_design::DigitalCoeffT;

using chebyshev_design::chebyshev1_lp_hp;
using chebyshev_design::chebyshev2_lp_hp;
using chebyshev_design::chebyshev2_bs;
//using chebyshev_design::chebyshev2_bp;
using chebyshev_design::make_biquad_cascade;

} // namespace dsp

#endif // DSP_CHEBYSHEV_DESIGN_HPP_INCLUDED
