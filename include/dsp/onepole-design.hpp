
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Code inspired on 'A one-pole filter' by Nigel Redmon
// http://www.earlevel.com/main/2012/12/15/a-one-pole-filter/

#ifndef DSP_ONEPOLE_DESIGN_HPP_INCLUDED
#define DSP_ONEPOLE_DESIGN_HPP_INCLUDED

#include <iostream>

#include "dsp/onepole.hpp"
#include "dsp/filter-design.hpp"

namespace dsp { namespace onepole_designs {

using core::identity_t;

// ToDo
using onepole::AT;
using onepole::BT;

// provide constructor to AT:

template< typename T >
struct AAT : onepole::AT<T>
{
    template< typename... Args >
    AAT( Args... args )
    : onepole::AT<T>{ T(args)...}
    {}
};

// provide constructor to AB:

template< typename T >
struct BBT : onepole::BT<T>
{
    template< typename... Args >
    BBT( Args... args )
    : onepole::BT<T>{ T(args)...}
    {}
};

// digital filter bi-quad coefficients from filter design:

template< typename T >
struct OnePoleCoeffT
{
    using value_type = T;

    auto size() const
    {
        return b.size();
    }

    BBT<T> b;
    AAT<T> a;
};

// bi-quad design methods, primary template:

template< FilterResponse R, typename T >
struct onepole_design_method {};

// low pass:

template< typename T >
struct onepole_design_method<FilterResponse::low_pass, T >
{
    static auto design( T f, T fs )
    {
        const auto a1 = -exp( -T{1} * num::pi<T> * f / fs );
        const auto b0 = T{1} - abs(a1);
        
        return OnePoleCoeffT<T>{ {b0}, {a1} };
    }
};

// high pass:

template< typename T >
struct onepole_design_method<FilterResponse::high_pass, T >
{
    static auto design( T f, T fs )
    {
        const auto a1 = +exp( -T{1} * num::pi<T> * (T{0.5} - f / fs ) );
        const auto b0 = T{1} - abs(a1);

        return OnePoleCoeffT<T>{ {b0}, {a1} };
    }
};

// bi-quad design: single parameter type:

template< FilterResponse R, typename T >
auto onepole_design_impl( T f, T fs )
{
    return onepole_design_method<R,T>::design( f, fs );
}

// bi-quad design: allow for parameters of mixed type:

template< FilterResponse R, typename FR, typename SR >
auto onepole_design( FR f, SR fs )
{
    using T = std::common_type_t<float, FR, SR>;

    return onepole_design_impl<R,T>( f, fs );
}

// algorithm: create a bi-quad cascade from digital filter coefficients:

template< typename Coeff >
inline auto make_onepole( Coeff const & coeff )
{
    return OnePoleT<typename Coeff::value_type>{{ coeff.b }, { coeff.a }};
}

} // namespace onepole_designs

using onepole_designs::onepole_design;
using onepole_designs::make_onepole;

} // namespace dsp

#endif // DSP_ONEPOLE_DESIGN_HPP_INCLUDED
