figure
Data = load('MovData.mat');


plot(1:length(Data.SIM_X),Data.SIM_X,'b');
hold on
plot(1:length(Data.WL_X),Data.WL_X,'r');
title('X-axis rotations');
legend('Simulator', 'Wheel Loader');
xlabel('Sample number');
ylabel('Rotation (radians)');
axis tight
hold off

figure
plot(1:length(Data.SIM_Y),Data.SIM_Y,'b');
hold on
plot(1:length(Data.WL_Y),Data.WL_Y,'r');
title('Y-axis rotations');
legend('Simulator', 'Wheel Loader');
xlabel('Sample number');
ylabel('Rotation (radians)');
axis tight
hold off
