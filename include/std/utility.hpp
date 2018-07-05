// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef STD_UTILITY_INCLUDED
#define STD_UTILITY_INCLUDED

#if !( defined( __AVR ) && __AVR )

#include <utility>              // std::initializer_list, std::swap()

namespace std20 {

using std::move;

// constexpr swap():

template< typename T >
constexpr void swap(  T & a, T & b ) noexcept
{
    T t{std::move(a)}; a = std::move(b); b = std::move(t);
}

} // namespace std20

#else // __AVR

#include "std/type_traits.hpp"

namespace std20 {

// move():

template< typename T >
constexpr typename remove_reference<T>::type && move( T && t ) noexcept
{
    return static_cast< typename remove_reference<T>::type&& >( t );
}

// constexpr swap():

template< typename T >
constexpr void swap(  T & a, T & b ) noexcept
{
    T t{move(a)}; a = move(b); b = move(t);
}

} // naespace std20

// initializer_list, must be in namespace std:

namespace std {

template< typename T >
class initializer_list
{
public:
    using value_type      = T;
    using reference       = const T&;
    using const_reference = const T&;
    using size_type       = decltype(sizeof(int)); // std::size_t;
    using iterator        = const T*;
    using const_iterator  = const T*;

    constexpr initializer_list() noexcept
    : m_array( nullptr )
    , m_size(  0 )
    {}

    constexpr size_type size() const noexcept
    {
        return m_size;
    }

    constexpr const T * begin() const noexcept
    {
        return m_array;
    }

    constexpr const T * end() const noexcept
    {
        return begin() + size();
    }

private:
    constexpr initializer_list( const_iterator arr, size_type len )
    : m_array( arr )
    , m_size(  len )
    {}

private:
    iterator m_array;
    size_type m_size;
};

template< typename E >
constexpr const E * begin( std::initializer_list<E> il ) noexcept
{
     return il.begin();
}

template< typename E >
constexpr const E * end( std::initializer_list<E> il ) noexcept
{
     return il.end();
}

} // naespace std

#endif  // __AVR

#endif // STD_UTILITY_INCLUDED
