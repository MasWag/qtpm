%% Scipt to Generate Monitoring Input of Cruise Control Model for
%% Different Simulation Time
% This script generates monitoring inputs (tsv files) for the experiment of our
% implementation of quantitative timed pattern matching using the cruise
% control model <https://github.com/ERATOMMSD/cruise-control-simulink>
%
% * Author: Masaki Waga
% * Created: 2018/06/26
%

%% The Output Format
%
% * $t$: the abosolute time
% * $v$: the velocity of the car
% * $v_{\mathit{ref}}$: the reference velocity of the car
%
% The output format is as follows.
%
% $$t, v, v_{\mathit{ref}}, |v - v_{\mathit{ref}}|$$

%% Initialize Breach and load the Simulink model
clear;
InitBreach;

warning('off', 'Simulink:LoadSave:EncodingMismatch')
mdl = 'cruise_ctrl_kpit';
BrCC = BreachSimulinkSystem(mdl);

%% Define the input generator for the reference speed
% Generate a pulse signal for speed ref
speed_ref_gen = pulse_signal_gen({'SpeedRef'}); 

InputGen = BreachSignalGen({speed_ref_gen});

%%
% The input to the model is completely periodic and without any random
% noise.
InputGen.SetParam({'SpeedRef_base_value', 'SpeedRef_pulse_period', ...
                   'SpeedRef_pulse_amp','SpeedRef_pulse_width'}, ... 
                  [20 500 30 .5]);

BrCC.SetInputGen(InputGen);

%% Run the model changing the simulation time
% The simulation time and the number of the sampled values is
% changed but the sampling frequency is fixed (100 Hz?).

for t =  1:10
    simTime = 60 * 1000 * t;
    
    tmp = BrCC.copy;
    %%
    % set Kp to 0.1 to make the system less stable
    tmp.SetParam('Kp', 0.1);
    %%
    % It samples the data every 10 unit time (ms ?) and finish the
    % execution after 60 * 1000 * t unit time
    tmp.Sim(0:10.0:simTime);
    %figure; 
    %tmp.PlotSignals({'velocity', 'ref_speed'});

    %%
    % It writes the log to a tsv file.
    dlmwrite(sprintf('./data/10.0/BrCCPulse-%d.tsv', simTime), ...
             vertcat(tmp.P.traj{1,1}.time, tmp.P.traj{1,1}.X(6,:), ...
                     tmp.P.traj{1,1}.X(3,:), abs(tmp.P.traj{1,1}.X(6,:) ...
                                                  - tmp.P.traj{1,1}.X(3,:)))','delimiter', '\t','precision', 10);
end

return
