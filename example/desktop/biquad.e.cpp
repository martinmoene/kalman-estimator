// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "dsp/biquad-io.hpp"
#include "dsp/filter-design.hpp"
#include "core/core.hpp"
#include <algorithm>
#include <string>
#include <vector>

using core::Text;
using core::line;

using value_type = double;

using BiQuad = dsp::BiQuadT<value_type>;

template< typename Rng >
auto print( Rng rng, Text text, Text design, BiQuad && bq )
{
    std::cout <<
        "\n"   << line(115, '=') <<
        "\n" << text << " " << design << ":\n" << bq <<
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

    std::cout <<
        "];\n"
        "\nplotyy(fn, y, fn, ydbV);"
        "\n" <<
        "\n% " << line(42) <<
        "\n% Corresponding Matlab/Octave filter design:" <<
        "\n"   <<
        "\npkg load signal\n" <<
        design <<
        "\nbq = tf2sos(b,a)"
        "\nfreqz(b,a);\n";
}

int main()
{
    using core::range;

    const auto Flo   = 0.0;
    const auto Fhi   = 0.5;
    const auto Nsamp = 201;
    const auto Fstep = (Fhi - Flo) / (Nsamp - 1);

    print( range( Flo, Fhi + Fstep, Fstep ), "Bi-quad filter design", "[b,a] = cheby1(2,3,0.4)" , BiQuad{ { 0.14361 , 0.28721 , 0.14361  }, { -0.67993, 0.49133 } } );
    print( range( Flo, Fhi + Fstep, Fstep ), "Bi-quad filter design", "[b,a] = cheby2(2,10,0.4)", BiQuad{ { 0.323496, 0.017539, 0.323496 }, { -0.66299, 0.32752 } } );
}

// cl -std:c++17 -permissive- -EHsc -O2 -I../../include biquad.e.cpp && biquad.e.exe
// g++ -std=c++17 -Wall -O2 -I../../include -o biquad.e.exe biquad.e.cpp && biquad.e.exe
