//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Parts are inspired on 'control' by Tom Lankhorst.
// https://github.com/tomlankhorst/control

#ifndef NUM_BIQUAD_HPP_INCLUDED
#define NUM_BIQUAD_HPP_INCLUDED

#include "std/complex.hpp"
#include "std/utility.hpp"  // std20::swap()

#if defined( __AVR ) && __AVR
# define assert( expr )  /*empty*/
#else
# include <cassert>
#endif

namespace num { namespace biquad {

// Identity: use to prevent a parameter to participate in template type deduction:

template< typename T >
struct identity { using type = T; };

template< typename T >
using identity_t = typename identity<T>::type;

// a-coefficients:

template< typename V >
struct AT
{
    V a1 = V{0}, a2 = V{0};
//    V a0 = V{1}, a1 = V{0}, a2 = V{0};
};

// b-coefficients:

template< typename V >
struct BT
{
    V b0 = V{0}, b1 = V{0}, b2 = V{0};
};

// biquad state:

template< typename V >
struct WT
{
    V v1 = V{0}, v2 = V{0};
};

// result for to_coeff():

template< typename V >
struct BAT : BT<V>, AT<V>
{};

// solution:

template< typename V >
struct SolutionT
{
    using Complex = std20::complex<V>;
    Complex s0, s1;
};

// normalized a:

template< typename V >
auto operator/( AT<V> a, identity_t<V> x )
{
    return AT<V>{ a.a1 / x, a.a2 / x, a.a0 / x };
}

// normalized b:

template< typename V >
auto operator/( BT<V> b, identity_t<V> x )
{
    return BT<V>{ b.b0 / x, b.b1 / x, b.b2 / x };
}

// enable assignment of result of to_coeff() to member data:

template< typename T >
struct Tie
{
    Tie( T & a, T & b, T & c, T & d, T & e )
    : b0(a), b1(b), b2(c), a1(d), a2(e)
    {}

    void operator=( BAT<T> const & bat )
    {
        b0 = bat.b0; b1 = bat.b1; b2 = bat.b2;
                     a1 = bat.a1; a2 = bat.a2;
    }

    T & b0; T & b1; T & b2; T & a1; T & a2;
};

template< typename T >
auto tie( T & a, T & b, T & c, T & d, T & e )
{
    return Tie<T>( a, b, c, d, e );
}

//
// A bi-quad filter section with arithmetic type T, coefficient type S (T)
//
// In z-domain, ratio of two quadratic functions, in general form:
//
//            b0 + b1 z^-1 + b2 z^-2
//    H(z) = ----------------------
//            a0 + a1 z^-1 + a2 z^-2
//
// Normalized by dividing all coefficients by a0.
//
template< typename T, typename S = /*const*/ T >
class BiQuadT
{
public:
    //  Types:
    using value_type = T;
    using coeff_type = S;

    using A = AT<coeff_type>;
    using B = BT<coeff_type>;
    using W = WT<value_type>;

    using  Solution = SolutionT<value_type>;
    using  Complex  = typename Solution::Complex;

    // Construction:

    constexpr BiQuadT() = default;

    constexpr BiQuadT
    (
        B b_
    )
        : b( b_)
    {}

    // Normalized coefficients:

    constexpr BiQuadT
    (
        B b_
        , A a_
    )
#if 1
        : a( a_)
        , b( b_)
#else
        : a( a_ / a_.a0 )
        , b( b_ / a_.a0 )
#endif
    {}

    // Initialize biquad from ZPK:

    BiQuadT( Solution z, Solution p, value_type k )
    {
        tie( b.b0, b.b1, b.b2, a.a1, a.a2 ) = to_coeff( z, p, k );
    }

    // Observers:

    auto coeff_a() const
    {
        return a;
    }

    auto coeff_b() const
    {
        return b;
    }

    auto state() const
    {
        return w;
    }

    auto poles() const
    {
        return solve( value_type{1}, a.a1, a.a2 );
    }

    auto zeros() const
    {
        return solve( b.b0, b.b1, b.b2 );
    }

    // stable if the magnitude of both poles is less or equal to leq unity:

    auto is_stable() const
    {
          const auto p = poles();

          return abs( p.s0 ) <= value_type{1}
              && abs( p.s1 ) <= value_type{1};
    }

    // Modifiers:

    void reset()
    {
        w = W{};
    }

    auto step( value_type x )
    {
        return step_df2( x );
    }

    // step, direct-form 2:

    auto step_df2( value_type x )
    {
        // Direct form II
        const auto w0 = x - a.a1 * w.v1 - a.a2 * w.v2;
        const auto y  = b.b0 * w0 + b.b1 * w.v1 + b.b2 * w.v2;

        w.v2 = w.v1;
        w.v1 = w0;

        return y;
    }

    // step, direct-form 2, transposed:

    auto step_df2_transposed( value_type x )
    {
        // Direct form II transposed
        const auto y = b.b0 * x + w.v1;

        w.v1 = b.b1 * x - a.a1 * y + w.v2;
        w.v2 = b.b2 * x - a.a2 * y;

        return y;
    }

    // Swap:

    void swap( BiQuadT & other )
    {
        using std20::swap;

        swap( a, other.a );
        swap( b, other.b );
        swap( w, other.w );
    }

private:
    template< typename U >
    struct Pair{ U first; U second; };

    // Convert zpk to a and b coefficients :

    auto to_coeff( Solution z, Solution p, value_type k )
    {
        auto b = to_coeff( z );
        auto a = to_coeff( p );

        return BAT<value_type>
        {
            k, k * b.first, k * b.second
            ,      a.first,     a.second
        };
    }

    // Coefficients of quadratic poly. corresponding to zeros
    //
    // Gets the coefficients of the quadratic polynomial s.t. its solutions are p
    //
    // (x - z1)(x - z2) -> x^2 - (z1 + z2) + z1 * z2

    auto to_coeff( Solution z )
    {
        auto z1 = z.s0;
        auto z2 = z.s1;

        auto z1r = z1.real();
        auto z1i = z1.imag();
        auto z2r = z2.real();

        // Assume this is a complex conjugate pair if p1 has imag part
        return z1i
            ? Pair<value_type>{  -2 * z1r, z1r * z1r + z1i * z1i }
            : Pair<value_type>{ -z1r -z2r, z1r * z2r             };
    }

    // Solve a quadratic polynomial: ax^2 + bx + c -> k(x - z1)(x - z2)

    auto solve( value_type a, value_type b, value_type c ) const
    {
        // normalize:
        b /= a;
        c /= a;

        // sqrt(b^2 - 4 * a * c)

        const auto ds = sqrt( Complex{ b * b, 0 } - 4 * c );

        // (-b +/- ds) / 2a

        const auto vt2 = value_type{2};

        return Solution{ (-b + ds) / vt2, (-b - ds) / vt2 };
    }

private:
    // Filter coefficients:
    A a;
    B b;

    // Bi-quad state:
    W w;
};

// response at normalized frequency (f/fs):

template< typename T, typename S >
auto response( BiQuadT<T,S> const & bq, identity_t<T> fnorm )
{
    using Complex = typename BiQuadT<T,S>::Complex;

    const auto Pi = T( 3.1415926535897932384626433832795028841971 );

    const auto a = bq.coeff_a();
    const auto b = bq.coeff_b();

    const auto w = 2 * Pi * fnorm;
    const auto czn1 = std20::polar(T{1}, -w     );
    const auto czn2 = std20::polar(T{1}, -2 * w );

    return ( Complex{ b.b0 } + b.b1 * czn1 + b.b2 * czn2 )
         / ( Complex{ 1    } + a.a1 * czn1 + a.a2 * czn2 );
}

} // namespace biquad

using biquad::BiQuadT;

} // namespace num

#endif // NUM_BIQUAD_HPP_INCLUDED
