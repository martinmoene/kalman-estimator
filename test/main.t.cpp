// C++11 - auto-register test cases.

#include "num/kalman.hpp"
#include "num/matrix.hpp"
#include "lest.hpp"

#define CASE( name ) lest_CASE( specification(), name )

#define kalman_PRESENT( x ) \
    std::cout << #x << ": " << x << "\n"

#define kalman_ABSENT( x ) \
    std::cout << #x << ": (undefined)\n"

// Suppress:
// - unused parameter, for cases without assertions such as [.std...]
#if defined __clang__
# pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined __GNUC__
# pragma GCC   diagnostic ignored "-Wunused-parameter"
#endif

lest::tests & specification()
{
    static lest::tests tests;
    return tests;
}

CASE( "kalman version" "[.version]" )
{
    kalman_PRESENT( kalman_MAJOR );
    kalman_PRESENT( kalman_MINOR );
    kalman_PRESENT( kalman_PATCH );
    kalman_PRESENT( kalman_VERSION );
}

CASE( "matrix version" "[.version]" )
{
    kalman_PRESENT( matrix_MAJOR );
    kalman_PRESENT( matrix_MINOR );
    kalman_PRESENT( matrix_PATCH );
    kalman_PRESENT( matrix_VERSION );
}

CASE( "__cplusplus" "[.stdc++]" )
{
    kalman_PRESENT( __cplusplus );

#ifdef _MSVC_LANG
    kalman_PRESENT( _MSVC_LANG );
#else
    kalman_ABSENT(  _MSVC_LANG );
#endif

//    kalman_PRESENT( kalman_CPLUSPLUS );
//    kalman_PRESENT( kalman_CPLUSPLUS_V );
}

CASE( "compiler version" "[.compiler]" )
{
#ifdef kalman_COMPILER_CLANG_VERSION
    kalman_PRESENT( kalman_COMPILER_CLANG_VERSION );
#else
    kalman_ABSENT(  kalman_COMPILER_CLANG_VERSION );
#endif

#ifdef kalman_COMPILER_GNUC_VERSION
    kalman_PRESENT( kalman_COMPILER_GNUC_VERSION );
#else
    kalman_ABSENT(  kalman_COMPILER_GNUC_VERSION );
#endif

#ifdef kalman_COMPILER_MSVC_VERSION
    kalman_PRESENT( kalman_COMPILER_MSVC_VERSION );
#else
    kalman_ABSENT(  kalman_COMPILER_MSVC_VERSION );
#endif
}

CASE( "presence of C++ language features" "[.stdlanguage]" )
{
#if defined(__cpp_exceptions)
    kalman_PRESENT( __cpp_exceptions );
#else
    kalman_ABSENT(  __cpp_exceptions );
#endif

#if defined(__EXCEPTIONS)
    kalman_PRESENT( __EXCEPTIONS );
#else
    kalman_ABSENT(  __EXCEPTIONS );
#endif

#if defined(_CPPUNWIND)
    kalman_PRESENT( _CPPUNWIND );
#else
    kalman_ABSENT(  _CPPUNWIND );
#endif
}

CASE( "presence of C++ library features" "[.stdlibrary]" )
{
}

CASE( "kalman configuration" "[.kalman]" )
{
    EXPECT( true );
}

CASE( "matrix configuration" "[.matrix]" )
{
    EXPECT( true );
}

CASE( "presence of kalman library features" "[.kalman]" )
{
    EXPECT( true );
}

CASE( "presence of matrix library features" "[.matrix]" )
{
    EXPECT( true );
}

int main( int argc, char * argv[] )
{
    return lest::run( specification(), argc, argv /*, std::cout */ );
}
