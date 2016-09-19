// UDP-client.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h> 

#define MAXBUFLEN 100

int main(int argc, char *argv[])
{
	int rv;
	int sockfd;
	int numbytes;
	int msec;
	double time_taken;
	int16_t operand_2_input;
	int16_t operand_1_input;
	uint8_t opcode_input;
	uint8_t request_ID = 0;					// The Request ID starts at zero
	uint8_t received_request_ID;			// Request ID received from server
	int32_t received_result;				// Result of calculation received from server
	const uint8_t TML = 8;					// The message length is always eight bytes
	const uint8_t number_operands = 2;		// There's always two operands 
	char buffer[MAXBUFLEN];					// Buffer for input from server
	socklen_t server_addr_len;
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage server_addr;
	struct timeval start;
	struct timeval end;
	
	// Packed struct that will be sent as the data in a packet. A packed struct
	// is used as it will not contain any "padding" added by the compiler
	struct packed_message {   
		uint8_t tml;
		uint8_t req_id;
		uint8_t opcode; 
		uint8_t num_operands;
		int16_t operand1;   
		int16_t operand2;   
	} __attribute__((__packed__));

	// Keep in mind - argv[0] is the name of the program!
	if (argc != 3) {
		fprintf(stderr,"Error: inappropriate amount of arguments given. \n");
		exit(1);
	}

	memset(&hints, 0, sizeof hints);	// Ensure the struct is empty 
	hints.ai_family = AF_UNSPEC;		// No preference for IPv4 or IPv6
	hints.ai_socktype = SOCK_DGRAM;		// Use UDP sockets, not TCP

	// Purpose: Call getaddrinfo() and handle the (non-zero) error case. 
	// Detail: getaddrinfo() performs the necessary DNS and service name lookups
	// regarding the destination, in addition to creating the structs needed to make
	// a socket. The IP address or hostname (like google.com) is the first argument,
	// the port number (like http or 10011) is the second, etc. The fourth
	// argument "servinfo" is a struct pointer. Once the function is called,
	// servinfo will point to a linked list of one or more addrinfo's structs.
	if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo() error: %s\n", gai_strerror(rv));
		exit(2);
	}

	// Purpose: Create a socket using the socket() system call and handle
	// the (-1) error case. 
	// Detail: Loop through the servinfo linked list, searching for a valid
	// entry. servinfo contains one or more addrinfo structs, as a host
	// can have multiple addresses. When a valid entry is found, the socket
	// is created, and the loop breaks. "sockfd" is the file descriptor to the
	// socket. Note that the same socket is used to both send and receive data
	for(p = servinfo; p != NULL; p = p -> ai_next) {
	
		if ((sockfd = socket(p -> ai_family, p -> ai_socktype,
				p -> ai_protocol)) == -1) {
			perror("Client: socket() error");
			continue;
		}

		break;
	}
	
	// "p" is null if servinfo contains no addrinfo structs. Due to the
	// boolean condition in the above loop, a socket will never be created
	// if servinfo contains no structs 
	if (p == NULL) {
		fprintf(stderr, "Client: failed to create socket \n");
		exit(3);
	}

	// Infinte loop used to continuously prompt the user for information, send the 
	// information to the server, and then display the results from the server
	while(1) {
	
		// Get the opcode from the user
		printf("\nClient: Enter an opcode (0-5): ");
  		scanf("%d", &opcode_input);		// Grabs a decimal value and stores it
  		
  		// Ensure the opcode is valid (between 0 and 5, inclusive) 
  		while(opcode_input < 0 || opcode_input > 5) {
  			printf ("Client: Invalid input. Enter a new opcode: ");
  			scanf ("%d", &opcode_input);
  		}
  		
  		// Get the two operands from the user
  		printf("Client: Enter the first operand: ");
  		scanf("%d", &operand_1_input);
  		printf("Client: Enter the second operand: ");
  		scanf("%d", &operand_2_input);
  		
  		// Pack the operands, etc., into a struct. This struct will be sent as the data in a packet
  		struct packed_message packet = {
  			TML, request_ID, opcode_input, number_operands, operand_1_input, operand_2_input
  		};
  		
  		gettimeofday(&start, NULL);		// "Start" the timer

		// Purpose: Send the packet using sendto() and handle the (-1) error case.
		// Details: Note that an implicit bind occurs when sendto() is called.
		// This is why a bind() call is not needed in order to receive data from
		// the server using recvfrom(). 
		if ((numbytes = sendto(sockfd, (void *)&packet, sizeof(packet), 0,
				 p -> ai_addr, p -> ai_addrlen)) == -1) {
			perror("Client: sendto() error");
			exit(4);
		}
		
		printf("Client: sent %d bytes to %s. Now waiting to receive... \n", numbytes, argv[1]);
		
		server_addr_len = sizeof(server_addr);
		
		// Purpose: Receive a response from the server using recvfrom() and
		// handle the (-1) error case.
		// Details: Note that server_addr is never defined and is passed in
		// by reference. This is because the function will simply accept any
		// incoming data, as UDP is a connectionless protocol. The sender is not
		// known. So, the function will insert the address information after data
		// is received, since the sender is now known. The sockaddr type of structure
		// is used as the sender could be using IPv4 or IPv6 
		if ((numbytes = recvfrom(sockfd, buffer, MAXBUFLEN - 1, 0, 
				(struct sockaddr *)&server_addr, &server_addr_len)) == -1) {
			perror("Client: recvfrom() error");
			exit(1);
		}
		
		gettimeofday(&end, NULL);		// "Stop" the timer
		
		// Calculate the time taken (in milliseconds)
        time_taken = (end.tv_sec - start.tv_sec) * 1000.0;
    	time_taken += (end.tv_usec - start.tv_usec) / 1000.0; 
	
		// Get the Request ID and result from the buffer
		received_request_ID = *(uint8_t *)(buffer + 1);	
		received_result = *(int8_t *)(buffer + 3);		

		printf("Client: Packet received. Request ID = %d, Result = %d.", received_request_ID, received_result);
		printf("\nClient: Time taken = %f", time_taken);
		
		// Increment the Request ID for the next iteration of the loop (to keep it unique)
		request_ID++;
	}

	// Frees up the memory used by the servinfo struct created earlier 
	freeaddrinfo(servinfo);
	
	// Close the socket
	close(sockfd);

	return 0;
}
