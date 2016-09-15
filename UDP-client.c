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
	uint8_t opcode_input;
	uint16_t operand_1_input;
	uint16_t operand_2_input;
	uint8_t request_ID = 0;
	const uint8_t TML = 8;
	const uint8_t number_operands = 2;
	
	// Packed struct that will be sent as packet
	struct packed_message {   
		uint8_t tml;
		uint8_t req_id;
		uint8_t opcode; 
		uint8_t num_operands;
		int16_t operand1;   
		int16_t operand2;   
	} __attribute__((__packed__));

	// Keep in mind - argv[0] is the name of the program
	if (argc != 3) {
		fprintf(stderr,"Error: inappropriate amount of arguments given. \n");
		exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

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
	
		// Get information from the user
		printf ("\nEnter an opcode: ");
  		scanf ("%d", &opcode_input);
  		
  		// Ensure the opcode is valid
  		while(opcode_input < 0 || opcode_input > 5) {
  			printf ("Invalid input. Enter a new opcode: ");
  			scanf ("%d", &opcode_input);
  		}
  		
  		printf ("Enter the first operand: ");
  		scanf ("%d", &operand_1_input);
  		printf ("Enter the second operand: ");
  		scanf ("%d", &operand_2_input);
  		
  		// Pack the operands, etc., into a structure to be sent
  		struct packed_message packet = {
  			TML, request_ID, opcode_input, number_operands, operand_1_input, operand_2_input
  		};

		// Send the packet and handle the error case
		// if ((numbytes = sendto(sockfd, (void *)&packet, sizeof(packet), 0,
// 				 p->ai_addr, p->ai_addrlen)) == -1) {
// 			perror("Client: sendto() error!");
// 			exit(4);
// 		}

		//freeaddrinfo(servinfo);

		printf("Client: sent %d bytes to %s\n", numbytes, argv[1]);
		//close(sockfd);
		
		request_ID++;
	}

	return 0;
}
