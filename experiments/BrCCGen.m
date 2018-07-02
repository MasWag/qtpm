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

for t =  1:10
    simTime = 60 * 1000 * t;
    
    tmp = BrCC.copy;
    tmp.Sim(0:10.0:simTime);
    figure; 
    tmp.PlotSignals({'velocity', 'ref_speed'});

    dlmwrite(sprintf('BrCCPulse-%d.tsv', simTime), ...
             vertcat(tmp.P.traj{1,1}.time, tmp.P.traj{1,1}.X(6,:), ...
                     tmp.P.traj{1,1}.X(3,:), abs(tmp.P.traj{1,1}.X(6,:) ...
                                                  - tmp.P.traj{1,1}.X(3,:)))','delimiter', '\t','precision', 10);
end
% BrCC.P.traj{1,1}.X(1:2,:)
% vertcat(BrCC.P.traj{1,1}.time, BrCC.P.traj{1,1}.X(1:2,:))
% [BrCC.P.traj{1,1}.time', BrCC.P.traj{1,1}.X(1:2,:)']'

return
