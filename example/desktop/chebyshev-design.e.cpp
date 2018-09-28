// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "dsp/chebyshev-design-io.hpp"
#include "dsp/biquad-cascade-io.hpp"
#include "core/core.hpp"

#ifndef  CONFIG_USE_LOWPASS
# define CONFIG_USE_LOWPASS  1

#endif
#ifndef  CONFIG_USE_BAND_STOP
# define CONFIG_USE_BAND_STOP  1
#endif

using core::Text;
using core::line;
using core::replaced;

// print information for the given digital filter coefficients:
// 1. the digital filter design coefficient information;
// 2. the frequency magnitude response of a corresponding bi-quad filter cascade
//    as Matlab/Octave code to create a plot;
// 3. the corresponding Matlab/Octave filter design code to create
//    a frequency response plot in dB.

template< typename Rng, typename Coeff >
void print_pass( Rng && rng, Text text, Text design, Coeff && coeff )
{
    const auto maxBiquad = 7;

    auto bqc = dsp::make_biquad_cascade<maxBiquad>( coeff );

    std::cout <<
        "\n"     << line(115, '=') <<
        "\nai: " << coeff.info     <<
        "\nb: "  << coeff.b        <<
        "\na: "  << coeff.a        <<
        "\n"     << line(115)      <<
        "\n";

    std::cout  <<
        "\n"   << text << " '" << design << "':\n" << bqc <<
        "\n% " << line(65) <<
        "\n% Matlab/Octave filter magnitude response for normalized frequency:\n" <<
        "\nfn = [";

    for ( auto fn : rng )
    {
        std::cout << fn << " ";
    }

    std::cout <<
        "];\n"
        "\ny = [";

    for ( auto fn : rng )
    {
        std::cout << abs( response( bqc, fn ) ) << " ";
    }

    std::cout <<
        "];\n"
        "\nydbV = [";

    for( auto fn : rng )
    {
        std::cout << dsp::dbV( abs( response( bqc, fn ) ), 1 ) << " ";
    }

    // edit C++ function call 'chebyX_lp_hp' to Matlab call 'lhchebyX':

    const auto matlab_design =
        replaced( replaced( replaced( replaced( replaced( design,
        "dsp::", "" ),
        "chebyshev1_lp_hp", "lhcheb1" ),
        "chebyshev2_lp_hp", "lhcheb2" ),
        "FilterResponse::low_pass", "1" ),
        "FilterResponse::high_pass", "-1" );

    std::cout <<
        "];\n"
        "\nplotyy(fn, y, fn, ydbV);"
        "\n" <<
        "\n% " << line(42) <<
        "\n% Corresponding Matlab/Octave filter design:" <<
        "\n"
        "\n[A,B,P] = " << matlab_design <<
        "\n% p.1  : s (1:lowpass,-1:highpass)" <<
        "\n% p.2-4: fs, fpass, fstop" <<
        "\n% p.5,6: Apass, Astop amplitude in dB" <<
        "\n% A: matrix of bi-quad a coefficients" <<
        "\n% B: matrix of bi-quad b coefficients" <<
        "\n% P: algorithm internal information" <<
        "\n"
        "\n% Concatenate horizontally, convert and plot:"
        "\nsos = cat(2,B,A)"
        "\n[b,a] = sos2tf(sos)"
        "\nfreqz(b,a);\n";
}

template< typename Rng, typename Coeff >
void print_band( Rng && rng, Text text, Text design, Coeff && coeff )
{
    using value_type = typename Coeff::value_type;

    const auto maxBiquad = 7;

// [A,B,P] = bscheb2( 100, 10, 35, 15, 30, 1, 10 )
// A =
//
//   1.00000  -0.32557   0.47164   0.00000   0.00000
//   1.00000  -0.60648   0.85511  -0.42187   0.58278
//
// B =
//
//   0.73582  -0.32557   0.73582   0.00000   0.00000
//   0.71650  -0.51417   1.00490  -0.51417   0.71650
//
// P =
//
//    1.00000    1.79360    2.20709   -1.79360    0.22123    0.50885    3.00000    2.06970    3.00000   12.79294   31.28641    0.60615

    dsp::BiQuadCascadeT<value_type, maxBiquad> bqc;

//    bqc.append({ { 1.00000,  -0.32557,   0.47164 }, {   0.00000 , 0.00000 } });
    bqc.append({ { 1.00000,  -0.60648,   0.85511 }, {  -0.42187 , 0.58278 } });
//    bqc.append({ { 0.73582,  -0.32557,   0.73582 }, {   0.00000 , 0.00000 } });
//    bqc.append({ { 0.71650,  -0.51417,   1.00490 }, {  -0.51417 , 0.71650 } });

//    auto bqc = dsp::make_biquad_cascade<maxBiquad>( coeff );

    std::cout <<
        "\n"     << line(115, '=') <<
        "\nai: " << coeff.info     <<
        "\nb: "  << coeff.b        <<
        "\na: "  << coeff.a        <<
        "\n"     << line(115) <<
        "\n";

    std::cout  <<
        "\n"   << text << " '" << design << "':\n" << bqc <<
        "\n% " << line(65) <<
        "\n% Matlab/Octave filter magnitude response for normalized frequency:\n" <<
        "\nfn = [";

    for ( auto fn : rng )
    {
        std::cout << fn << " ";
    }

    std::cout <<
        "];\n"
        "\ny = [";

    for ( auto fn : rng )
    {
        std::cout << abs( response( bqc, fn ) ) << " ";
    }

    std::cout <<
        "];\n"
        "\nydbV = [";

    for( auto fn : rng )
    {
        std::cout << dsp::dbV( abs( response( bqc, fn ) ), 1 ) << " ";
    }

    // edit C++ function call 'chebyX_lp_hp' to Matlab call 'lhchebyX':

    const auto matlab_design = replaced( replaced( design,
        "dsp::", "" ),
        "chebyshev2_bs", "bscheb2" );

    std::cout <<
        "];\n"
        "\nplotyy(fn, y, fn, ydbV);"
        "\n" <<
        "\n% " << line(42, '-') <<
        "\n"
        "\n[A,B,P] = " << matlab_design <<
        "\n% Corresponding Matlab/Octave filter design:"
        "\n...TBD...";
}

#define STRINGIFY(a)  STRINGIFY_(a)
#define STRINGIFY_(a)  #a

#if CONFIG_USE_LOWPASS
# define CHEBY1_PASS   dsp::chebyshev1_lp_hp( dsp::FilterResponse::low_pass, 100, 20, 24, 1, 10 )
# define CHEBY2_PASS   dsp::chebyshev2_lp_hp( dsp::FilterResponse::low_pass, 100, 20, 24, 1, 10 )
#else
# define CHEBY1_PASS   dsp::chebyshev1_lp_hp( dsp::FilterResponse::high_pass, 100, 24, 20, 1, 10 )
# define CHEBY2_PASS   dsp::chebyshev2_lp_hp( dsp::FilterResponse::high_pass, 100, 24, 20, 1, 10 )
#endif

#if CONFIG_USE_BAND_STOP
# define  CHEBY2_BAND  dsp::chebyshev2_bs( 100, 15, 35, 20, 30, 1, 10 )
#else
# define  CHEBY2_BAND  dsp::chebyshev2_bp( 100, 35, 15, 30, 20, 1, 10 )
#endif

void print_ISP()
{
    std::cout <<
        "\n"
        "\n" << line(71) <<
        "\nSophocles J. Orfanidis. Introduction to Signal Processing."
        "\nMatlab functions: http://www.ece.rutgers.edu/~orfanidi/intro2sp/#mfunct"
        "\n";
}

int main()
{
    using core::range;

    const auto Flo   = 0.0;
    const auto Fhi   = 0.5;
    const auto Nsamp = 201;
    const auto Fstep = (Fhi - Flo) / (Nsamp - 1);

    print_pass
    (
        range( Flo, Fhi + Fstep, Fstep ),
        "Bi-quad filter design", STRINGIFY( CHEBY1_PASS ),
        CHEBY1_PASS
    );

    print_pass
    (
        range( Flo, Fhi + Fstep, Fstep ),
        "Bi-quad filter design", STRINGIFY( CHEBY2_PASS ),
        CHEBY2_PASS
    );

    print_band
    (
        range( Flo, Fhi + Fstep, Fstep ),
        "Bi-quad filter design", STRINGIFY( CHEBY2_BAND ),
        CHEBY2_BAND
    );

    print_ISP();
}

// cl -std:c++17 -permissive- -EHsc -O2 -I../../include chebyshev-design.e.cpp && chebyshev-design.e.exe
// g++ -std=c++17 -Wall -O2 -I../../include -o chebyshev-design.e.exe chebyshev-design.e.cpp && chebyshev-design.e.exe
