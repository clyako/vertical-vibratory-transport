%{

Use this file to observe and/or compare the object motion in response to
the optimal surface waveform with the experimentally observed object
motion. 

Use Section B to select which experimental trial you want to use for
comparison. This section loads the experimental data, and also grabs the
object's initial position and velocity, which are used to the determine the
optimal motion in Section C.

In Section C, there are several parameters that can be modified: the
friction coefficients, mu_s and mu_k, the mass of the object, m_o, and the
normal force relative to the object weight, f_n. Note that modifying these
parameters will not change the read-in experimental data, only the object
motion with respect to the calculated optimal waveform. 

Use Section D to set the final parameters for the optimal waveform. Namely,
the maximum surface acceleration (i.e., an actuator limit), and the
frequency of the surface motion. This section also defines the timeseries
that is the optimal surface motion (acceleration, velocity, and position).

Run Section E to update the various scopes in the Simulink file
'vibrational_transport_optimal_surface.slx'. 

%}

%% SECTION A: reset
clc
clear
close all

%% SECTION B: set trial to run and import experimental data
trial_number = 5;
trial_name = append('data/Trial #', string(trial_number), '.csv');

trial = readtable(trial_name);

disp('Table imported');

i_0 = 4;
i_f = 2;
t_0 = trial.t(i_0);
timevals = trial.t(i_0:end-i_f) - t_0;

% select data entries, ommit first and last NaN values and convert to m
platform_z = (trial.zp(i_0:end-i_f) - trial.zp(i_0)) / 1000.0;
platform_vz = trial.vzp(i_0:end-i_f) / 1000.0;
platform_az = trial.azp(i_0:end-i_f) / 1000.0;
part_z = (trial.zo(i_0:end-i_f) - trial.zo(i_0)) / 1000.0;
part_vz = trial.vzo(i_0:end-i_f) / 1000.0;
part_az = trial.azo(i_0:end-i_f) / 1000.0;

part_z_init = part_z(1);
part_vz_init = part_vz(1);

% make time series
platform_z_ts = timeseries(platform_z,timevals);
platform_vz_ts = timeseries(platform_vz,timevals);
platform_az_ts = timeseries(platform_az,timevals);
part_z_ts = timeseries(part_z,timevals);
part_vz_ts = timeseries(part_vz,timevals);
part_az_ts = timeseries(part_az,timevals);

disp('Data import successful');

%% SECTION C: set experimental parameters
% experimental parameters were measured as follows:
% • m_o = 0.009 [kg]
% • mu_s = 0.72 [-]
% • mu_k = 0.59 [-]

% Object mass and gravity
g = 9.81;                   % [m/s/s] gravitational constant
m_o = 0.009;                % [kg] object mass

% Stick-slip parameters (dry friction)
mu_s = 0.72;                % [-] static coefficient of friction
mu_k = 0.59;                % [-] kinetic coefficient of friction

% set how hard to squeeze
f_n = 20;                   % [-] normal force relative to object weight
F_n = f_n * m_o * 9.81;     % [N] normal force
v_min = 1e-5;               % [m/s] relative velocity below which stiction is assumed

% Initial conditions
z_o_init = part_z_init;   % [m] object initial position
v_o_init = part_vz_init;  % [m/s] object initial velocity

disp('Parameters set');

%% SECTION D: optimal surface motion
freq = 40;                          % [Hz] surface motion frequency
a_max = 20 * g;                     % max surface acceleration

a_kn = mu_k * F_n / m_o;            % kinetic acceleration from friction
offset = 5;                       % this is needed to "catch" the object appropriately while avoiding numerical errors
a_sn = mu_s * F_n / m_o - offset;   % static acceleration from friction
a_lim = (mu_s * f_n + 1) * g;       % minimum downard acceleration for slip

T = 1/freq;                         % [s] period
T_1 = (a_kn + g) * T / (a_sn + a_kn);
T_2 = T_1 + (a_max + a_kn + g) * (a_sn - g) * T / (2 * a_max * (a_sn + a_kn));

% define surface acceleration
syms surface_acceleration_period(x)
surface_acceleration_period(x) = piecewise(x < T_1, a_sn - g, (T_1 <= x) & (x < T_2), -a_max, x >= T_2, a_max);

% calculate starting velocity
syms acceleration_period_int(x)
acceleration_period_int(x) = int(surface_acceleration_period(x),x,0,x);
syms acceleration_period_int_int(x)
acceleration_period_int_int(x) = int(acceleration_period_int(x),x,0,x);
surface_starting_velocity = - acceleration_period_int_int(T) / T;

% calculate velocity
syms surface_velocity_period(x)
surface_velocity_period(x) = acceleration_period_int(x) + surface_starting_velocity;

syms surface_position_period(x)
surface_position_period(x) = int(surface_velocity_period(x),x,0,x);

% generalise [0,T] to periodic over ]-inf,+inf[
surface_acceleration(x) = surface_acceleration_period(mod(x,T));
surface_velocity(x) = surface_velocity_period(mod(x,T));
surface_position(x) = surface_position_period(mod(x,T));

% create time series
sim_duration = timevals(end); % [s]
optimal_timevals = 0:T/500:sim_duration;
surface_acceleration_ts = timeseries(double(surface_acceleration(optimal_timevals)), optimal_timevals);
surface_velocity_ts = timeseries(double(surface_velocity(optimal_timevals)), optimal_timevals);
surface_position_ts = timeseries(double(surface_position(optimal_timevals)), optimal_timevals);

surface_starting_velocity_double = double(surface_starting_velocity);
v_o_init = surface_starting_velocity_double;

disp('Optimal motion set');

%% SECTION E: run simulation
simulation_out = sim('vibrational_transport_optimal_surface.slx', timevals(end));
