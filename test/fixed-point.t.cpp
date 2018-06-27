//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "num/fixed-point.hpp"
#include "num/fixed-point-io.hpp"
#include "lest.hpp"
#include <algorithm>

// Suppress:
// - unused parameter, for cases without assertions such as [.std...]
#if defined __clang__
# pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined __GNUC__
# pragma GCC   diagnostic ignored "-Wunused-parameter"
#endif

#define CASE( name ) lest_CASE( specification(), name )

extern lest::tests & specification();

struct approx
{
    bool operator()( double a, double b )
    {
        return a == lest::approx(b);
    }
};

using namespace num;

template< int X >
struct S
{
    int x[X];
};

S<detail::power2(2)> s;

CASE( "fixed_point: ...." )
{
    std::cout
        << "sizeof(s)" << sizeof(s) << "\n"
        << "power2<0>:" << detail::power2(0) << "\n"
        << "power2<1>:" << detail::power2(1) << "\n"
        << "power2<2>:" << detail::power2(2) << "\n"
        << "power2<3>:" << detail::power2(3) << "\n"
        << "power2<4>:" << detail::power2(4) << "\n"
        ;

    using fp_t = fixed_point<int, 15>;

    fp_t x{ 123 };
    fp_t y{ 0.123 };

    auto z = x + y;

    std::cout
        << "fixed_point<int, 15> x{ 123 };\n"
        << "fixed_point<int, 15> y{ 0.123 };\n"
        << "digits: " << x.digits << "\n"
        << "integer_digits: "       << x.integer_digits << "\n"
        << "fractional_digits: "    << x.fractional_digits << "\n"
        << "x.min(): "              << x.min().as<double>() << "\n"
        << "x.max(): "              << x.max().as<double>() << "\n"
        << "x.underlying_value(): " << x.underlying_value() << "\n"
        << "y.underlying_value(): " << y.underlying_value() << "\n"
        << "z.underlying_value(): " << z.underlying_value() << "\n"
        << "to_rep<fp_t>()(z): "    << to_rep<fp_t>()(z) << "\n"
        << "to_rep<fp_t>()(+z): "   << to_rep<fp_t>()(+z) << "\n"
        << "to_rep<fp_t>()(-z): "   << to_rep<fp_t>()(-z) << "\n"
        << "(-z).as<double>(): "    << (-z).as<double>() << "\n"
        << "(x).as<double>(): "      << (x).as<double>() << "\n"
        << "(y).as<double>(): "      << (y).as<double>() << "\n"
        << "(x+y).as<double>(): "    << (x+y).as<double>() << "\n"
        << "(y+x).as<double>(): "    << (y+x).as<double>() << "\n"
        << "(1*x).as<double>(): "    << (1*x).as<double>() << "\n"
        << "(3*x).as<double>(): "    << (3*x).as<double>() << "\n"
        << "(x*3).as<double>(): "    << (x*3).as<double>() << "\n"
        << "(3*y).as<double>(): "    << (3*y).as<double>() << "\n"
        << "(x*y).as<double>(): "    << (x*y).as<double>() << "\n"
        << "(y*x).as<double>(): "    << (y*x).as<double>() << "\n"
        << "(x/y).as<double>(): "    << (x/y).as<double>() << "\n"
        << "(y/x).as<double>(): "    << (y/x).as<double>() << "\n"
        << "(y/2).as<double>(): "    << (y/2).as<double>() << "\n"
        ;

//    using ufp_t = fixed_point<unsigned int, 15>;
//    ufp_t u{ 1.23 };

}

// -----------------------------------------------------------------------
// Applets

CASE( "fixed_point: printing" " [.app][.print]" )
{
//    std::cout
//        << "rowvec<int,2>({1,2}):\n" << rowvec<int,2>({1,2}) << "\n"
//        << "colvec<int,2>({1,2}):\n" << colvec<int,2>({1,2}) << "\n"
//        << "matrix<int,2,2>({1,2,3,4}):\n" << matrix<int,2,2>({1,2,3,4}) << "\n"
//        ;
}

