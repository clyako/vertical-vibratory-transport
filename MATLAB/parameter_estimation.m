%{

Use this file to perform a fit of the parameters used in the model to the
experimental data for each of the 10 recorded trials. The data for each
trial can be found in the 'data' folder. There are 3 main sections to this
file: C, D, and E. Note that Sections A and B should always be run first.

Run Section C to perform an optimizatin over the normal force and
coefficient of kinetic friction to best fit the dynamical model to the
experimental data. This uses particle swarm optimization, and the best
found parameters will be saved to the Workspace.

Run Section D to check the performance of the found parameters. Performance
is measured by several metrics, namely:
    • Average Relative Error: the average difference between the
    experimental part position and the simulated part position.
    • Average Relative Error Relative to Maximum Position: the value
    calculated above as a percent of the maximum position reached.
    • Percent Change in Normal Force: a measure of the difference between
    the normal force found from the fit and the measured normal force.
  
Run Section E to run a specific trial. You need to specify which trial to
run and which parameter set to use. You can view plots of the experimental
and simulation data by viewing various scopes in the Simulink file
'vibrational_transport_experimental_surface.slx'. 

%}

%% SECTION A: reset
clc
clear
close all

%% SECTION B: set parameters

% part mass and gravity
g = 9.81;       % [m/s/s] gravitational constant
m_o = 0.009;    % [kg] part mass

% Stick-slip parameters (dry friction)
mu_s = 0.72;    % [-] static coefficient of friction, experiment = 0.72
v_min = 1e-5;   % [m/s] relative velocity below which stiction is assumed

global relative_errors max_positions estimated_normal_forces;
relative_errors = zeros(1, 10);
max_positions = zeros(1, 10);
estimated_normal_forces = zeros(1, 10);
trial_to_run = 5;

params_from_paper = [0.6234, 10.6500, 13.0009, 9.9833, 6.7872, 6.0413, 7.0798, 6.2765, 6.6180, 7.1825, 8.2225];

disp('Parameters set');

%% SECTION C: run minimization

optimFunc = @(x) param_estimation(x, g, m_o, mu_s, v_min, trial_to_run);
lb = [0.0, 4, 12.8, 4, 4.9, 4.9, 4.9, 4.9, 4.56, 4, 8];
ub = [mu_s, 16, 19.2, 16, 7.3, 7.3, 7.3, 7.3, 6.84, 8, 24];

options = optimoptions(@particleswarm,'Display','iter', 'UseParallel', true);
params = particleswarm(optimFunc, 11, lb, ub, options);

%% SECTION D: calculate reported values using params
% best found params: params = [0.6234, 10.6500, 13.0009, 9.9833, 6.7872, 6.0413, 7.0798, 6.2765, 6.6180, 7.1825, 8.2225];
% Average relative error = 0.161 mm, Average relative error relative to max experimental part position = 11.87%
% Median percent change in normal force = 26.99%
% Mean percent change in normal force = 33.78%

% setting 'trial_to_run = 0' will calculate the deviation across all trials
% define your own parameters (or use those from the paper)
trial_to_run = 0;
params = params_from_paper;
param_estimation(params, g, m_o, mu_s, v_min, trial_to_run);

% global relative_errors max_positions estimated_normal_forces;
fprintf("Average relative error = %.3f mm, ", mean(relative_errors));
fprintf("Average relative error relative to max experimental part position = %2.2f%%\n", mean(relative_errors ./ max_positions) * 100)

measured_normal_forces = [11.089, 16.901, 5.278, 5.278, 5.278, 5.278, 5.278, 5.278, 5.278, 22.841];
normal_force_percent_change = zeros(1, length(measured_normal_forces));

for i = 1:length(measured_normal_forces)
    normal_force_percent_change(i) = 100 * (abs(estimated_normal_forces(i) - measured_normal_forces(i))) / measured_normal_forces(i);
end

fprintf("Median percent change in normal force = %2.2f%%\n", median(normal_force_percent_change))
fprintf("Mean percent change in normal force = %2.2f%%\n", mean(normal_force_percent_change))

%% SECTION E: run a specific trial, view results in Simulink

% parameters needs to be defined first, which occurs once the optimization
% is run (or you can specify them)
trial_to_run = 6;

% define your parameters here
params = params_from_paper;
param_estimation(params, g, m_o, mu_s, v_min, trial_to_run);

%% helper function(s)
function relative_error_sum = param_estimation(x, g, m_o, mu_s, v_min, trial_to_run)
    global relative_errors max_positions estimated_normal_forces
    % set mu_k for all trials
    mu_k = x(1);
    % set f_n per trial
    f_ns = x(2:end);

    % variables for reading in data
    data_folder = 'data/';
    trials_to_evaluate = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];

    if trial_to_run ~= 0
        trials_to_evaluate = trials_to_evaluate(trial_to_run);
    end

    num_trials = length(trials_to_evaluate);

    % vector to keep track of final relative position
    relative_error_sum = 0;
    
    % loop to calculate final position errors
    for idx = 1:num_trials
        % setting f_n
        f_n = f_ns(idx);
        if trial_to_run ~= 0
            f_n = f_ns(trial_to_run);
        end
        F_n = f_n * m_o * g;
       
        % creating the trial string and reading in the data
        trial_number = string(trials_to_evaluate(idx));
        trial_name = append('Trial #', trial_number, '.csv');
        filename = append(data_folder, trial_name);
        T = readtable(filename);
        
        i_0 = 4;
        i_f = 2;
        t_0 = T.t(i_0);
        timevals = T.t(i_0:end-i_f) - t_0;
        
        % select data entries, ommit first and last NaN values and convert to m
        platform_z = (T.zp(i_0:end-i_f) - T.zp(i_0)) / 1000.0;
        platform_vz = T.vzp(i_0:end-i_f) / 1000.0;
        platform_az = T.azp(i_0:end-i_f) / 1000.0;
        part_z = (T.zo(i_0:end-i_f) - T.zo(i_0)) / 1000.0;
        part_vz = T.vzo(i_0:end-i_f) / 1000.0;
        part_az = T.azo(i_0:end-i_f) / 1000.0;
        
        part_z_init = part_z(1);
        part_vz_init = part_vz(1);
        z_o_init = part_z_init;
        v_o_init = part_vz_init;
        
        % make time series
        platform_z_ts = timeseries(platform_z,timevals);
        platform_vz_ts = timeseries(platform_vz,timevals);
        platform_az_ts = timeseries(platform_az,timevals);
        part_z_ts = timeseries(part_z,timevals);
        part_vz_ts = timeseries(part_vz,timevals);
        part_az_ts = timeseries(part_az,timevals);
                
        end_time = timevals(end);
        
        % adding all local variables to 'base' workspace
        list = whos;
        N = length(list);
        for jdx = 1:N
            assignin('base', list(jdx).name, eval(list(jdx).name));
        end

        sim_results = sim('vibrational_transport_experimental_surface.slx', end_time);

        error = mean(abs(sim_results.experimental_part_position.Data - sim_results.part_position.Data)) * 1000;
        max_position = max(abs(sim_results.experimental_part_position.Data) * 1000);

        if trial_to_run == 0
            relative_errors(idx) = error;
            max_positions(idx) = max_position;
            estimated_normal_forces(idx) = f_n;
        end
        % adding up the relative errors
        relative_error_sum = relative_error_sum + error;
    end
end