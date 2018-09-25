// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This code is based on cpp11-range by Konrad Rudolph,
// https://github.com/klmr/cpp11-range

#ifndef CORE_TRAITS_HPP_INCLUDED
#define CORE_TRAITS_HPP_INCLUDED

namespace core {

// identity: use to prevent a parameter to participate in template type deduction:

template< typename T >
struct identity { using type = T; };

template< typename T >
using identity_t = typename identity<T>::type;

} // namespace core

#endif // CORE_TRAITS_HPP_INCLUDED
