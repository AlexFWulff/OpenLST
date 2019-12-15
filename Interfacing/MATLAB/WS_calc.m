% Inputs
data_rate_bps = 3000;
pass_len_mins = 10;
RTT_ms = 200;
payload_size = 240; % bytes
data_size = 100000; % bytes

% Intermediates
data_rate = floor(data_rate_bps / 8); % Bps
pass_len = pass_len_mins * 60; % s
RTT = RTT_ms / 1000;

WS = 1:30;

frame_size = payload_size * WS;
total_time = (frame_size ./ data_rate + RTT) .* (data_size ./ frame_size);
plot(WS,total_time);