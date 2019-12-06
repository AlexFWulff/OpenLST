if ~exist('port','var')
    ports = serialportlist;
    port = serialport(ports(end),115200);
end

ws = 2; delayVal = 0.2; payload_len = 3;
maxSize = 3 + 12 + payload_len;

% dump_status_cmd_t is 0x80 = 128
message = [1, 0, 17, 0, 1, 128];
header = [34, 105, length(message)];
message = [header, message];

port.flush
write(port,message,"uint8");

pause(delayVal)

while (port.NumBytesAvailable > 0)
    bytes = read(port,port.NumBytesAvailable,"uint8")
    
    dropCount = 0;
    drops = [];
    
    for i = 1:ws
        seqnum = bytes(((i-1)*maxSize + 6));
        
        if rand(1,1) > 0.8
            dropCount = dropCount + 1;
            drops = [drops seqnum];
        end
    end
    
    ack = [34 105 15 1 0 0 0 1 127 uint8(dropCount)];
    
    for i = 1:dropCount
        ack = [ack uint8(drops(i)) uint8(0)];
    end
    
    write(port,ack,"uint8");
    
    pause(delayVal)
end