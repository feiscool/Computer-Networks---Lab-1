import socket
import sys
import struct
import binascii

errorCode = 0

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
portNum = int(sys.argv[1])

# Bind the socket to the port
server_address = ('', portNum)
print >>sys.stderr, 'starting up on %s port %s' % server_address
sock.bind(server_address)

while True:
    print >>sys.stderr, '\nwaiting to receive message'
    dataReceived, address = sock.recvfrom(1024)
    
    print >>sys.stderr, 'received %s bytes from %s' % (len(dataReceived), address)
    print >>sys.stderr, dataReceived

    dataCharArray = list(binascii.hexlify(dataReceived))

    #parse data
    TMLReceived = int(dataCharArray[0]) + int(dataCharArray[1]) 
    reqID = int(dataCharArray[2]) + int(dataCharArray[3]) 
    opcode = int(dataCharArray[4]) + int(dataCharArray[5])  
    numOperands = int(dataCharArray[6]) + int(dataCharArray[7]) 
    operand1 = int(dataCharArray[8]) + int(dataCharArray[9]) + int(dataCharArray[10]) + int(dataCharArray[11])
    operand2 = int(dataCharArray[12]) + int(dataCharArray[13]) + int(dataCharArray[14]) + int(dataCharArray[15])

    #do calculation here
    if (opcode == 0):
        result = int(operand1) + int(operand2)
    if (opcode == 1): 
        result = int(operand1) - int(operand2)
    if (opcode == 2):  
        result = int(operand1) | int(operand2)
    if (opcode == 3):
        result = int(operand1) & int(operand2)
    if (opcode == 4):
        result = int(operand1) >> int(operand2)
    if (opcode == 5): 
        result = int(operand1) << int(operand2)

    print('result: %i', result)
    
    #construct structResponse
    reply = struct.pack("i i i i", TMLReceived, reqID, errorCode, result)
    
    if reply:
        sent = sock.sendto(reply, address)
        print >>sys.stderr, 'sent %s bytes back to %s' % (sent, address)
