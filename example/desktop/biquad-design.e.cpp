// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "dsp/biquad-design.hpp"
#include "dsp/biquad-io.hpp"
#include "core/core.hpp"

using core::Text;
using core::line;
using core::replaced;

template< typename Rng, typename Coeff >
auto print( Rng rng, Text text, Text design, Coeff && coeff )
{
    auto bq = dsp::make_biquad( coeff );

    std::cout <<
        "\n"   << line(115, '=') <<
        "\n" << text << " '" << design << "':\n" << bq <<
        "\n% " << line(65) <<
        "\n% Matlab/Octave filter magnitude response for normalized frequency:\n" <<
        "\nfn = [";

    for( auto fn : rng )
    {
        std::cout << fn << " ";
    }

    std::cout <<
        "];\n"
        "\ny = [";

    for( auto fn : rng )
    {
        std::cout << abs( response( bq, fn ) ) << " ";
    }

    std::cout <<
        "];\n"
        "\nydbV = [";

    for( auto fn : rng )
    {
        std::cout << dsp::dbV( abs( response( bq, fn ) ), 1 ) << " ";
    }

    // edit C++ function call 'chebyX_lp_hp' to Matlab call 'lhchebyX':

    const auto matlab_design =
        replaced( replaced( replaced( design,
        "(dsp::biquad_design<FilterResponse::", "" ),
        ">(", "(" ),
        "))", ")" );

    std::cout <<
        "];\n"
        "\nplotyy(fn, y, fn, ydbV);"
        "\n" <<
        "\n% " << line(42) <<
        "\n% Corresponding Matlab/Octave filter design:" <<
        "\n"   <<
        "\npkg load signal\n" <<
        matlab_design <<
        "\nbq = tf2sos(b,a)"
        "\nfreqz(b,a);\n";
}

#define STRINGIFY(a)  STRINGIFY_((a))
#define STRINGIFY_(a)  #a

// parameters:                                                    dbg | f | fs | Q
#define BIQUAD_LPF   dsp::biquad_design<FilterResponse::low_pass  >( 0, 25, 100, 0.8f )
#define BIQUAD_HPF   dsp::biquad_design<FilterResponse::high_pass >( 0, 25, 100, 0.8f )
#define BIQUAD_LPSB  dsp::biquad_design<FilterResponse::low_shelf >(+6, 25, 100, 3 )
#define BIQUAD_LPSC  dsp::biquad_design<FilterResponse::low_shelf >(-6, 25, 100, 3 )
#define BIQUAD_HPSB  dsp::biquad_design<FilterResponse::high_shelf>(+6, 25, 100, 3 )
#define BIQUAD_HPSC  dsp::biquad_design<FilterResponse::high_shelf>(-6, 25, 100, 3 )
#define BIQUAD_BPF   dsp::biquad_design<FilterResponse::band_pass >( 0, 25, 100, 3 )
#define BIQUAD_BSF   dsp::biquad_design<FilterResponse::band_stop >( 0, 25, 100, 3 )
#define BIQUAD_PEQB  dsp::biquad_design<FilterResponse::peak_band_eq>(+6, 25, 100, 3 )
#define BIQUAD_PEQC  dsp::biquad_design<FilterResponse::peak_band_eq>(-6, 25, 100, 3 )

int main()
{
    using core::range;
    using dsp::FilterResponse;

    const auto Flo   = 0.0;
    const auto Fhi   = 0.5;
    const auto Nsamp = 101;
    const auto Fstep = (Fhi - Flo) / (Nsamp - 1);

    // low pass
    print
    (
        range( Flo, Fhi + Fstep, Fstep ),
        "Bi-quad filter design", STRINGIFY( BIQUAD_LPF ),
        BIQUAD_LPF
    );

    // high pass
    print
    (
        range( Flo, Fhi + Fstep, Fstep ),
        "Bi-quad filter design", STRINGIFY( BIQUAD_HPF ),
        BIQUAD_HPF
    );

    // low pass shelve, boost
    print
    (
        range( Flo, Fhi + Fstep, Fstep ),
        "Bi-quad filter design", STRINGIFY( BIQUAD_LPSB ),
        BIQUAD_LPSB
    );

    // low pass shelve, cut
    print
    (
        range( Flo, Fhi + Fstep, Fstep ),
        "Bi-quad filter design", STRINGIFY( BIQUAD_LPSC ),
        BIQUAD_LPSC
    );

    // high pass shelve, boost
    print
    (
        range( Flo, Fhi + Fstep, Fstep ),
        "Bi-quad filter design", STRINGIFY( BIQUAD_HPSB ),
        BIQUAD_HPSB
    );

    // high pass shelve, cut
    print
    (
        range( Flo, Fhi + Fstep, Fstep ),
        "Bi-quad filter design", STRINGIFY( BIQUAD_HPSC ),
        BIQUAD_HPSC
    );

    // band pass, avoid 0, 0.5
    print
    (
        range( Flo + Fstep, Fhi, Fstep ),
        "Bi-quad filter design", STRINGIFY( BIQUAD_BPF ),
        BIQUAD_BPF
    );

    // band stop, avoid 0, 0.5
    print
    (
        range( Flo + Fstep, Fhi, Fstep ),
        "Bi-quad filter design", STRINGIFY( BIQUAD_BSF ),
        BIQUAD_BSF
    );

    // peak equalizer boost:
    print
    (
        range( Flo, Fhi + Fstep, Fstep ),
        "Bi-quad filter design", STRINGIFY( BIQUAD_PEQB ),
        BIQUAD_PEQB
    );

    // peak equalizer cut:
    print
    (
        range( Flo, Fhi + Fstep, Fstep ),
        "Bi-quad filter design", STRINGIFY( BIQUAD_PEQC ),
        BIQUAD_PEQC
    );
}

// cl -std:c++17 -permissive- -EHsc -O2 -I../../include biquad-design.e.cpp && biquad-design.e.exe
// g++ -std=c++17 -Wall -O2 -I../../include -o biquad-design.e.exe biquad-design.e.cpp && biquad-design.e.exe
