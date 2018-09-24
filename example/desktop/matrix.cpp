//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "num/matrix.hpp"
#include "num/matrix-io.hpp"
#include <iostream>

int main()
{
    num::colvec<double,2> x = { 1, 2, };
    std::cout <<
        "\nx =\n" << x <<
        "\nx + x =\n" << (x + x) <<
        "\nx * 2 =\n" << (x * 2) <<
        "\n2 * x =\n" << (2 * x) <<
        "\ntransposed(x) =\n" << transposed(x) ;

    num::rowvec<double,2> y = { 1, 2, };
    std::cout <<
        "\ny =\n" << y <<
        "\nx * y =\n" << (x * y) <<
        "\ny * x =\n " << (y * x) << "\n";

    num::matrix<double,2,2> A = { 1, 2,
                           3, 4, };
    std::cout <<
        "\nA =\n" << A <<
        "\nA + A =\n" << ( A + A ) <<
        "\nA * 2 =\n" << ( A * 2 ) <<
        "\n2 * A =\n" << ( 2 * A ) <<
        "\nA * x =\n " << (A * x) <<
        "\ny * A =\n " << (y * A) <<
        "\ntransposed(A) =\n" << transposed(A) <<
        "\ninverted(A) =\n"   << inverted(A) ;

    auto r = A * x;

    std::cout <<
        "\nr =\n" << r ;

    std::cout <<
        "\neye<3,int>() =\n" << num::eye<3,int>() ;
}


// cl -EHsc -O2 -I../include matrix.cpp && matrix.exe
// g++ -Wall -O2 -I../include -o matrix.exe matrix.cpp && matrix.exe
