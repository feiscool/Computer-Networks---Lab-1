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

int main(int argc, char *argv[])
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	int16_t operand_2_input;
	int16_t operand_1_input;
	uint8_t opcode_input;
	uint8_t request_ID = 0;					// The Request ID starts at zero
	const uint8_t TML = 8;					// The message length is always eight bytes
	const uint8_t number_operands = 2;		// There's always two operands 
	
	// Packed struct that will be sent as a packet. A packed struct
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

	// Call getaddrinfo() and handle the (non-zero) error case. 
	// getaddrinfo() performs the necessary DNS and service name lookups,
	// in addition to creating the structs needed to make a socket. The
	// IP address or hostname (like google.com) is the first argument,
	// the port number (like http or 10011) is the second, etc. The fourth
	// argument "servinfo" is a struct pointer. Once the function is called,
	// servinfo will point to a linked list of one or more addrinfo's structs
	if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo() error: %s\n", gai_strerror(rv));
		exit(2);
	}

	// Loop through all the results and make a socket
	for(p = servinfo; p != NULL; p = p->ai_next) {
	
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("Client: socket() error");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "Client: failed to create socket! \n");
		exit(3);
	}

	while(1) {
	
		// Get the opcode from the user
		printf("\nEnter an opcode: ");
  		scanf("%d", &opcode_input);
  		
  		// Ensure the opcode is valid
  		while(opcode_input < 0 || opcode_input > 5) {
  			printf ("Invalid input. Enter a new opcode: ");
  			scanf ("%d", &opcode_input);
  		}
  		
  		// Get the two operands from the user
  		printf("Enter the first operand: ");
  		scanf("%d", &operand_1_input);
  		printf("Enter the second operand: ");
  		scanf("%d", &operand_2_input);
  		
  		printf("\nOpcode = %d, Operand 1 = %d, Operand 2 = %d \n", opcode_input, operand_1_input, operand_2_input);
  		
  		// Pack the operands, etc., into a structure to be sent
  		struct packed_message packet = {
  			TML, request_ID, opcode_input, number_operands, operand_1_input, operand_2_input
  		};

		// Send the packet and handle the error case
		if ((numbytes = sendto(sockfd, (void *)&packet, sizeof(packet), 0,
				 p->ai_addr, p->ai_addrlen)) == -1) {
			perror("Client: sendto() error!");
			exit(4);
		}

		printf("Client: sent %d bytes to %s\n", numbytes, argv[1]);
		
		// Increment the Request ID for the next iteration of the loop to keep it unique 
		request_ID++;
	}

	// Frees up the memory used by the servinfo struct created earlier 
	freeaddrinfo(servinfo);
	close(sockfd);

	return 0;
}
