% Copyright 2018 by Martin Moene
%
% https:%github.com/martinmoene/kalman-estimator
%
% Distributed under the Boost Software License, Version 1.0.
% (See accompanying file LICENSE.txt or copy at http:%www.boost.org/LICENSE_1_0.txt)

function...     % [K, xhat, P] = kalman(u, z, A, B, H, Q, R, P, xhat)
[
    K...        % Kalman gain
    , xhat...   % Updated system state estimate
    , P...      % Updated estimate error covariance
] = kalman(
    u...        % Control inputs
    , z...      % Measurements inputs
    , A...      % System dynamics matrix: state-k-1 => state-k
    , B...      % Control input matrix: control => state
    , H...      % Measurement output matrix: state => measurement estimation
    , Q...      % Process noise covariance
    , R...      % Measurement noise covariance
    , P...      % (Initial) estimate error covariance
    , xhat...   % (Initial) system state estimate
)
    % Update the time:
    %t += dt;

    % --------------------------------------
    % 1. Predict (time update)

    % 1a: Project the state ahead:
    xhat = A * xhat + B * u;

    % 1b: Project the error covariance ahead:
    P = A * P * A' + Q;

    % --------------------------------------
    % 2. Correct (measurement update)

    % 2a: Compute the Kalman gain:
    K = P * H' * inv(H * P * H' + R);

    % 2b: Update estimate with measurement:
    xhat = xhat + K * (z - H * xhat);

    % 2c: Update the error covariance:
    I = eye(length(xhat));
    P = (I - K * H) * P;
end % kalman
