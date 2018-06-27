// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef STD_UTILITY_INCLUDED
#define STD_UTILITY_INCLUDED

namespace std {

// swap():

template< typename T >
void swap(  T & a, T & b ) noexcept
{
    T t(a); a = b; b = t;
}

// initializer_list:

template< typename T >
class initializer_list
{
public:
    using value_type      = T;
    using reference       = const T&;
    using const_reference = const T&;
    using size_type       = decltype(sizeof(int)); // std::size_t;
    using iterator        = const T*;
    using const_iterator  = const T *;

    constexpr initializer_list() noexcept;

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

#endif // STD_UTILITY_INCLUDED
