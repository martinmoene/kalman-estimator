// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef NUM_KALMAN_HPP_INCLUDED
#define NUM_KALMAN_HPP_INCLUDED

#include "num/matrix.hpp"

#define kalman_MAJOR  0
#define kalman_MINOR  0
#define kalman_PATCH  0
#define kalman_VERSION  kalman_STRINGIFY(kalman_MAJOR) "." kalman_STRINGIFY(kalman_MINOR) "." kalman_STRINGIFY(kalman_PATCH)

#define kalman_STRINGIFY(  x )  kalman_STRINGIFY_( x )
#define kalman_STRINGIFY_( x )  #x

namespace num {

//
// Kalman estimator.
//
template
<
    typename T      // Numeric type
    , int S         // System dimension
    , int M         // Number of measurements
    , int U         // Number of control inputs
>
class kalman
{
public:
    using real_t = T;                   // Numeric type for computations
    using A_t    = num::matrix<T,S,S>;  // System dynamics matrix: state-k-1 => state-k
    using B_t    = num::matrix<T,S,U>;  // Control input matrix: control => state
    using H_t    = num::matrix<T,M,S>;  // Measurement output matrix: state => measurement estimation
    using Q_t    = num::matrix<T,S,S>;  // Process noise covariance
    using R_t    = num::matrix<T,M,M>;  // Measurement noise covariance
    using P_t    = num::matrix<T,S,S>;  // Estimate error covariance
    using K_t    = num::matrix<T,S,M>;  // Kalman gain
    using x_t    = num::colvec<T,S>;    // Initial system state estimate
    using u_t    = num::colvec<T,U>;    // Control inputs
    using z_t    = num::colvec<T,M>;    // Measurements inputs
    using xhat_t = num::colvec<T,S>;    // System state estimate

    // Constructor

    kalman(
        real_t const dt_        // Time step
        , A_t const & A_        // System dynamics matrix: state-k-1 => state-k
        , B_t const & B_        // Control input matrix: control => state
        , H_t const & H_        // Measurement output matrix: state => measurement estimation
        , Q_t const & Q_        // Process noise covariance
        , R_t const & R_        // Measurement noise covariance
        , P_t const & P_        // Initial estimate error covariance
        , xhat_t const & xhat_  // Initial system state estimate
    )
        : A( A_)
        , B( B_)
        , H( H_)
        , Q( Q_)
        , R( R_)
        , P( P_)
        , xhat( xhat_)
        , t(  0  )
        , dt( dt_)
    {}

    // Update estimator for dt:

    void update( u_t const & u, z_t const & z )
    {
        // Update the time:
        t += dt;

        // --------------------------------------
        // 1. Predict (time update)

        // 1a: Project the state ahead:
        xhat = A * xhat + B * u;

        // 1b: Project the error covariance ahead:
        P = A * P * transposed(A) + Q;

        // --------------------------------------
        // 2. Correct (measurement update)

        // 2a: Compute the Kalman gain:
        K = P * transposed(H) * inverted(H * P * transposed(H) + R);

        // 2b: Update estimate with measurement:
        xhat = xhat + K * (z - H * xhat);

        // 2c: Update the error covariance:
        P = (I() - K * H) * P;
    }

    // Observers:

    xhat_t system_state() const
    {
        return xhat;
    }

    K_t kalman_gain() const
    {
        return K;
    }

    P_t estimation_error_covariance() const
    {
        return P;
    }

    real_t time() const
    {
        return t;
    }

private:
    A_t I() const
    {
        return num::eye<real_t,S>();
    }

private:
    A_t const A;    // System dynamics matrix:
    B_t const B;    // Control input matrix
    H_t const H;    // Measurement output matrix
    Q_t const Q;    // Process noise covariance
    R_t const R;    // Measurement noise covariance

    K_t K;          // Kalman gain
    P_t P;          // Estimate error covariance
    xhat_t xhat;    // System state estimate

    real_t t;       // Elapsed time
    real_t dt;      // Time-step
};

} // namespace num

#endif // NUM_KALMAN_HPP_INCLUDED
