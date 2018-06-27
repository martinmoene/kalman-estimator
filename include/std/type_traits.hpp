// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef STD_TYPE_TRAITS_INCLUDED
#define STD_TYPE_TRAITS_INCLUDED

#include "std/limits.hpp"

namespace std {

template< typename T, T v >
struct integral_constant
{
    static constexpr T value = v;

    typedef T value_type;
    typedef integral_constant type;

    constexpr operator value_type() const noexcept { return value; }
    constexpr value_type operator()() const noexcept { return value; }
};

using true_type = std::integral_constant<bool, true>;
using false_type = std::integral_constant<bool, false>;

template< typename T >
struct is_integral : integral_constant<bool, numeric_limits<T>::is_integer>{};

template< typename T >
inline constexpr bool is_integral_v = is_integral<T>::value;

template< typename T >
struct is_signed : integral_constant<bool, numeric_limits<T>::is_signed>{};

template< typename T >
inline constexpr bool is_signed_v = is_signed<T>::value;

template<bool B, typename T = void>
struct enable_if {};

template<typename T>
struct enable_if<true, T> { typedef T type; };

template< bool B, typename T = void >
using enable_if_t = typename enable_if<B,T>::type;

}

#endif // STD_TYPE_TRAITS_INCLUDED
