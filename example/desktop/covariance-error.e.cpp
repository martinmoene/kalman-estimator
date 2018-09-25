// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "num/matrix-io.hpp"

using real_t = float;

template< int N, int M >
using matrix = num::matrix<real_t, N, M>;

int main()
{
    const int S = 2;            // system dimension
    const int M = 1;            // number of measurements

    using A_t = matrix<S,S>;    // System dynamics / transition matrix
    using H_t = matrix<M,S>;    // Output / measurement matrix
    using K_t = matrix<S,M>;    // Kalman gain

    const real_t dt = 1;              // time-step
    const real_t measnoise  = 0.14;     // position measurement noise (feet)
    const real_t accelnoise = 0.2;    // acceleration noise (feet/sec^2)

    A_t A = { 1, dt,            // System dynamics / transition matrix
              0, 1 };
    H_t H = { real_t(1.0),      // Output / measurement matrix
              real_t(0.0) };

    auto R = measnoise * measnoise;     // Measurement noise covariance
    auto P = accelnoise * accelnoise    // Process noise variance
           * A_t( { dt*dt*dt*dt/4, dt*dt*dt/2, dt*dt*dt/2, dt*dt } );
    auto PJ = P;

    real_t t = 0;
    for ( auto k = real_t(1.0); k > 0.001; k /= 2, t += dt )
    {
        using num::eye;

        std::cout
            << "\ndt:" << t << " P:\n" << P
            << "\ndt:" << t << " PJ:\n" << PJ
            ;

        K_t K = { 0.1f+k, 0.1f+k };

        P = P - K * H * P;

        auto A = eye<real_t,S>() - K * H;
        PJ = A * PJ * transposed(A) + K * R * transposed(K);
    }
}

// cl -EHsc -std:c++latest -I../include covariance-error.e.cpp && covariance-error.e.exe
// g++ -Wall -std=c++17 -I../include -o covariance-error.e.exe covariance-error.e.cpp && covariance-error.e.exe
