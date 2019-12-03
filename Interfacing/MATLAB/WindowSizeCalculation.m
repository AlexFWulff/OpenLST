% Inputs
data_rate_bps = 3000;
pass_len_mins = 10;
RTT_ms = 200;
max_payload_size = 240; % bytes
data_size = 100000; % bytes

% Intermediates
data_rate = floor(data_rate_bps / 8); % Bps
pass_len = pass_len_mins * 60; % s
RTT = RTT_ms / 1000;

if (data_size > pass_len * data_rate)
    disp("Impossible!");

else
    rate_needed = ceil(data_size / pass_len);
    window_size_bytes = ceil(rate_needed / RTT);
    window_size_packets = ceil(window_size_bytes / max_payload_size);
    disp("Window Size Needed (num packets): " + window_size_packets)
end