// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "dsp/onepole-io.hpp"
#include "dsp/filter-design.hpp"
#include "core/core.hpp"
#include <algorithm>
#include <string>
#include <vector>

using core::Text;
using core::line;

using value_type = double;

using OnePole = dsp::OnePoleT<value_type>;

template< typename Rng >
auto print( Rng rng, Text text, Text design, OnePole && op )
{
    std::cout <<
        "\n"   << line(115, '=') <<
        "\n" << text << " " << design << ":\n" << op <<
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

    print( range( Flo, Fhi + Fstep, Fstep ), "One-pole filter design", "OnePole{ { 0.05 }, { -0.95 } }" , OnePole{ { 0.05 }, { -0.95 } } );
    print( range( Flo, Fhi + Fstep, Fstep ), "One-pole filter design", "OnePole{ { 0.05 }, { +0.95 } } ", OnePole{ { 0.05 }, { +0.95 } } );
}

// cl -std:c++17 -permissive- -EHsc -O2 -I../../include onepole.e.cpp && onepole.e.exe
// g++ -std=c++17 -Wall -O2 -I../../include -o onepole.e.exe onepole.e.cpp && onepole.e.exe
