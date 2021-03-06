// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "dsp/biquad-cascade-io.hpp"
#include "dsp/filter-design.hpp"
#include "core/range.hpp"
#include "core/text.hpp"
#include <string>
#include <vector>

const auto Bqmax = 7;       // maximum number of bi-quads in cascade
const auto Nsamp = 100;     // print this number of samples

using core::Text;
using core::line;

using Count = int;
using value_type = double;

using BiQuad = dsp::BiQuadT<value_type>;

template< Count N >
using BiQuadCascade = dsp::BiQuadCascadeT<value_type, N>;

template< typename Rng, Count BqMax >
auto print( Rng rng, Text text, Text design, BiQuadCascade<BqMax> && bqc )
{
    std::cout  <<
        "\n"   << line(115, '=') <<
        "\n"   << text << " " << design << ":\n" << bqc <<
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
        std::cout << abs( response( bqc, fn ) ) << " ";
    }

    std::cout <<
        "];\n"
        "\nydbV = [";

    for( auto fn : rng )
    {
        std::cout << dsp::dbV( abs( response( bqc, fn ) ), 1 ) << " ";
    }

    std::cout <<
        "];\n"
        "\nplotyy(fn, y, fn, ydbV);"
        "\n"   <<
        "\n% " << line(42) <<
        "\n% Corresponding Matlab/Octave filter design:" <<
        "\n"   <<
        "\npkg load signal\n" <<
        design <<
        "\nbqc = tf2sos(b,a)"
        "\nfreqz(b,a);\n";
}

int main()
{
    using core::range;

    const auto Flo   = 0.0;
    const auto Fhi   = 0.5;
    const auto Nsamp = 201;
    const auto Fstep = (Fhi - Flo) / (Nsamp - 1);

    print
    (
        range( Flo, Fhi + Fstep, Fstep ),
        "Bi-quad filter design", "[b,a] = cheby1(6,3,0.4)",
        BiQuadCascade<Bqmax>
        (
            BiQuad{   { 0.0025892, 0.0051937, 0.0026045 }, { -0.5913597, 0.8879900 } }
            , BiQuad{ { 1.0000000, 1.9999693, 0.9999809 }, { -0.9156187, 0.6796728 } }
            , BiQuad{ { 1.0000000, 1.9941195, 0.9941310 }, { -1.3316440, 0.5193180 } }
        )
    );

    print
    (
        range( Flo, Fhi + Fstep, Fstep ),
       "Bi-quad filter design", "[b,a] = cheby2(6,20,0.4)",
        BiQuadCascade<Bqmax>
        (
            BiQuad{   { 0.158682, 0.245885, 0.158682 }, {  0.239123, 0.084944 } }
            , BiQuad{ { 1.000000, 0.054217, 1.000000 }, { -0.264548, 0.416971 } }
            , BiQuad{ { 1.000000,-0.554665, 1.000000 }, { -0.701098, 0.797053 } }
        )
    );
}

// cl -std:c++17 -permissive- -EHsc -O2 -I../../include biquad-cascade.e.cpp && biquad-cascade.e.exe
// g++ -std=c++17 -Wall -O2 -I../../include -o biquad-cascade.e.exe biquad-cascade.e.cpp && biquad-cascade.e.exe
