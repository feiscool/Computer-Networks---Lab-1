import socket
import sys
import struct
import binascii

TML = 8
reqID = 0
numOperands = 2
# Create a TCP/IP socket
ipAddress = 'localhost'#sys.argv[1]
portNum = 10011#sys.argv[2]

while (1):
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	opcode = input("Client: Enter an opcode (0-5): ")
	#Ensure the opcode is valid (between 0 and 5, inclusive) 
	if(opcode < 0 or opcode > 5): 
	    opcode = input("Client: Opcode must be between 0 and 5. Please enter a new opcode: ");

	operand1 = input("Client: Enter the first operand: ")
	operand2 = input("Client: Enter the second operand: ")

	#construct structResponse

	packedStruct = struct.pack("=bbbbhh", int(TML), int(reqID), int(opcode), int(numOperands), int(operand1), int(operand2))

	# Connect the socket to the port where the server is listening
	server_address = (ipAddress, portNum)
	print >>sys.stderr, 'connecting to', ipAddress, 'port', portNum
	sock.connect(server_address)

	#After the connection is established, 
	#data can be sent through the socket with send() #
	#and received with recv().
	try:
	    # Send data
	    print >>sys.stderr, 'sending...'
	    sock.send(packedStruct)

	    print("Sent ", len(packedStruct), " bytes to ", ipAddress)    
	    reqID = reqID + 1
	    # Look for the response
	    
	    data = sock.recv(16)
	    amount_received = 0
	    amount_expected = len(data)
	    
	    while amount_received < amount_expected:
	        amount_received += len(data)
	        print(list(binascii.hexlify(data)))

	finally:
	    test = sock.recv(16)
	    
sock.close()

