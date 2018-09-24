// Copyright 2018 by Martin Moene
//
// https://github.com/martinmoene/kalman-estimator
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Pro Trinket, atmega328: avr5: Free running Blink: 144kHz

#include "num/fixed-point.hpp"
#include "num/matrix.hpp"
//#include "dsp/kalman.hpp"

// Blink LED to measure loop frequency:

#include <avr/io.h>

#define led_ddr  DDRB
#define led_port PORTB
#define led_bit  PB5

constexpr uint8_t bit_mask( uint8_t pin )
{
    return static_cast<uint8_t>( 1 << pin );
}

inline void init_led()
{
    led_port = 0b11111111;
    led_ddr |= bit_mask( led_bit );
}

inline void blink_led()
{
    led_port ^= bit_mask( led_bit );
}

//---------------------------------------------------------

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
        , compute_kalman_gain(
#if defined(KE_UPDATE_KALMAN_GAIN) && KE_UPDATE_KALMAN_GAIN == 0
            false
#else
            true
#endif // defined
        )
    {}

    // Update estimator for dt:

    void update( u_t const & u, z_t const & z )
    {
        // Update the time:
//      t += dt;

        // --------------------------------------
        // 1. Predict (time update)

        // 1a: Project the state ahead:
        xhat = A * xhat + B * u;

        if ( compute_kalman_gain )
        {
            // 1b: Project the error covariance ahead:
            P = A * P * transposed(A) + Q;

            // --------------------------------------
            // 2. Correct (measurement update)

            // 2a: Compute the Kalman gain:
            K = P * transposed(H) * inverted(H * P * transposed(H) + R);

            // 2c: Update the error covariance:
            P = (I() - K * H) * P;
        }

        // 2b: Update estimate with measurement:
        xhat = xhat + K * (z - H * xhat);
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
    auto I() const
    {
        return num::eye<T,S>();
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

    bool compute_kalman_gain;  // Update Kalman gain?
};

} // namespace num

//---------------------------------------------------------

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
    init_led();

    // Time:

//  const kalman::real_t T  = 50;       // duration [s]
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

    // Use a constant commanded acceleration of 1 [m/s^2]:
    const kalman::u_t u(1); // = {1};

    // Simulate the linear system:
    x = A * x + B * u ; // + process_noise( dt, accelnoise );

    // Simulate the noisy measurement:
    auto z = H * x; // + meas_noise( measnoise );

    for ( ;; )
    {
        blink_led();

        // To add
        // - ADC measurement
        // - DAC actuation

        // Process a time-step:
        estim.update( u, z );
    } // for(t)
}
