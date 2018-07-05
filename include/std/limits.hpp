// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef STD_LIMITS_INCLUDED
#define STD_LIMITS_INCLUDED

#if !( defined( __AVR ) && __AVR )

#include <limits>

namespace std20 {

    using std::numeric_limits;
}

#else

namespace std20 {

template< typename T > class numeric_limits;

template<>
class numeric_limits<float>
{
public:
    static constexpr bool is_signed  = true;
    static constexpr bool is_integer = false;
};

template<>
class numeric_limits<double>
{
public:
    static constexpr bool is_signed  = true;
    static constexpr bool is_integer = false;
};

template<>
class numeric_limits<__INT8_TYPE__>
{
    using T = __INT8_TYPE__;
public:
    static constexpr bool is_signed  = true;
    static constexpr bool is_integer = true;
    static constexpr int  digits     = __CHAR_BIT__ * sizeof(T) - 1;

    static constexpr T min() { return (-__INT8_MAX__ - 1); }
    static constexpr T max() { return   __INT8_MAX__     ; }
};

template<>
class numeric_limits<__INT16_TYPE__>
{
    using T = __INT16_TYPE__;
public:
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = true;
    static constexpr int  digits     = __CHAR_BIT__ * sizeof(T) - 1;

    static constexpr T min() { return (-__INT16_MAX__ - 1); }
    static constexpr T max() { return   __INT16_MAX__     ; }
};

template<>
class numeric_limits<__INT32_TYPE__>
{
    using T = __INT32_TYPE__;
public:
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = true;
    static constexpr int  digits     = __CHAR_BIT__ * sizeof(T) - 1;

    static constexpr T min() { return (-__INT32_MAX__ - 1); }
    static constexpr T max() { return   __INT32_MAX__     ; }
};

template<>
class numeric_limits<__INT64_TYPE__>
{
    using T = __INT64_TYPE__;
public:
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = true;
    static constexpr int  digits     = __CHAR_BIT__ * sizeof(T) - 1;

    static constexpr T min() { return (-__INT64_MAX__ - 1); }
    static constexpr T max() { return   __INT64_MAX__     ; }
};

} // namespace std20

#endif // __AVR

#endif // STD_LIMITS_INCLUDED
