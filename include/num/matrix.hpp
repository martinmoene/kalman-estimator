//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef NUM_MATRIX_HPP_INCLUDED
#define NUM_MATRIX_HPP_INCLUDED

#define matrix_MAJOR  0
#define matrix_MINOR  0
#define matrix_PATCH  0
#define matrix_VERSION  matrix_STRINGIFY(matrix_MAJOR) "." matrix_STRINGIFY(matrix_MINOR) "." matrix_STRINGIFY(matrix_PATCH)

#define matrix_STRINGIFY(  x )  matrix_STRINGIFY_( x )
#define matrix_STRINGIFY_( x )  #x

#include "std/algorithm.hpp"    // constexpr std20::copy(), std20::fill()
#include "std/utility.hpp"      // std20::swap(), std::initializer_list

namespace num {

// Identity: use to prevent a parameter to participate in template type deduction:

template< typename T >
struct identity { using type = T; };

template< typename T >
using identity_t = typename identity<T>::type;

// matrix, colvec and rowvec:

template< typename T, int M, int N >
class matrix;

template< typename T, int N >
using colvec = matrix<T, N, 1>;

template< typename T, int M >
using rowvec = matrix<T, 1, M>;

// 2d matrix:

template< typename T, int N, int M >
class matrix
{
public:
    // Types:

    using value_type = T;
    using iterator = value_type *;
    using const_iterator = value_type const *;

    // Construction:

#if defined( __AVR ) && __AVR
    constexpr matrix() : storage() {}
#else
    constexpr matrix() = default;
#endif
    constexpr matrix( matrix && ) = default;
    constexpr matrix( matrix const & ) = default;
    constexpr matrix & operator=( matrix && ) = default;
    constexpr matrix & operator=( matrix const & ) = default;

    constexpr matrix( value_type v )
    : storage()
    {
        std20::fill( begin(), end(), v );
    }

    constexpr matrix( std::initializer_list<T> il )
    : storage()
    {
        std20::copy( il.begin(), il.end(), begin() );
    }

    // Observers:

    constexpr int rows() const
    {
        return N;
    }

    constexpr int columns() const
    {
        return M;
    }

    constexpr int size() const
    {
        return rows() * columns();;
    }

    constexpr value_type operator[]( int ndx ) const
    {
        return at( 0, ndx );
    }

    constexpr value_type operator()( int ndx ) const
    {
        return at( 0, ndx );
    }

    constexpr value_type at( int ndx ) const
    {
        return storage[ ndx ];
    }

    constexpr value_type operator()( int row, int col ) const
    {
        return at( row, col );
    }

    constexpr value_type at( int row, int col ) const
    {
        return at( row * M + col );
    }

    // Modifiers:

    constexpr value_type & operator[]( int col )
    {
        return at( 0, col );
    }

    constexpr value_type & operator()( int col )
    {
        return at( 0, col );
    }

    constexpr value_type & operator()( int row, int col )
    {
        return at( row, col );
    }

    constexpr value_type & at( int ndx )
    {
        return storage[ ndx ];
    }

    constexpr value_type & at( int row, int col )
    {
        return at( row * M + col );
    }

    // Iteration:

    constexpr iterator begin()
    {
        return &storage[ 0 ];
    }

    constexpr iterator end()
    {
        return &storage[ N * M ];
    }

    constexpr const_iterator begin() const
    {
        return &storage[ 0 ];
    }

    constexpr const_iterator end() const
    {
        return &storage[ N * M ];
    }

private:
    value_type storage[ N * M ];
};

// ----------------------------------------------
// 1x1 matrix algorithms

// vec_1x1 + v:

template< typename T >
constexpr T operator+( matrix<T,1,1> const & A, identity_t<T> v )
{
    return A(0) + v;
}

// v + vec_1x1:

template< typename T >
constexpr T operator+( identity_t<T> v, matrix<T,1,1> const & A )
{
    return A(0) + v;
}

// vec_1x1 - v:

template< typename T >
constexpr T operator-( matrix<T,1,1> const & A, identity_t<T> v )
{
    return A(0) - v;
}

// v - vec_1x1:

template< typename T >
constexpr T operator-( identity_t<T> v, matrix<T,1,1> const & A )
{
    return v - A(0);
}

// vec_1x1 * v:

template< typename T >
constexpr T operator*( matrix<T,1,1> const & A, identity_t<T> v )
{
    return A(0) * v;
}

// v * vec_1x1:

template< typename T >
constexpr T operator*( identity_t<T> v, matrix<T,1,1> const & A )
{
    return A * v;
}

// A + B1x1:

template< typename T, int N, int M >
constexpr matrix<T,N,M> operator+( matrix<T,N,M> const & A, matrix<T,1,1> const & B )
{
    return A + B(0);
}

// A1x1 + B:

template< typename T, int N, int M >
constexpr matrix<T,N,M> operator+( matrix<T,1,1> const & A, matrix<T,N,M> const & B )
{
    return A(0) + B;
}

// A1x1 * A1x1:
template< typename T >
constexpr matrix<T,1,1> operator*( matrix<T,1,1> const & A, matrix<T,1,1> const & B )
{
    return { A(0) * B(0) };
}

// ----------------------------------------------
// vector algorithms

// rowvec * colvec (dot product):

template< typename T, int N > //, typename std20::enable_if<N != 1>::type >
constexpr T operator*( rowvec<T,N> const & a, colvec<T,N> const & b )
{
    T result = T();

    for( int i = 0; i < a.size(); ++i )
    {
        result += a(i) * b(i);
    }
    return result;
}

// colvec * rowvec:

template< typename T, int N > // , typename std20::enable_if<N != 1>::type >
constexpr matrix<T,N,N> operator*( colvec<T,N> const & a, rowvec<T,N> const & b )
{
    matrix<T,N,N> result(0);

    for( int row = 0; row < a.rows(); ++row )
    {
        for( int col = 0; col < b.columns(); ++col )
        {
            result(row,col) = a(row) * b(col);
        }
    }
    return result;
}

// ----------------------------------------------
// matrix algorithms

// A + v:
template< typename T, int N, int M >
constexpr matrix<T,N,M> operator+( matrix<T,N,M> const & A, identity_t<T> v )
{
    matrix<T,N,M> result(0);

    for( int i = 0; i < A.size(); ++i )
    {
        result(i) = A(i) + v;
    }
    return result;
}

// v + A:

template< typename T, int N, int M >
constexpr matrix<T,N,M> operator+( identity_t<T> v, matrix<T,N,M> const & A )
{
    return A + v;
}

// A - v:

template< typename T, int N, int M >
constexpr matrix<T,N,M> operator-( matrix<T,N,M> const & A, identity_t<T> v )
{
    return A + -v;
}

// A * v:

template< typename T, int N, int M >
constexpr matrix<T,N,M> operator*( matrix<T,N,M> const & A, identity_t<T> v )
{
    matrix<T,N,M> result(0);

    for( int i = 0; i < A.size(); ++i )
    {
        result(i) = A(i) * v;
    }
    return result;
}

// v * A

template< typename T, int N, int M >
constexpr matrix<T,N,M> operator*( identity_t<T> v, matrix<T,N,M> const & A )
{
    return A * v;
}

// A * A1x1

template< typename T, int N, int M>
constexpr matrix<T,N,M> operator*( matrix<T,N,M> const & A, matrix<T,1,1> const & v )
{
    return A * v(0);
}

// A1x1 * A

template< typename T, int N, int M >
constexpr matrix<T,N,M> operator*( matrix<T,1,1> const & v, matrix<T,N,M> const & A )
{
    return A * v(0);
}

// A + A:

template< typename T, int N, int M >
constexpr matrix<T,N,M> operator+( matrix<T,N,M> const & a, matrix<T,N,M> const & b )
{
    matrix<T,N,M> result(0);

    for( int i = 0; i < a.size(); ++i )
    {
        result(i) = a(i) + b(i);
    }
    return result;
}

// A - A:

template< typename T, int N, int M >
constexpr matrix<T,N,M> operator-( matrix<T,N,M> const & a, matrix<T,N,M> const & b )
{
    matrix<T,N,M> result(0);

    for( int i = 0; i < a.size(); ++i )
    {
        result(i) = a(i) - b(i);
    }
    return result;
}

// A * B:

template< typename T, int N, int M >
constexpr matrix<T,N,M> operator*( matrix<T,N,M> const & A, matrix<T,N,M> const & B )
{
    matrix<T,N,M> result(0);

    for ( int row = 0; row < A.rows(); ++row )
    {
        for ( int col = 0; col < A.columns(); ++col )
        {
            for ( int k = 0; k < A.columns(); ++k )
            {
                result(row, col) += A(row, k) * B(k, col);
            }
        }
    }

    return result;
}

// x * A - rowvec:

template< typename T, int N, int M >
constexpr rowvec<T,N> operator*( rowvec<T,N> const & x, matrix<T,N,M> const & A )
{
    rowvec<T,N> result( 0 );
    for ( int row = 0; row < A.rows(); ++row )
    {
        for ( int col = 0; col < A.columns(); ++col )
        {
            result(col) += A(row, col) * x(row);
        }
    }
    return result;
}

// A * x - colvec:

template< typename T, int N, int M >
constexpr colvec<T,N> operator*( matrix<T,N,M> const & A, colvec<T,N> const & x )
{
    colvec<T,N> result( 0 );
    for ( int row = 0; row < A.rows(); ++row )
    {
        for ( int col = 0; col < A.columns(); ++col )
        {
            result(row) += A(row, col) * x(col);
        }
    }
    return result;
}

// ----------------------------------------------
// Transposition algorithms

// transposed(x) - rowvec:

template< typename T, int N >
constexpr rowvec<T,N> transposed( colvec<T,N> const & x )
{
    rowvec<T,N>  result(0);

    std20::copy( x.begin(), x.end(), result.begin() );

    return result;
}

// transposed(x) - colvec:

template< typename T, int N >
constexpr colvec<T,N> transposed( rowvec<T,N> const & x )
{
    colvec<T,N>  result(0);

    std20::copy( x.begin(), x.end(), result.begin() );

    return result;
}

// transposed(A)
//template< typename T, int N >
//matrix<T,N,N> transposed( matrix<T,N,N> const & A )
//{
//    static_assert(false, "Implement transposed(Anxn)");
//}

// transposed(A) - 2x2:

template< typename T >
constexpr matrix<T,2,2> transposed( matrix<T,2,2> const & A )
{
    matrix<T,2,2> result( A );

    using std20::swap; swap( result(0,1), result(1,0) );

    return result;
}

// ----------------------------------------------
// Inversion algorithms

// inverted(v):

template< typename T >
constexpr T inverted( T v )
{
    return v != 0 ? 1 / v : 0;
}

// inverted(A) - 2x2:

template< typename T >
constexpr matrix<T,2,2> inverted( matrix<T,2,2> const & A )
{
    matrix<T,2,2> result(0);

    const auto det = 1 / ( A(0) * A(3) - A(1) * A(2) );

    result(0) = + det * A(3);
    result(1) = - det * A(2);
    result(2) = - det * A(1);
    result(3) = + det * A(0);

    return result;
}

// ----------------------------------------------
// Other

// identity matrix, NxN:

template< typename T, int N >
constexpr matrix<T,N,N> eye()
{
    matrix<T,N,N> result(0);

    for ( int i = 0; i < N; ++i )
    {
        result( i, i ) = 1;
    }
    return result;
}

} // namespace num

#endif // NUM_MATRIX_HPP_INCLUDED
