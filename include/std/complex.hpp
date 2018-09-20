// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef STD_COMPLEX_INCLUDED
#define STD_COMPLEX_INCLUDED

#if !( defined( __AVR ) && __AVR )

#include <complex>

namespace std20 {

using std::complex;
using std::polar;

} // namespace std20

#else // __AVR

namespace std20 {

template< typename T >
class complex
{
public:
    using value_type = T;

    constexpr complex( T const & re = T(), T const & im = T() )
    : re_( re )
    , im_( im )
    {}

    constexpr complex( complex const & other ) = default;

    template< class X >
    constexpr complex( complex<X> const & other)
    : re_( other.real() )
    , im_( other.imag() )
    {}

    constexpr complex & operator=( T const & x )
    {
        re_ = x;
        im_ = T();
    }

    constexpr complex & operator=( complex const & cx )
    {
        re_ = cx.real();
        im_ = cx.imag();
    }

    template<class X>
    constexpr complex & operator=( complex<X> const & cx)
    {
        re_ = cx.real();
        im_ = cx.imag();
    }

    constexpr T real() const
    {
        return re_;
    }

    constexpr T imag() const
    {
        return im_;
    }

    constexpr void real( T value )
    {
        re_ = value;
    }

    constexpr void imag( T value )
    {
        im_ = value;
    }

    constexpr complex & operator+=( T const & other );
    constexpr complex & operator-=( T const & other );
    constexpr complex & operator*=( T const & other );
    constexpr complex & operator/=( T const & other );

    template<class X>
    constexpr complex& operator+=( complex<X> const & other);

    template<class X>
    constexpr complex& operator-=( complex<X> const & other);

    template<class X>
    constexpr complex& operator*=( complex<X> const & other);

    template<class X>
    constexpr complex& operator/=( complex<X> const & other);

private:
    value_type re_;
    value_type im_;
};

// unary:

template< class T >
constexpr complex<T> operator+( T const & val );

template< class T >
constexpr complex<T> operator-( complex<T> const & val );

// binary:

template< class T >
constexpr complex<T> operator+( complex<T> const & lhs, T const & rhs)
{
    return complex(lhs) += rhs;
}

template< class T >
constexpr complex<T> operator-( complex<T> const & lhs, T const & rhs)
{
    return complex(lhs) -= rhs;
}

template< class T >
constexpr complex<T> operator*( complex<T> const & lhs, T const & rhs)
{
    return complex(lhs) *= rhs;
}

template< class T >
constexpr complex<T> operator/( complex<T> const & lhs, T const & rhs)
{
    return complex(lhs) /= rhs;
}

template< class T >
constexpr complex<T> operator+( T const & lhs, complex<T> const & rhs )
{
    return complex(lhs) += rhs;
}

template< class T >
constexpr complex<T> operator-( T const & lhs, complex<T> const & rhs )
{
    return complex(lhs) -= rhs;
}

template< class T >
constexpr complex<T> operator*( T const & lhs, complex<T> const & rhs )
{
    return complex(lhs) *= rhs;
}

template< class T >
constexpr complex<T> operator/( T const & lhs, complex<T> const & rhs )
{
    return complex(lhs) /= rhs;
}

template< class T >
constexpr complex<T> operator+( complex<T> const & lhs, complex<T> const & rhs)
{
    return complex(lhs) += rhs;
}

template< class T >
constexpr complex<T> operator-( complex<T> const & lhs, complex<T> const & rhs)
{
    return complex(lhs) -= rhs;
}

template< class T >
constexpr complex<T> operator*( complex<T> const & lhs, complex<T> const & rhs)
{
    return complex(lhs) *= rhs;
}

template< class T >
constexpr complex<T> operator/( complex<T> const & lhs, complex<T> const & rhs)
{
    return complex(lhs) /= rhs;
}

template< class T >
T abs( complex<T> const & z );

template< class T >
complex<T> sqrt( complex<T> const & z );

} // naespace std20

#endif  // __AVR

#endif // STD_UTILITY_INCLUDED
