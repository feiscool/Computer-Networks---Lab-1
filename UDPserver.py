import socket
   
UDP_IP = "131.204.14.65"
UDP_PORT = 10011
   
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # Internet, UDP
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1) #The SO_REUSEADDR flag tells the kernel to reuse a local socket in TIME_WAIT state, without waiting for its natural timeout to expire.
sock.bind((UDP_IP, UDP_PORT))
sock.listen(5)
(conn, addr) = sock.accept()

while True:
	print ("before")
   	lent = sock.recvfrom(1024) # buffer size is 1024 bytes
	print ("received message:", lent)