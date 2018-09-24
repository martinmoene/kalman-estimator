% Copyright 2018 by Martin Moene
%
% https:%github.com/martinmoene/kalman-estimator
%
% Distributed under the Boost Software License, Version 1.0.
% (See accompanying file LICENSE.txt or copy at http:%www.boost.org/LICENSE_1_0.txt)

% Time:

T  = 50;                % duration [s]
dt = 1;                 % t-step [s]

% System:

measnoise  = 10;        % position measurement noise [m]
accelnoise = 0.2;       % acceleration noise [m/s^2]

% A: System dynamics matrix: state-k-1 => state-k
A = [ 1, dt;            % pos => pos, vel => pos
      0, 1 ];           % pos => vel, vel => vel

% B: Control input matrix: control => state
B = [ dt * dt / 2;      % ctl => pos
      dt         ];     % ctl => vel

% H: Measurement output matrix: state => measurement estimation
H = [ 1, 0 ];           % pos => pos-est, vel=> vel-est

% x: Initial system state estimate
x = [ 0; 0 ];           % position, velocity
xhat = x;               % initial system state estimate

% R: Position measurement noise covariance
R = measnoise * measnoise;     % (rms)^2

% Q: Process noise variance
Q = accelnoise * accelnoise * ...
    [ dt*dt*dt*dt/4, dt*dt*dt/2;    %
         dt*dt*dt/2, dt*dt ];       %

% P: Initial estimate error covariance
P = Q;

disp( ["\nA = ", mat2str(A), " % System dynamics matrix (state-k-1 => state-k)\n"...
      ,"\nB = ", mat2str(B), " % Control input matrix (control => state)\n"...
      ,"\nH = ", mat2str(H), " % Measurement output matrix (state => measurement estimation)\n"...
      ,"\nR = ", num2str(R), " % Measurement noise covariance\n"...
      ,"\nQ = ", mat2str(Q), " % Process noise covariance\n"...
]);

% Initialize arrays for later plotting:

pos     = [];   % true position
poshat  = [];   % estimated position
posmeas = [];   % measured position
posgain = [];   % position gain
posecov = [];   % position gain estimation error covariance
vel     = [];   % true velocity
velhat  = [];   % estimated velocity
velgain = [];   % velocity gain
velecov = [];   % velocity gain estimation error covariance

% To keep it simple, we use a function in Matlab, hence no object creation.

for t = 0 : dt: T
    % Use a constant commanded acceleration of 1 [m/s^2]:
    u = [1];

    % Simulate the linear system:
    x = A * x + B * u + accelnoise * [(dt^2/2)*randn; dt*randn]; % process_noise( dt, accelnoise )

    % Simulate the noisy measurement:
    z = H * x + measnoise * randn; % meas_noise( measnoise );

    % Process a time-step:
    [K, xhat, P] = kalman( u, z, A, B, H, Q, R, P, xhat );

    pos     = [pos     ; x(1)    ];
    poshat  = [poshat  ; xhat(1) ];
    posmeas = [posmeas ; z       ];
    posgain = [posgain ; K(1)    ];
    posecov = [posecov ; P(1)    ];
    vel     = [vel     ; x(2)    ];
    velhat  = [velhat  ; xhat(2) ];
    velgain = [velgain ; K(2)    ];
    velecov = [velecov ; P(2)    ];
end

% Print collected readings:

disp([...
    "\n\npos = "     , mat2str(pos)...
    ,"\n\nposhat = " , mat2str(poshat)...
    ,"\n\nposmeas = ", mat2str(posmeas)...
    ,"\n\nposgain = ", mat2str(posgain)...
    ,"\n\nposecov = ", mat2str(posecov)...
    ,"\n\nvel = "    , mat2str(vel)...
    ,"\n\nvelhat = " , mat2str(velhat)...
    ,"\n\nvelgain = ", mat2str(velgain)...
    ,"\n\nvelecov = ", mat2str(velecov)
])

% Plot the results
close all;
t = 0 : dt : T;

figure;
plot(t,pos, t,posmeas, t,poshat);
grid;
xlabel('Time [s]');
ylabel('Position [m]');
title('Figure 1 - Position (True, Measured, and Estimated)')

figure;
plot(t,pos-posmeas, t,pos-poshat);
grid;
xlabel('Time [s]');
ylabel('Position Error [m]');
title('Figure 2 - Position Measurement Error and Position Estimation Error');

figure;
ax = plotyy(t,posgain, t, posecov);
grid;
xlabel('Time [s]');
ylabel(ax(1), 'Position Gain []');
ylabel(ax(2), 'Position Covariance []');
title('Figure 3 - Position Gain and Position Covariance');

figure;
plot(t,vel, t,velhat);
grid;
xlabel('Time [s]');
ylabel('Velocity [m/s]');
title('Figure 4 - Velocity (True and Estimated)');

figure;
plot(t,vel-velhat);
grid;
xlabel('Time [s]');
ylabel('Velocity Error [m/s]');
title('Figure 5 - Velocity Estimation Error');

figure;
ax = plotyy(t,velgain, t, velecov);
grid;
xlabel('Time [s]');
ylabel(ax(1), 'Velocity Gain []');
ylabel(ax(2), 'Velocity Covariance []');
title('Figure 3 - Velocity Gain and Velocity Covariance');

