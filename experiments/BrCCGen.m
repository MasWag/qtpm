% scipt created to generate Monitoring input
% 
% Masaki Waga
% Created: 2018/06/26

clear;
InitBreach;

warning('off', 'Simulink:LoadSave:EncodingMismatch')
mdl = 'cruise_ctrl_kpit';
BrCC = BreachSimulinkSystem(mdl);

speed_ref_gen = pulse_signal_gen({'SpeedRef'}); % Generate a pulse signal for speed ref

InputGen = BreachSignalGen({speed_ref_gen});

InputGen.SetParam({'SpeedRef_base_value', 'SpeedRef_pulse_period', ...
                   'SpeedRef_pulse_amp','SpeedRef_pulse_width'}, ... 
                  [20 500 30 .5]);

BrCC.SetInputGen(InputGen);

simTime = 60 * 100

BrCC.Sim(0:.05:simTime);
figure; 
BrCC.PlotSignals({'velocity', 'ref_speed'});

dlmwrite(sprintf('BrCCPulse-%d.tsv', simTime), ...
         vertcat(BrCC.P.traj{1,1}.time, BrCC.P.traj{1,1}.X(6,:), ...
                 BrCC.P.traj{1,1}.X(3,:), abs(BrCC.P.traj{1,1}.X(6,:) ...
                                              - BrCC.P.traj{1,1}.X(3,:)))','delimiter', '\t','precision', 10);

% BrCC.P.traj{1,1}.X(1:2,:)
% vertcat(BrCC.P.traj{1,1}.time, BrCC.P.traj{1,1}.X(1:2,:))
% [BrCC.P.traj{1,1}.time', BrCC.P.traj{1,1}.X(1:2,:)']'

return
