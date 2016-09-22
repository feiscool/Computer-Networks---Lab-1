import socket
import sys
import struct
import binascii

errorCode = 0
result = 0

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
portNum = int(sys.argv[1])

# Bind the socket to the port
server_address = ('', portNum)
print >> sys.stderr, 'Server: Starting up on port %s' % server_address
sock.bind(server_address)

while True:
    
    print >>sys.stderr, '\nServer: Waiting to receive packet'
    dataReceived, address = sock.recvfrom(1024)
    
    print >>sys.stderr, '\nServer: Received %s bytes from %s' % (len(dataReceived), address)

    dataCharArray = list(binascii.hexlify(dataReceived))

    #parse data
    TMLReceivedIn = str(dataCharArray[0]) + str(dataCharArray[1])
    reqIDIn = str(dataCharArray[2]) + str(dataCharArray[3])
    opcodeIn = str(dataCharArray[4]) + str(dataCharArray[5])  
    numOperands = str(dataCharArray[6]) + str(dataCharArray[7]) 
    if (str(dataCharArray[10]) + str(dataCharArray[11]) == '00'):
    	op1In = str(dataCharArray[8]) + str(dataCharArray[9])
    else:	
    	op1In = str(dataCharArray[8]) + str(dataCharArray[9]) + str(dataCharArray[10]) + str(dataCharArray[11])
    if (str(dataCharArray[14]) + str(dataCharArray[15]) == '00'):
    	op2In = str(dataCharArray[12]) + str(dataCharArray[13])
    else:
    	op2In = str(dataCharArray[12]) + str(dataCharArray[13]) + str(dataCharArray[14]) + str(dataCharArray[15])

    # Convert to int
    TMLReceived = int(TMLReceivedIn)
    reqID = int(reqIDIn)
    opcode = int(opcodeIn)
  
    operand1 = int(op1In, 16)
    operand2 = int(op2In, 16)

   # Do calculation here
    if (opcode == 0):
        result = operand1 + operand2
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

    # Construct structResponse

    try:
        reply.append(TMLReceived)
        reply.append(reqID)
        reply.append(errorCode)
        
        resultLen = len(str(result))
        zerosNeeded = 4 - resultLen
        resultArray = [int(x) for x in str(result)]
        #place each byte from the result,
    	#into byte array, starting at index 4.

        for x in range (0, zerosNeeded):
        	reply.append(0)

        for j in range (0, len(resultArray)):
            reply.append(resultArray[j])  
    except:
        errorCode = 256       

    packedStruct = struct.pack("=bbbI", int(7), int(reqID), int(errorCode), int(result))
    
    # print("***contents of reply data**")
    # print("packedStruct %c" , packedStruct)
    # print("*****")

    if reply:
        sent = sock.sendto(packedStruct, address)
        del reply [:]
        print >>sys.stderr, 'Server: Sent %s bytes back to %s' % (sent, address)
