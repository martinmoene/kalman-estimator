#include "num/biquad-io.hpp"
#include <algorithm>
#include <string>
#include <vector>

const auto Nsamp = 100;

using Count = int;
using Text  = std::string;

using value_type = double;

using BiQuad = num::BiQuadT<value_type>;

// generate range [first, last] in cnt steps:

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

template< typename Rng >
auto print( Rng rng, Text text, Text design, BiQuad && bq )
{
    std::cout <<
        "\n" << text << " " << design << ":\n" << bq <<
        "\nFilter magnitude response for normalized frequency:\n" <<
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
        "\nplot(fn, y);\n"
        "\n" <<
        "\n% Filter design:\n" <<
        design <<
        "\nbq = tf2sos(b,a)\n"
        "\nfreqz(b,a);\n";
}

int main()
{
    print( range( 0.0, 0.5, Nsamp ), "Bi-quad filter design", "[b,a] = cheby1(2,3,0.4)" , BiQuad{ { 0.14361 , 0.28721 , 0.14361  }, { -0.67993, 0.49133 } } );
    print( range( 0.0, 0.5, Nsamp ), "Bi-quad filter design", "[b,a] = cheby2(2,10,0.4)", BiQuad{ { 0.323496, 0.017539, 0.323496 }, { -0.66299, 0.32752 } } );
}

// cl -EHsc -std:c++17 -O2 -I../../include biquad.cpp && biquad.exe
// g++ -Wall -std=c++17 -O2 -I../../include -o biquad.exe biquad.cpp && biquad.exe
