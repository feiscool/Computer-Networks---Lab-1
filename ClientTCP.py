import socket
import sys
import struct

TML = 0
reqID = 0
numOperands = 2
# Create a TCP/IP socket
ipAddress = 'localhost'#sys.argv[1]
portNum = 10011#sys.argv[2]

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

opcode = input("Client: Enter an opcode (0-5): ")

#Ensure the opcode is valid (between 0 and 5, inclusive) 
if(opcode < 0 or opcode > 5): 
    opcode = input("Client: Opcode must be between 0 and 5. Please enter a new opcode: ");

operand1 = input("Client: Enter the first operand: ")
operand2 = input("Client: Enter the second operand: ")

#construct structResponse

packedStruct = struct.pack("=bbbbhh", int(TML), int(reqID), int(opcode), int(numOperands), int(operand1), int(operand2))
    
print("***contents of reply data**")
print("packedStruct %c" , packedStruct)
print("*****")


# Connect the socket to the port where the server is listening
server_address = (ipAddress, portNum)
print >>sys.stderr, 'connecting to %s port %s', ipAddress, portNum
sock.connect(server_address)

#After the connection is established, 
#data can be sent through the socket with sendall() #
#and received with recv().
try:
    # Send data
    print >>sys.stderr, 'sending...'
    sock.sendall(packedStruct)

    print("Sent ", len(packedStruct), " bytes to ", ipAddress)    

    # Look for the response
    amount_received = 0
    amount_expected = len(packedStruct)
    
    # while amount_received < amount_expected:
    #     data = sock.recv(16)
    #     amount_received += len(data)
    #     print >>sys.stderr, 'received "%s"' % data

finally:
    print >>sys.stderr, 'closing socket'
    sock.close()

