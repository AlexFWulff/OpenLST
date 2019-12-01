if ~exist('port','var')
    ports = serialportlist;
    port = serialport(ports(end),115200);
end

message = [1, 0, 17, 0, 1, 23];
header = [34, 105, length(message)];

message = [header, message];
write(port,message,"uint8");

if (port.NumBytesAvailable > 0)
    bytes = read(port,port.NumBytesAvailable,"uint8");
    unicodestr = native2unicode(bytes, "US-ASCII")
end