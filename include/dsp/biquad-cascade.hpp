// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef DSP_BIQUAD_CASCADE_HPP_INCLUDED
#define DSP_BIQUAD_CASCADE_HPP_INCLUDED

#include "dsp/biquad.hpp"

namespace dsp { namespace biquad_cascade {

// Identity: use to prevent a parameter to participate in template type deduction:

template< typename T >
struct identity { using type = T; };

template< typename T >
using identity_t = typename identity<T>::type;

//
// A cascade of bi-quad filter sections with arithmetic type T, maximum of N sections.
//
template< typename T, int N >
class BiQuadCascadeT
{
public:
    using value_type = T;
    using index_type = int;
    using BiQuad = BiQuadT<T>;

    // Construction:

    template< typename... Bq >
    BiQuadCascadeT( Bq... bq )
    : biquad{ bq... }
    , next{ sizeof...(Bq) }
    {}

    // Observers:

    auto size() const
    {
        return next;
    }

    static auto reserved()
    {
        return N;
    }

    auto coeff_a( index_type i ) const
    {
        assert( 0 <= i && i < next );

        return biquad[i].coeff_a();
    }

    auto coeff_b( index_type i ) const
    {
        assert( 0 <= i && i < next );

        return biquad[i].coeff_b();
    }

    auto state( index_type i ) const
    {
        assert( 0 <= i && i < next );

        return biquad[i].state();
    }

    // Modifiers:

    void clear()
    {
        next = 0;
    }

    void append( BiQuad bq )
    {
        assert( next < reserved() );

        biquad[next++] = bq;
    }

    void remove( index_type i )
    {
        assert( 0 <= i && i < next );

        --next;
        for ( auto k = i; k != next; ++k )
        {
            biquad[k] = biquad[k + 1];
        }
    }

    void insert( index_type i, BiQuad bq )
    {
        assert( 0 <= i && i < next && next < reserved() );

        for ( auto k = next; k != i; --k )
        {
            biquad[k] = biquad[k - 1];
        }
        ++next;

        biquad[i] = bq;
    }

    // replace bi-quad, return previous content:

    auto replace( index_type i, BiQuad bq )
    {
        assert( 0 <= i && i < next );

        bq.swap( biquad[i] );

        return bq;
    }

    // Iteration, non-const:

    auto begin()
    {
        return &biquad[0];
    }

    auto end()
    {
        return &biquad[next];
    }

    // Iteration, const:

    auto begin() const
    {
        return &biquad[0];
    }

    auto end() const
    {
        return &biquad[next];
    }

private:
    // the bi-quad sections (std:array if not for AVR):
    BiQuad biquad[N];

    // insertion point:
    index_type next = 0;
};

// true if filter cascade is stable:

template< typename T, int N >
auto is_stable( BiQuadCascadeT<T,N> const & filters )
{
    bool result = true;
    for ( auto const & bq : filters )
    {
        result = result && bq.is_stable();
    }
    return result;
}

// response of filter cascade:

template< typename T, int N >
auto response( BiQuadCascadeT<T,N> const & filters, identity_t<T> fnorm )
{
    using Complex = typename BiQuadCascadeT<T,N>::BiQuad::Complex;

    Complex result{1};
    for ( auto const & bq : filters )
    {
        result *= response( bq, fnorm );
    }
    return result;
}

// reset filter cascade:

template< typename T, int N >
void reset( BiQuadCascadeT<T,N> & filters )
{
    for ( auto & bq : filters )
    {
        bq.reset();
    }
}

// compute step cascade filter result:

template< typename T, int N >
auto step( BiQuadCascadeT<T,N> & filters, identity_t<T> v )
{
    for ( auto & bq : filters )
    {
        v = bq.step( v );
    }
    return v;
}

// compute df2 step cascade filter result:

template< typename T, int N >
auto step_df2( BiQuadCascadeT<T,N> & filters, identity_t<T> v )
{
    for ( auto & bq : filters )
    {
        v = bq.step_df2( v );
    }
    return v;
}

// compute transposed df2 step cascade filter result:

template< typename T, int N >
auto step_df2_transposed( BiQuadCascadeT<T,N> & filters, identity_t<T> v )
{
    for ( auto & bq : filters )
    {
        v = bq.step_df2_transposed( v );
    }
    return v;
}

} // namespace biquad_cascade

using biquad_cascade::BiQuadCascadeT;

} // namespace dsp

#endif // DSP_BIQUAD_CASCADE_HPP_INCLUDED
