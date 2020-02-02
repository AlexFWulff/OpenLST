% attach the clear FTDI to 1 and the black FTDI to 2
if ~exist('radio1','var') && ~exist('radio2','var')
    ports = serialportlist;
    port1 = ports(end);
    port2 = ports(end-2);
    
    if contains(port1,"FTAU4JIU") && contains(port2,"AB0JSQF8")
        radio1 = serialport(port2,115200);
        radio2 = serialport(port1,115200);
    elseif contains(port2,"FTAU4JIU") && contains(port1,"AB0JSQF8")
        radio1 = serialport(port1,115200);
        radio2 = serialport(port2,115200);
    else
        disp("Bad ports...")
        return
    end
end

sending_port = radio2;

message = [1, 0, 17, 0, 1, 17, 115, 97, 100, 106, 100, 115, 107, 106, 106, 100, 115, 97, 106, 100, 97, 115, 106, 107, 100, 108, 115, 106, 115, 108, 97, 100, 107, 108, 107, 97, 106, 108, 106, 107, 100, 115, 107, 108, 106, 100, 115, 97, 107, 108, 106, 100, 115, 97, 107, 108, 106];
header = [34, 105, length(message)];
message = [header, message];

sending_port.flush
write(sending_port,message,"uint8");

pause(0.1)

if (sending_port.NumBytesAvailable > 0)
    bytes = read(sending_port,sending_port.NumBytesAvailable,"uint8");
    bytes = bytes(4:end);
    disp(bytes)
    % unicodestr = native2unicode(bytes, "US-ASCII")
end