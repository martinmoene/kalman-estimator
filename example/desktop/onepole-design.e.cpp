// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "dsp/onepole-design.hpp"
#include "dsp/onepole-io.hpp"
#include "core/core.hpp"

using core::Text;
using core::line;
using core::replaced;

template< typename Rng, typename Coeff >
auto print( Rng rng, Text text, Text design, Coeff && coeff )
{
    auto op = dsp::make_onepole( coeff );

    std::cout <<
        "\n"   << line(115, '=') <<
        "\n" << text << " '" << design << "':\n" << op <<
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
        std::cout << abs( response( op, fn ) ) << " ";
    }

    std::cout <<
        "];\n"
        "\nydbV = [";

    for( auto fn : rng )
    {
        std::cout << dsp::dbV( abs( response( op, fn ) ), 1 ) << " ";
    }

    // edit C++ function call 'chebyX_lp_hp' to Matlab call 'lhchebyX':

    const auto matlab_design =
        replaced( replaced( replaced( design,
        "(dsp::onepole_design<FilterResponse::", "" ),
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

// parameters:                                                        f | fs
#define BIQUAD_LPF   dsp::onepole_design<FilterResponse::low_pass  >(  2, 100 )
#define BIQUAD_HPF   dsp::onepole_design<FilterResponse::high_pass >( 48, 100 )

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
}

// cl -std:c++17 -permissive- -EHsc -O2 -I../../include onepole-design.e.cpp && onepole-design.e.exe
// g++ -std=c++17 -Wall -O2 -I../../include -o onepole-design.e.exe onepole-design.e.cpp && onepole-design.e.exe
