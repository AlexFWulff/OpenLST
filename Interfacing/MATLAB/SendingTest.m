if ~exist('port','var')
    ports = serialportlist;
    port = serialport(ports(end),115200);
end

message = [2, 0, 17, 0, 1, 23];
header = [34, 105, length(message)];
message = [header, message];

port.flush
write(port,message,"uint8");

pause(0.1)

if (port.NumBytesAvailable > 0)
    bytes = read(port,port.NumBytesAvailable,"uint8");
    bytes = bytes(4:end) %#ok<*NOPRT>
    % unicodestr = native2unicode(bytes, "US-ASCII")
end