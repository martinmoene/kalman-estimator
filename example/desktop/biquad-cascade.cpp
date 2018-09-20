#include "num/biquad-cascade-io.hpp"
#include <algorithm>
#include <vector>

const auto Bqmax = 7;
const auto Nsamp = 100;

using value_type = double;

using BiQuad = num::BiQuadT<value_type>;

template< int N >
using BiQuadCascade = num::BiQuadCascadeT<value_type, N>;

// generate range [first, last] in steps of step:

template< typename T, typename U >
auto range( T first, T last , U cnt )
{
    const auto step = (last - first) / (cnt - 1);

    std::vector<value_type> result( cnt );

    std::generate(
        result.begin(), result.end(),
        [step, n = 0] () mutable { return step * n++; }
    );

    return result;
}

template< int BqMax >
auto print( int Nsamp, char const * const text, char const * const design, BiQuadCascade<BqMax> && bqc )
{
    std::cout <<
        "\n" << text << " " << design << ":\n" << bqc <<
        "\nFilter magnitude response for normalized frequency:\n" <<
        "\nfn = [";

    const auto rng = range( 0.0, 0.4, Nsamp );

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
        "\nplot(fn, y);"
        "\n" <<
        "\n% Filter design:\n" <<
        design <<
        "\nbqc = tf2sos(b,a)\n"
        "\nfreqz(b,a);\n";
}

int main()
{
    print
    (
        Nsamp,
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
        Nsamp,
        "Bi-quad filter design", "[b,a] = cheby2(6,20,0.4)",
        BiQuadCascade<Bqmax>
        (
            BiQuad{   { 0.158682, 0.245885, 0.158682 }, {  0.239123, 0.084944 } }
            , BiQuad{ { 1.000000, 0.054217, 1.000000 }, { -0.264548, 0.416971 } }
            , BiQuad{ { 1.000000,-0.554665, 1.000000 }, { -0.701098, 0.797053 } }
        )
    );
}

// cl -EHsc -O2 -I../../include biquad-cascade.cpp && biquad-cascade.exe
// g++ -Wall -O2 -I../../include -o biquad-cascade.exe biquad-cascade.cpp && biquad-cascade.exe