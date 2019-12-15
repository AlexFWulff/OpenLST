% Inputs
data_rate_bps = 3000;
pass_len_mins = 10;
RTT_ms = 200;
payload_size = 240; % bytes
data_size = 3000; % bytes

% Intermediates
data_rate = floor(data_rate_bps / 8); % Bps
pass_len = pass_len_mins * 60; % s
RTT = RTT_ms / 1000;

% WS = 1;
syms WS
frame_size = WS * payload_size;
eq = pass_len >= (frame_size / data_rate + RTT) * (data_size / frame_size);
WS = solve(eq,WS)

if (data_size > pass_len * data_rate)
    disp("Impossible!");

else
    rate_needed = ceil(data_size / pass_len);
    window_size_bytes = ceil(rate_needed / RTT);
    window_size_packets = ceil(window_size_bytes / payload_size);
    disp("Window Size Needed (num packets): " + window_size_packets)
end
