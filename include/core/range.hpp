// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This code is based on cpp11-range by Konrad Rudolph,
// https://github.com/klmr/cpp11-range

#ifndef CORE_RANGE_HPP_INCLUDED
#define CORE_RANGE_HPP_INCLUDED

//#include <cassert>
#include <iterator>
#include <type_traits>

#ifndef _MSC_VER
# include <experimental/type_traits>
#endif

namespace core { namespace ranges {

namespace detail {

// range iterator

template <typename T>
struct range_iter_base
{
    using value_type = typename std::iterator_traits<T*>::value_type;
    using pointer    = typename std::iterator_traits<T*>::pointer;
    using reference  = typename std::iterator_traits<T*>::reference;

    using iterator_category = std::input_iterator_tag;
    using difference_type   = typename std::iterator_traits<T*>::difference_type;

    range_iter_base( T current_)
    : current( current_)
    {}

    T operator*() const
    {
        return current;
    }

    T const * operator->() const
    {
        return &current;
    }

    range_iter_base& operator++()
    {
        ++current; return *this;
    }

    range_iter_base operator++( int )
    {
        auto copy = *this; ++*this; return copy;
    }

    bool operator==( range_iter_base const & other ) const
    {
        return current == other.current;
    }

    bool operator!=( range_iter_base const & other ) const
    {
        return ! ( *this == other );
    }

protected:
    T current;
};

// a 'classic' iterator range proxy:

template< typename T >
struct range_proxy
{
    struct iterator : detail::range_iter_base<T>
    {
        iterator( T current_ )
        : detail::range_iter_base<T>( current_ )
        {}
    };

    range_proxy( T first_, T last_)
    : first(first_)
    , last(last_)
    {}

    iterator begin() const { return first; }
    iterator end()   const { return last;  }

private:
    iterator first;
    iterator last;
};

// a stepping iterator range proxy:

template< typename T >
struct step_range_proxy
{
    struct iterator : detail::range_iter_base<T>
    {
        iterator( T current_, T step_, bool infinite_)
        : detail::range_iter_base<T>( current_ )
        , first( current_)
        , step( step_)
        , infinite( infinite_)
        {}

        iterator & operator++()
        {
            this->current = first + ++factor * step; return *this;
        }

        iterator operator++( int )
        {
            auto copy = *this; ++*this; return copy;
        }

        bool operator==( iterator const & other ) const
        {
            return !infinite && this->current >= other.current;
        }

        bool operator!=( iterator const & other ) const
        {
            return ! ( *this == other );
        }

    private:
        T factor = 0;
        T first;
        T step;
        bool infinite;
    };

    step_range_proxy(T first_, T last_, T step_, bool infinite_ = false )
    : first( first_, step_, infinite_ )
    , last(  last_ , step_, infinite_ )
    {}

    iterator begin() const { return first; }
    iterator end()   const { return last;   }

private:
    iterator first;
    iterator last;
};

// a range [first..inf), default=0, step=1:

template <typename T>
struct infinite_range_proxy : step_range_proxy<T>
{
    infinite_range_proxy( T first_ = T(), T step_ = T(1) )
    : step_range_proxy<T>( first_, first_, step_, true /*infinite*/ )
    {}

    auto step( T step_)
    {
        return infinite_range_proxy( *step_range_proxy<T>::begin(), step_);
    }
};

// a range [first..last), step=1:

template< typename T >
auto range_impl( T first, T last ) -> range_proxy<T>
{
    return { first, last };
}

// a range [first..last), step:

template< typename T >
auto range_impl( T first, T last, T step ) -> step_range_proxy<T>
{
    return { first, last, step };
}

// a range [first..inf), step:

template< typename T >
auto infinite_range_impl( T first, T step ) -> infinite_range_proxy<T>
{
    return { first, step };
}

} // namespace detail

// a range [first, last), step=1:

template< typename P, typename Q >
auto range( P first, Q last )
{
    using T = std::common_type_t<P,Q>;

    return detail::range_impl<T>( first, last );
}

// a range [first, last), step:

template< typename P, typename Q, typename R >
auto range( P first, Q last, R step )
{
    using T = std::common_type_t<P,Q,R>;

    return detail::range_impl<T>( first, last, step );
}

// a range [first, inf), step=1:

template< typename T >
auto range( T begin )
{
    return detail::infinite_range_proxy<T>{ begin };
}

// indices for a C-array:

template< typename T, std::size_t N >
auto indices( T (&)[N] ) -> detail::range_proxy<std::size_t>
{
    return { 0, N };
}

// indices for a container, require method size(), unless MSVC:

#ifndef _MSC_VER

template< typename C >
using has_size_t = decltype( std::declval<C&>().size() );

template< typename C >
constexpr bool has_size_v = std::experimental::is_detected_v<has_size_t, C>;

//static_assert( std::experimental::is_detected_v<has_size_t, std::vector<int>>, "std::vector must have size()" );
#endif // _MSC_VER

#ifdef _MSC_VER
template< typename C >
#else
template< typename C, typename = typename std::enable_if_t<has_size_v<C>> >
#endif
auto indices( C const & cont ) -> detail::range_proxy<decltype( cont.size() )>
{
    return { 0, cont.size() };
}

// indices for an initializer-list:

template< typename T >
auto indices( std::initializer_list<T> && cont ) -> detail::range_proxy<typename std::initializer_list<T>::size_type>
{
    return { 0, cont.size() };
}

} // ranges

// provide in namespace core:

using ranges::range;
using ranges::indices;

} // core

#endif // CORE_RANGE_HPP_INCLUDED
