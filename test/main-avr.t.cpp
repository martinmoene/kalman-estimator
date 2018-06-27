//
// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Pro Trinket, atmega328: avr5: Free running Blink: 144kHz

#include "num/fixed-point.hpp"
#include "num/matrix.hpp"
#include "num/kalman.hpp"

// Fixed point numeric type for Kalman estimator:

using fp32_t = num::fixed_point<int, 15>;

// Kalman estimator type:

using kalman = num::kalman
<
#ifdef KE_NUMERIC_TYPE
    KE_NUMERIC_TYPE
#else
    double  // numeric type for computations
#endif
    , 2     // 2d system
    , 1     // single measurement
    , 1     // single control input
>;

// Create Kalman estimator and run it with process and measurement noise:

int main()
{
    // Time:

    const kalman::real_t T  = 50;       // duration [s]
    const kalman::real_t dt = 1;        // t-step [s]

    // System:

    kalman::real_t measnoise  = 10;     // position measurement noise [m]
    kalman::real_t accelnoise = 0.2;    // acceleration noise [m/s^2]

    // A: System dynamics matrix: state-k-1 => state-k
    kalman::A_t A = { 1, dt,            // pos => pos, vel => pos
                      0, 1 };           // pos => vel, vel => vel

    // B: Control input matrix: control => state
    kalman::B_t B = { dt * dt / 2,      // ctl => pos
                      dt         };     // ctl => vel

    // H: Measurement output matrix: state => measurement estimation
    kalman::H_t H = { 1, 0 };           // pos => pos-est, vel=> vel-est

    // x: Initial system state estimate
    kalman::x_t x = { 0, 0 };           // position, velocity
    kalman::xhat_t xhat = x;            // initial system state estimate

    // R: Position measurement noise covariance
    auto R = measnoise * measnoise;     // (rms)^2

    // Q: Process noise variance
    auto Q = accelnoise * accelnoise * kalman::Q_t(
        { dt*dt*dt*dt/4, dt*dt*dt/2,    //
             dt*dt*dt/2, dt*dt } );     //

    // P: Initial estimate error covariance
    auto P = Q;

    // Create Kalman estimator:

    kalman estim(
        dt          // time step
        , A         // system dynamics matrix: state-k-1 => state-k
        , B         // control input matrix: control => state
        , H         // measurement output matrix: state => measurement
        , Q         // process noise covariance
        , R         // position measurement noise covariance
        , P         // initial estimate error covariance
        , xhat      // initial system state estimate
    );

    // Use the estimator:

    for ( int i = 0; i*dt < T; ++i )
    {
        // Use a constant commanded acceleration of 1 [m/s^2]:
        const kalman::u_t u = {1};

        // Simulate the linear system:
        x = A * x + B * u ; // + process_noise( dt, accelnoise );

        // Simulate the noisy measurement:
        auto z = H * x; // + meas_noise( measnoise );

        // Process a time-step:
        // AVR: double: 122 function calls
        // AVR: fixed_point: 62 function calls
        estim.update( u, z );
    }
}
