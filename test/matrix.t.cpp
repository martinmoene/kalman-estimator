// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "num/matrix.hpp"
#include "num/matrix-io.hpp"
#include "lest.hpp"
#include <algorithm>

// Configuration:

#ifndef  KE_USE_STATIC_EXPECT
# define KE_USE_STATIC_EXPECT  0
#endif

// Suppress:
// - unused parameter, for cases without assertions such as [.std...]
#if defined __clang__
# pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined __GNUC__
# pragma GCC   diagnostic ignored "-Wunused-parameter"
#endif

#if defined( KE_USE_STATIC_EXPECT ) && KE_USE_STATIC_EXPECT
# define STATIC_EXPECT(     expr )  static_assert(   expr  )
# define STATIC_EXPECT_NOT( expr )  static_assert( !(expr) )
#else
# define STATIC_EXPECT(     expr )  EXPECT(     expr )
# define STATIC_EXPECT_NOT( expr )  EXPECT_NOT( expr )
#endif

#define CASE( name ) lest_CASE( specification(), name )

extern lest::tests & specification();

// approx function object, simplified floating point comparison:

struct approx
{
    constexpr auto abs( double a )
    {
        return a < 0.0 ? -a : +a;
    }

    constexpr bool operator()( double a, double b )
    {
        return abs(a - b) < 0.0001;
    }
};

using namespace num;

CASE( "rowvec: Allows to default construct" )
{
    rowvec<int,3> x;
    EXPECT( x.size() == 3 );
}

CASE( "colvec: Allows to default construct" )
{
    colvec<int,3> x;
    EXPECT( x.size() == 3 );
}

CASE( "matrix: Allows to default construct" )
{
    matrix<int,3,3> A;
    EXPECT( A.size() == 3*3 );
}

CASE( "rowvec: Allows to construct from a single value" )
{
    constexpr rowvec<int,2> x(7);

    STATIC_EXPECT( x[0] == 7 );
    STATIC_EXPECT( x[1] == 7 );
}

CASE( "colvec: Allows to construct from a single value" )
{
    constexpr rowvec<int,2> x(7);
    STATIC_EXPECT( x[0] == 7 );
    STATIC_EXPECT( x[1] == 7 );
}

CASE( "matrix: Allows to construct from a single value" )
{
    constexpr matrix<int,2,2> A(7);
    STATIC_EXPECT( A[0] == 7 );
    STATIC_EXPECT( A[1] == 7 );
    STATIC_EXPECT( A[2] == 7 );
    STATIC_EXPECT( A[3] == 7 );
}

CASE( "rowvec: Allows to construct from an initializer-list" )
{
    constexpr rowvec<int,2> x = { 1, 2 };
    STATIC_EXPECT( x[0] == 1 );
    STATIC_EXPECT( x[1] == 2 );
}

CASE( "colvec: Allows to construct from an initializer-list" )
{
    constexpr colvec<int,2> x = { 1, 2 };
    STATIC_EXPECT( x[0] == 1 );
    STATIC_EXPECT( x[1] == 2 );
}

CASE( "matrix: Allows to construct from an initializer-list" )
{
    constexpr matrix<int,2,2> A = { 1, 2, 3, 4 };
    STATIC_EXPECT( A[0] == 1 );
    STATIC_EXPECT( A[1] == 2 );
    STATIC_EXPECT( A[2] == 3 );
    STATIC_EXPECT( A[3] == 4 );
}

CASE( "rowvec: Allows to copy-construct from another rowvec of the same type" )
{
    constexpr rowvec<int,2> x = rowvec<int,2>({ 1, 2 });
    STATIC_EXPECT( x[0] == 1 );
    STATIC_EXPECT( x[1] == 2 );
}

CASE( "colvec: Allows to copy-construct from another colvec of the same type" )
{
    constexpr colvec<int,2> x = colvec<int,2>({ 1, 2 });
    STATIC_EXPECT( x[0] == 1 );
    STATIC_EXPECT( x[1] == 2 );
}

CASE( "matrix: Allows to copy-construct from another matrix of the same type" )
{
    constexpr matrix<int,2,2> A = matrix<int,2,2>({ 1, 2, 3, 4 });
    STATIC_EXPECT( A[0] == 1 );
    STATIC_EXPECT( A[1] == 2 );
    STATIC_EXPECT( A[2] == 3 );
    STATIC_EXPECT( A[3] == 4 );
}

CASE( "rowvec: Allows to copy-assign from another rowvec of the same type" )
{
    rowvec<int,2> x;
    rowvec<int,2> y = { 1, 2 };

    x = y;

    EXPECT( x[0] == 1 );
    EXPECT( x[1] == 2 );
}

CASE( "colvec: Allows to copy-assign from another colvec of the same type" )
{
    colvec<int,2> x;
    colvec<int,2> y = { 1, 2 };

    x = y;

    EXPECT( x[0] == 1 );
    EXPECT( x[1] == 2 );
}

CASE( "matrix: Allows to copy-assign from another matrix of the same type" )
{
    matrix<int,2,2> A;
    matrix<int,2,2> B = { 1, 2, 3, 4 };

    A = B;

    EXPECT( A[0] == 1 );
    EXPECT( A[1] == 2 );
    EXPECT( A[2] == 3 );
    EXPECT( A[3] == 4 );
}

CASE( "matrix: Allows to observe the number of rows" )
{
    matrix<int,2,3> A;
    EXPECT( A.rows() == 2 );
}

CASE( "matrix: Allows to observe the number of columns" )
{
    matrix<int,2,3> A;
    EXPECT( A.columns() == 3 );
}

CASE( "matrix: Allows to observe the number of elements" )
{
    matrix<int,2,3> A;
    EXPECT( A.size() == 2*3 );
}

CASE( "matrix: Allows to observe an element via array indexing (single dimension)" )
{
    constexpr matrix<int,2,2> A = { 1, 2, 3, 4 };
    EXPECT( A[0] == 1 );
    EXPECT( A[1] == 2 );
    EXPECT( A[2] == 3 );
    EXPECT( A[3] == 4 );
}

CASE( "matrix: Allows to observe an element via call indexing (single dimension)" )
{
    constexpr matrix<int,2,2> A = { 1, 2, 3, 4 };
    EXPECT( A(0) == 1 );
    EXPECT( A(1) == 2 );
    EXPECT( A(2) == 3 );
    EXPECT( A(3) == 4 );
}

CASE( "matrix: Allows to observe an element via call indexing (dual dimension)" )
{
    constexpr matrix<int,2,2> A = { 1, 2, 3, 4 };
    EXPECT( A(0,0) == 1 );
    EXPECT( A(0,1) == 2 );
    EXPECT( A(1,0) == 3 );
    EXPECT( A(1,1) == 4 );
}

CASE( "matrix: Allows to observe an element via at() (single dimension)" )
{
    constexpr matrix<int,2,2> A = { 1, 2, 3, 4 };
    EXPECT( A.at(0) == 1 );
    EXPECT( A.at(1) == 2 );
    EXPECT( A.at(2) == 3 );
    EXPECT( A.at(3) == 4 );
}

CASE( "matrix: Allows to observe an element via at() (dual dimension)" )
{
    constexpr matrix<int,2,2> A = { 1, 2, 3, 4 };
    EXPECT( A.at(0,0) == 1 );
    EXPECT( A.at(0,1) == 2 );
    EXPECT( A.at(1,0) == 3 );
    EXPECT( A.at(1,1) == 4 );
}

CASE( "matrix: Allows to change an element via array indexing (single dimension)" )
{
    matrix<int,2,2> A = { 1, 2, 3, 4 };

    A[2] = 7;

    EXPECT( A[2] == 7 );
}

CASE( "matrix: Allows to change an element via call indexing (single dimension)" )
{
    matrix<int,2,2> A = { 1, 2, 3, 4 };

    A(2) = 7;

    EXPECT( A(2) == 7 );
}

CASE( "matrix: Allows to change an element via call indexing (dual dimension)" )
{
    matrix<int,2,2> A = { 1, 2, 3, 4 };

    A(1,0) = 7;

    EXPECT( A(1,0) == 7 );
}

CASE( "matrix: Allows to change an element via at() (single dimension)" )
{
    matrix<int,2,2> A = { 1, 2, 3, 4 };

    A.at(2) = 7;

    EXPECT( A.at(2) == 7 );
}

CASE( "matrix: Allows to change an element via at() (dual dimension)" )
{
    matrix<int,2,2> A = { 1, 2, 3, 4 };

    A.at(1,0) = 7;

    EXPECT( A.at(1,0) == 7 );
}

CASE( "matrix: Allows forward iteration" )
{
    matrix<int,2,2> A = { 1, 2, 3, 4 };

    std::fill( A.begin(), A.end(), 7 );

    EXPECT( A.at(0,0) == 7 );
    EXPECT( A.at(0,1) == 7 );
    EXPECT( A.at(1,0) == 7 );
    EXPECT( A.at(1,1) == 7 );
}

CASE( "matrix: Allows const forward iteration" )
{
    constexpr matrix<int,2,2> A = { 1, 2, 3, 4 };

    STATIC_EXPECT( std20::equal( A.begin(), A.end(), A.begin() ) );
}

CASE( "algorithm:        [x1]  + value       " " [vec][1x1][val][add]" )
{
    constexpr auto r = rowvec<int,1>({2}) + 7;
    constexpr auto c = colvec<int,1>({3}) + 7;

    STATIC_EXPECT( r == 2+7 );
    STATIC_EXPECT( c == 3+7 );
}

CASE( "algorithm:       value  + [x1]        " " [vec][1x1][val][add]" )
{
    constexpr auto r = 7 + rowvec<int,1>({2});
    constexpr auto c = 7 + colvec<int,1>({3});

    STATIC_EXPECT( r == 7+2 );
    STATIC_EXPECT( c == 7+3 );
}

CASE( "algorithm:        [x1]  - value       " " [vec][1x1][val][sub]" )
{
    constexpr auto r = rowvec<int,1>({2}) - 7;
    constexpr auto c = colvec<int,1>({3}) - 7;

    STATIC_EXPECT( r == 2-7 );
    STATIC_EXPECT( c == 3-7 );
}

CASE( "algorithm:       value  - [x1]        " " [vec][1x1][val][sub]" )
{
    constexpr auto r = 7 - rowvec<int,1>({2});
    constexpr auto c = 7 - colvec<int,1>({3});

    STATIC_EXPECT( r == 7-2 );
    STATIC_EXPECT( c == 7-3 );
}

CASE( "algorithm:        [x1]  . value       " " [vec][1x1][val][mul]" )
{
    constexpr auto r = rowvec<int,1>({2}) * 3;
    constexpr auto c = colvec<int,1>({3}) * 3;

    STATIC_EXPECT( r == 2*3 );
    STATIC_EXPECT( c == 3*3 );
}

CASE( "algorithm:       value  . [x1]        " " [vec][1x1][val][mul]" )
{
    constexpr auto r = 3 * rowvec<int,1>({2});
    constexpr auto c = 3 * colvec<int,1>({3});

    EXPECT( r == 2*3 );
    EXPECT( c == 3*3 );
}

CASE( "algorithm: [x1 ... xm]  . value       " " [row][vec][val][mul]" )
{
    constexpr auto r = rowvec<int,2>({1,2}) * 3;
    EXPECT( r[0] == 1*3 );
    EXPECT( r[1] == 2*3 );
}

CASE( "algorithm:       value  . [x1 ... xm] " " [row][vec][val][mul]")
{
    constexpr auto r = 3 * rowvec<int,2>({1,2});
    STATIC_EXPECT( r[0] == 1*3 );
    STATIC_EXPECT( r[1] == 2*3 );
}

CASE( "algorithm: [x1 ... xm]T . value       " " [col][vec][val][mul]")
{
    constexpr auto r = colvec<int,2>({1,2}) * 3;
    STATIC_EXPECT( r[0] == 1*3 );
    STATIC_EXPECT( r[1] == 2*3 );
}

CASE( "algorithm:       value  . [x1 ... xm]T" " [col][vec][val][mul]")
{
    constexpr auto r = 3 * colvec<int,2>({1,2});
    EXPECT( r[0] == 1*3 );
    EXPECT( r[1] == 2*3 );
}

CASE( "algorithm: [x1 ... xm]  . [x1 ... xm]T" " [row][col][vec][mul][dot]" )
{
    constexpr auto v = rowvec<int,2>({1,2}) * colvec<int,2>({1,2});
    STATIC_EXPECT( v == 5 );
}

CASE( "algorithm: [x1 ... xm]T . [x1 ... xm] " " [col][row][vec][mul][cross]" )
{
    constexpr colvec<int,2>   x = { 1, 2 };
    constexpr rowvec<int,2>   y = { 1, 2 };
    constexpr matrix<int,2,2> R = { 1, 2, 2, 4 };

    constexpr auto A = x * y;

    STATIC_EXPECT( std20::equal( A.begin(), A.end(), R.begin() ) );
}

CASE( "algorithm:     [a ; b]  + value       " " [mat][val][add]" )
{
    matrix<int,2,2> A = { 1  , 2  , 3  , 4   };
    matrix<int,2,2> B = { 1+7, 2+7, 3+7, 4+7 };

    A = A + 7;

    EXPECT( std20::equal( A.begin(), A.end(), B.begin() ) );
}

CASE( "algorithm:       value  + [a ; b]     " " [mat][val][add]" )
{
    matrix<int,2,2> A = { 1  , 2  , 3  , 4   };
    matrix<int,2,2> B = { 1+7, 2+7, 3+7, 4+7 };

    A = 7 + A;

    EXPECT( std20::equal( A.begin(), A.end(), B.begin() ) );
}

CASE( "algorithm:     [a ; b]  . value       " " [mat][val][mul]" )
{
    matrix<int,2,2> A = { 1  , 2  , 3  , 4   };
    matrix<int,2,2> B = { 1*7, 2*7, 3*7, 4*7 };

    A = A * 7;

    EXPECT( std20::equal( A.begin(), A.end(), B.begin() ) );
}

CASE( "algorithm:       value  . [a ; b]     " " [mat][val][mul]" )
{
    matrix<int,2,2> A = { 1  , 2  , 3  , 4   };
    matrix<int,2,2> B = { 1*7, 2*7, 3*7, 4*7 };

    A = 7 * A;

    EXPECT( std20::equal( A.begin(), A.end(), B.begin() ) );
}

CASE( "algorithm:     [a ; b]  + [c ; d]     " " [mat][add]" )
{
    matrix<int,2,2> A = { 1  , 2  , 3  , 4   };
    matrix<int,2,2> B = { 1+1, 2+2, 3+3, 4+4 };

    A = A + A;

    EXPECT( std20::equal( A.begin(), A.end(), B.begin() ) );
}

CASE( "algorithm:     [a ; b]  - [c ; d]     " " [mat][sub]" )
{
    matrix<int,2,2> A = { 1  , 2  , 3  , 4   };
    matrix<int,2,2> B(0);

    A = A - A;

    EXPECT( std20::equal( A.begin(), A.end(), B.begin() ) );
}

CASE( "algorithm:         [a]  . [b]         " " [mat][1x1][mul]" )
{
    matrix<int,1,1> v = { 2 };
    matrix<int,2,2> A = { 1, 2, 3, 4 };
    matrix<int,2,2> R = { 2*1, 2*2, 2*3, 2*4 };
    matrix<int,2,2> B, C;

    B = v * A;
    C = A * v;

    EXPECT( std20::equal( B.begin(), B.end(), R.begin() ) );
    EXPECT( std20::equal( C.begin(), C.end(), R.begin() ) );
}

CASE( "algorithm:     [a ; b]  . [c ; d]     " " [mat][mul]" )
{
    matrix<int,2,2> A = { 1,  2,  3,  4 };
    matrix<int,2,2> B = { 7, 10, 15, 22 };

    A = A * A;

    EXPECT( std20::equal( A.begin(), A.end(), B.begin() ) );
}

CASE( "algorithm: [x1 ... xm]  . [a ; b]     " " [row][vec][mat][mul]" )
{
    constexpr rowvec<int,2>   r = { 7, 10 };
    constexpr rowvec<int,2>   x = { 1,  2 };
    constexpr matrix<int,2,2> A = { 1,  2,  3,  4 };

#ifdef _MSC_VER
    auto y = x * A;

    EXPECT( std20::equal( y.begin(), y.end(), r.begin() ) );
#else
    constexpr auto y = x * A;

    STATIC_EXPECT( std20::equal( y.begin(), y.end(), r.begin() ) );
#endif
}

CASE( "algorithm:     [a ; b]  . [x1 ... xm]T" " [mat][col][vec][mul]" )
{
    constexpr colvec<int,2>   r = { 5, 11 };
    constexpr colvec<int,2>   x = { 1,  2 };
    constexpr matrix<int,2,2> A = { 1,  2,  3,  4 };

#ifdef _MSC_VER
    auto y = A * x;

    EXPECT( std20::equal( y.begin(), y.end(), r.begin() ) );
#else
    constexpr auto y = A * x;

    STATIC_EXPECT( std20::equal( y.begin(), y.end(), r.begin() ) );
#endif
}

CASE( "algorithm: [x1 ... xm]T               " " [row][vec][transposed]" )
{
    constexpr rowvec<int,2> x = { 1, 2 };
    constexpr colvec<int,2> r = { 1, 2 };

    constexpr auto y = transposed(x);

    STATIC_EXPECT( std20::equal( y.begin(), y.end(), r.begin() ) );
}

CASE( "algorithm: [x1 ... xm]TT              " " [col][vec][transposed]" )
{
    constexpr colvec<int,2> x = { 1, 2 };
    constexpr rowvec<int,2> r = { 1, 2 };

    constexpr auto y = transposed(x);

    STATIC_EXPECT( std20::equal( y.begin(), y.end(), r.begin() ) );
}

CASE( "algorithm: [a ; b]T                   " " [mat2x2][transposed]" )
{
    constexpr matrix<int,2,2> A = { 1, 2, 3, 4 };
    constexpr matrix<int,2,2> R = { 1, 3, 2, 4 };

    constexpr auto AT = transposed(A);

    STATIC_EXPECT( std20::equal( AT.begin(), AT.end(), R.begin() ) );
}

CASE( "algorithm: [a ; ...]T                 " " [matNxN][transposed]" )
{
#if 0
    constexpr matrix<int,3,3> A = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr matrix<int,3,3> R = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    constexpr auto AT = transposed(A);

    STATIC_EXPECT( std20::equal( AT.begin(), AT.end(), R.begin() ) );
#else
    EXPECT( !!"Implement transposed(Anxn)");
#endif
}

CASE( "algorithm: inverted( value        )   " " [val][inverted]" )
{
    constexpr double v = 0.5;

    constexpr auto r = inverted(v);

    STATIC_EXPECT( r == 2 );
}

CASE( "algorithm: inverted( [a ; b]      )   " " [mat2x2][inverted]" )
{
    constexpr matrix<double,2,2> A = {  1, 2  , 3,  4 };
    constexpr matrix<double,2,2> R = { -2, 1.5, 1, -0.5 };

    constexpr auto AI = inverted(A);

    STATIC_EXPECT( std20::equal( AI.begin(), AI.end(), R.begin(), approx() ) );
}

CASE( "algorithm: inverted( [a ; ...]    )   " " [matNxN][inverted]" )
{
#if 0
    constexpr matrix<int,3,3> A = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    constexpr matrix<int,3,3> R = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    constexpr auto AI = inverted(A);

    STATIC_EXPECT( std20::equal( AI.begin(), AI.end(), R.begin() ) );
#else
    EXPECT( !!"Implement inverted(Anxn)");
#endif
}

CASE( "algorithm: eye<N,T>()                 " " [mat][identity]" )
{
    constexpr matrix<int,3,3> R = { 1, 0, 0,
                                    0, 1, 0,
                                    0, 0, 1 };

    constexpr auto A = eye<int,3>();

    STATIC_EXPECT( std20::equal( A.begin(), A.end(), R.begin() ) );
}

// -----------------------------------------------------------------------
// Applets

CASE( "matrix: printing" " [.app][.print]" )
{
    std::cout
        << "\n"
        << "\nrowvec<int,2>({1,2}):\n" << rowvec<int,2>({1,2})
        << "\ncolvec<int,2>({1,2}):\n" << colvec<int,2>({1,2})
        << "\nmatrix<int,2,2>({1,2,3,4}):\n" << matrix<int,2,2>({1,2,3,4})
        ;
}

CASE( "matrix: eye<N,T>()" " [.app][mat]" )
{
    std::cout << "\neye<int,3>():\n" << eye<int,3>();
}

CASE( "matrix: [1 2] . [1 2]T" " [.app][row][col][vec]" )
{
    std::cout << "\n[1 2] . [1 2]T: " << (rowvec<int,2>({1,2}) * colvec<int,2>({1,2})) << "\n";
}

CASE( "matrix: [1 2]T . [1 2]" " [.app][col][row][vec]" )
{
    std::cout << "\n[1 2]T . [1 2]:\n" << (colvec<int,2>({1,2}) * rowvec<int,2>({1,2}));
}

CASE( "matrix: [1 2]T" " [.app][row][vec][transposed]" )
{
    std::cout << "\n[1 2]T:\n" << transposed(rowvec<int,2>({1,2}));
}

CASE( "matrix: [1 2]TT" " [.app][col][vec][transposed]" )
{
    std::cout << "\n[1 2]TT:\n" << transposed(colvec<int,2>({1,2}));
}

CASE( "matrix: [1 2;3 4]" " [.app][mat]" )
{
    std::cout << "\n[1 2;3 4]:\n" << matrix<int,2,2>({1,2,3,4});
}

CASE( "matrix: [1 2;3 4]T" " [.app][mat][transposed]" )
{
    std::cout << "\n[1 2;3 4]T:\n" << transposed(matrix<int,2,2>({1,2,3,4}));
}

CASE( "matrix: inverted([1 2;3 4])" " [.app][mat][inverted]" )
{
    std::cout << "\ninverted([1 2;3 4]):\n" << inverted(matrix<double,2,2>({1,2,3,4}));
}
