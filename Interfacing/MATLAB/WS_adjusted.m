% Inputs
data_rate_bps = 3000;
pass_len_mins = 10;
RTT_ms = 50;
packet_size = 255;  % bytes
payload_size = 240; % bytes
data_size = 102400; % bytes

% Intermediates
data_rate = floor(data_rate_bps / 8); % Bps
pass_len = pass_len_mins * 60; % s
RTT = RTT_ms / 1000;

WS = 1:30;

frame_size = packet_size * WS;
frame_content = payload_size * WS;

total_time = (frame_size ./ data_rate + RTT) .* (data_size .* 1.048) ./ frame_content;
plot(WS,total_time, 'LineWidth', 3);
xlabel('Window Size'); ylabel('Time (s)')