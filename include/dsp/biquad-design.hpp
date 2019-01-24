// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Code based on Biquad C++ source code by Nigel Redmo,
// http://www.earlevel.com/main/2012/11/26/biquad-c-source-code/

#ifndef DSP_BIQUAD_DESIGN_HPP_INCLUDED
#define DSP_BIQUAD_DESIGN_HPP_INCLUDED

#include "dsp/biquad.hpp"
#include "dsp/filter-design.hpp"

#if __cplusplus >= 201703L
# define dsp_MAYBE_UNUSED  [[maybe_unused]]
#else
# define dsp_MAYBE_UNUSED
#endif

namespace dsp {

// filter type selection (lp, hp, ...)

enum class FilterResponse
{
    all_pass,
    low_pass,
    high_pass,
    band_pass,
    band_stop,
    low_shelf,
    high_shelf,
    peak_band_eq
};

// biquad realization form (direct form 1, 2, transposed):

enum class BiquadForm
{
    df1,
    df2,
    df2_transposed
};

} // namespace dsp

namespace dsp { namespace biquad_designs {

// ToDo
using biquad::AT;
using biquad::BT;

// provide constructor to AT:

template< typename T >
struct AAT : biquad::AT<T>
{
    template< typename... Args >
    AAT( Args... args )
    : biquad::AT<T>{ T(args)...}
    {}
};

// provide constructor to AB:

template< typename T >
struct BBT : biquad::BT<T>
{
    template< typename... Args >
    BBT( Args... args )
    : biquad::BT<T>{ T(args)...}
    {}
};

// digital filter bi-quad coefficients from filter design:

template< typename T >
struct BiquadCoeffT
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
struct biquad_design_method {};

// low pass:

template< typename T >
struct biquad_design_method<FilterResponse::low_pass, T >
{
    static auto design( dsp_MAYBE_UNUSED T dbGain, T f, T fs, T Q )
    {
        const auto K = prewarp( f, fs );

        const auto norm = 1 / (1 + K / Q + K * K);
        const auto b0   = K * K * norm;
        const auto b1   = 2 * b0;
        const auto b2   = b0;
        const auto a1   = 2 * (K * K - 1) * norm;
        const auto a2   = (1 - K / Q + K * K) * norm;

        return BiquadCoeffT<T>{ {b0, b1, b2}, {a1,a2} };
    }
};

// high pass:

template< typename T >
struct biquad_design_method<FilterResponse::high_pass, T >
{
    static auto design( dsp_MAYBE_UNUSED T dbGain, T f, T fs, T Q )
    {
        const auto K = prewarp( f, fs );

        const auto norm = 1 / (1 + K / Q + K * K);
        const auto b0   = 1 * norm;
        const auto b1   = -2 * b0;
        const auto b2   = b0;
        const auto a1   = 2 * (K * K - 1) * norm;
        const auto a2   = (1 - K / Q + K * K) * norm;

        return BiquadCoeffT<T>{ {b0, b1, b2}, {a1,a2} };
    }
};

// band pass:

template< typename T >
struct biquad_design_method<FilterResponse::band_pass, T >
{
    static auto design( dsp_MAYBE_UNUSED T dbGain, T f, T fs, T Q )
    {
        const auto K = prewarp( f, fs );

        const auto norm = 1 / (1 + K / Q + K * K);
        const auto b0   = K / Q * norm;
        const auto b1   = 0;
        const auto b2   = -b0;
        const auto a1   =  2 * (K * K - 1) * norm;
        const auto a2   = (1 - K / Q + K * K) * norm;

        return BiquadCoeffT<T>{ {b0, b1, b2}, {a1,a2} };
    }
};

// band stop:

template< typename T >
struct biquad_design_method<FilterResponse::band_stop, T >
{
    static auto design( dsp_MAYBE_UNUSED T dbGain, T f, T fs, T Q )
    {
        const auto K = prewarp( f, fs );

        const auto norm =  1 / (1 + K / Q + K * K);
        const auto b0   = (1 + K * K) * norm;
        const auto b1   =  2 * (K * K - 1) * norm;
        const auto b2   =  b0;
        const auto a1   =  b1;
        const auto a2   = (1 - K / Q + K * K) * norm;

        return BiquadCoeffT<T>{ {b0, b1, b2}, {a1,a2} };
    }
};

// low shelf:

template< typename T >
struct biquad_design_method<FilterResponse::low_shelf, T >
{
    static auto design( T dbGain, T f, T fs, T Q )
    {
        const auto V = dbV_factor( dbGain );
        const auto K = prewarp( f, fs );

        if ( dbGain >= 0 )
        {
            // boost:
            const auto norm =  1 / (1 + std::sqrt(2) * K + K * K);
            const auto b0   = (1 + std::sqrt(2*V) * K + V * K * K) * norm;
            const auto b1   =  2 * (V * K * K - 1) * norm;
            const auto b2   = (1 - std::sqrt(2*V) * K + V * K * K) * norm;
            const auto a1   =  2 * (K * K - 1) * norm;
            const auto a2   = (1 - std::sqrt(2) * K + K * K) * norm;

            return BiquadCoeffT<T>{ {b0, b1, b2}, {a1,a2} };
        }

        // cut:
        const auto norm =  1 / (1 + std::sqrt(2*V) * K + V * K * K);
        const auto b0   = (1 + std::sqrt(2) * K + K * K) * norm;
        const auto b1   =  2 * (K * K - 1) * norm;
        const auto b2   = (1 - std::sqrt(2) * K + K * K) * norm;
        const auto a1   =  2 * (V * K * K - 1) * norm;
        const auto a2   = (1 - std::sqrt(2*V) * K + V * K * K) * norm;

        return BiquadCoeffT<T>{ {b0, b1, b2}, {a1,a2} };
    }
};

// high shelf:

template< typename T >
struct biquad_design_method<FilterResponse::high_shelf, T >
{
    static auto design( T dbGain, T f, T fs, T Q )
    {
        const auto V = dbV_factor( dbGain );
        const auto K = prewarp( f, fs );

        // boost:
        if ( dbGain >= 0 )
        {
            const auto norm =  1 / (1 + std::sqrt(2) * K + K * K);
            const auto b0   = (V + std::sqrt(2*V) * K + K * K) * norm;
            const auto b1   =  2 * (K * K - V) * norm;
            const auto b2   = (V - std::sqrt(2*V) * K + K * K) * norm;
            const auto a1   =  2 * (K * K - 1) * norm;
            const auto a2   = (1 - std::sqrt(2) * K + K * K) * norm;

            return BiquadCoeffT<T>{ {b0, b1, b2}, {a1,a2} };
        }

        // cut:
        const auto norm =  1 / (V + std::sqrt(2*V) * K + K * K);
        const auto b0   = (1 + std::sqrt(2) * K + K * K) * norm;
        const auto b1   =  2 * (K * K - 1) * norm;
        const auto b2   = (1 - std::sqrt(2) * K + K * K) * norm;
        const auto a1   =  2 * (K * K - V) * norm;
        const auto a2   = (V - std::sqrt(2*V) * K + K * K) * norm;

        return BiquadCoeffT<T>{ {b0, b1, b2}, {a1,a2} };
    }
};

// peak band equalizer:

template< typename T >
struct biquad_design_method<FilterResponse::peak_band_eq, T >
{
    static auto design( T dbGain, T f, T fs, T Q )
    {
        const auto V = dbV_factor( dbGain );
        const auto K = prewarp( f, fs );

        // boost:
        if ( dbGain >= 0 )
        {
            const auto norm =  1 / (1 + 1 / Q * K + K * K);
            const auto b0   = (1 + V / Q * K + K * K) * norm;
            const auto b1   =  2 * (K * K - 1) * norm;
            const auto b2   = (1 - V / Q * K + K * K) * norm;
            const auto a1   = b1;
            const auto a2   = (1 - 1 / Q * K + K * K) * norm;

            return BiquadCoeffT<T>{ {b0, b1, b2}, {a1,a2} };
        }

        // cut:
        const auto norm =  1 / (1 + V / Q * K + K * K);
        const auto b0   = (1 + 1 / Q * K + K * K) * norm;
        const auto b1   =  2 * (K * K - 1) * norm;
        const auto b2   = (1 - 1 / Q * K + K * K) * norm;
        const auto a1   = b1;
        const auto a2   = (1 - V / Q * K + K * K) * norm;

        return BiquadCoeffT<T>{ {b0, b1, b2}, {a1,a2} };
    }
};

// bi-quad design: single parameter type:

template< FilterResponse R, typename T >
auto biquad_design_impl( T dbGain, T f, T fs, T Q )
{
    return biquad_design_method<R,T>::design( dbGain, f, fs, Q );
}

// bi-quad design: allow for parameters of mixed type:

template< FilterResponse R, typename DBG, typename FR, typename SR, typename QT >
auto biquad_design( DBG dbGain, FR f, SR fs, QT Q )
{
    using T = std::common_type_t<float, DBG, FR, SR, QT>;

    return biquad_design_impl<R,T>( dbGain, f, fs, Q );
}

// algorithm: create a bi-quad cascade from digital filter coefficients:

template< typename Coeff >
inline auto make_biquad( Coeff const & coeff )
{
    return BiQuadT<typename Coeff::value_type>{{ coeff.b }, { coeff.a }};
}

} // namespace biquad_designs

using biquad_designs::biquad_design;
using biquad_designs::make_biquad;

} // namespace dsp

#endif // DSP_BIQUAD_DESIGN_HPP_INCLUDED
