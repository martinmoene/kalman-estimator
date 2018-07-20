// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef STD_TYPE_TRAITS_INCLUDED
#define STD_TYPE_TRAITS_INCLUDED

#if !( defined( __AVR ) && __AVR )

#include <type_traits>

namespace std20 {

    using std::remove_reference;
    using std::integral_constant;
    using std::is_integral;
    using std::is_integral_v;
    using std::is_signed;
    using std::is_signed_v;
    using std::enable_if;
    using std::enable_if_t;
}

#else

#include "std/limits.hpp"

namespace std20 {

template< typename T > struct remove_reference      { typedef T type; };
template< typename T > struct remove_reference<T&>  { typedef T type; };
template< typename T > struct remove_reference<T&&> { typedef T type; };

template< typename T, T v >
struct integral_constant
{
    static constexpr T value = v;

    typedef T value_type;
    typedef integral_constant type;

    constexpr operator value_type() const noexcept { return value; }
    constexpr value_type operator()() const noexcept { return value; }
};

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

template< typename T >
struct is_integral : integral_constant<bool, numeric_limits<T>::is_integer>{};

template< typename T >
inline constexpr bool is_integral_v = is_integral<T>::value;

template< typename T >
struct is_signed : integral_constant<bool, numeric_limits<T>::is_signed>{};

template< typename T >
inline constexpr bool is_signed_v = is_signed<T>::value;

template< typename T, typename U>
struct is_same : false_type {};

template< typename T>
struct is_same<T, T> : true_type {};

template< typename T, typename U>
inline constexpr bool is_same_v = is_same<T,U>::value;

template<bool B, typename T = void>
struct enable_if {};

template<typename T>
struct enable_if<true, T> { typedef T type; };

template< bool B, typename T = void >
using enable_if_t = typename enable_if<B,T>::type;

} // namespace std20

#endif // __AVR

#endif // STD_TYPE_TRAITS_INCLUDED
