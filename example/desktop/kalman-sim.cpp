// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "num/fixed-point-io.hpp"
#include "dsp/kalman.hpp"
#include "dsp/kalman-io.hpp"
#include "core/vector-io.hpp"

#include <random>

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

// Process and measurement noise generators:

std::default_random_engine generator;

kalman::x_t process_noise( kalman::real_t const dt, kalman::real_t const accelnoise )
{
    std::normal_distribution<double> distQ( 0.0, 1.0);

    return accelnoise * kalman::x_t({ (dt * dt / 2)* distQ( generator ), dt * distQ( generator )});
}

kalman::real_t meas_noise( kalman::real_t const measnoise )
{
    std::normal_distribution<double> distR( 0.0, 1.0);

    return measnoise * distR( generator );
}

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

    std::cout <<
        "\nA = % System dynamics matrix (state-k-1 => state-k)\n"<< A  <<
        "\nB = % Control input matrix (control => state)\n" << B  <<
        "\nH = % Measurement output matrix (state => measurement estimation)\n" << H  <<
        "\nR = " << R << " % Measurement noise covariance\n" <<
        "\nQ = % Process noise covariance\n" << Q
        ;

    // Initialize arrays for later plotting:

    typedef std::vector<kalman::real_t> vec_t;
    vec_t pos;      // true position
    vec_t posmeas;  // measured position
    vec_t poshat;   // estimated position
    vec_t posgain;  // position gain
    vec_t posecov;  // position gain estimation error covariance
    vec_t vel;      // true velocity
    vec_t velhat;   // estimated velocity
    vec_t velgain;  // velocity gain
    vec_t velecov;  // velocity gain estimation error covariance

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
        x = A * x + B * u + process_noise( dt, accelnoise );

        // Simulate the noisy measurement:
        auto z = H * x + meas_noise( measnoise );

        // Process a time-step:
        estim.update( u, z );

        // Save some parameters for plotting later:
        const auto K    = estim.kalman_gain();
        const auto xhat = estim.system_state();
        const auto ecov = estim.estimation_error_covariance();

        pos    .push_back( x(0)    );
        posmeas.push_back( z       );
        poshat .push_back( xhat(0) );
        posgain.push_back( K(0)    );
        posecov.push_back( ecov(0) );
        vel    .push_back( x(1)    );
        velhat .push_back( xhat(1) );
        velgain.push_back( K(1)    );
        velecov.push_back( ecov(1) );
    } // for(t)

    // Print collected readings:

    std::cout
        << "\npos = "     << pos
        << "\nposmeas = " << posmeas
        << "\nposhat = "  << poshat
        << "\nposgain = " << posgain
        << "\nposecov = " << posecov
        << "\nvel = "     << vel
        << "\nvelhat = "  << velhat
        << "\nvelgain = " << velgain
        << "\nvelecov = " << velecov
        ;
}

// cl -EHsc -std:c++latest -I../include kalman-class.cpp && kalman-class.exe
// g++ -Wall -std=c++17 -I../include -o kalman-class.exe kalman-class.cpp && kalman-class.exe
