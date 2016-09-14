/*
** talker.c -- a datagram "client" demo
*/

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
#include <time.h>

#define SERVERPORT "10011"	// the port users will be connecting to
//req code...sent to server, get back, increment, sent next
int request_id = 1;//rand()%255;

int main(int argc, char *argv[]) {
	struct packed_msg {   
		uint8_t tml;
		uint8_t req_id;
		uint8_t opcode; 
		uint8_t num_operands;
		int16_t operand1;   
		int16_t operand2;   
	} __attribute__((__packed__));  

	srand(time(NULL));
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;

	if (argc != 3) {
		fprintf(stderr,"usage: talker hostname message\n");
		exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(argv[1], SERVERPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and make a socket
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("talker: socket");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "talker: failed to create socket\n");
		return 2;
	}

	int operand1Input;
	int operand2Input;
	int opcodeInput;

	printf ("Enter Operand1: \n");
  	scanf ("%s", &operand1Input);
  	printf ("Enter Operand2: ");
  	scanf ("%s", &operand2Input);
  	printf ("Enter the Opcode: ");
  	scanf ("%s", &opcodeInput);

  	struct packed_msg packed = {8, request_id, opcodeInput, 2, operand1Input, operand2Input};
  	request_id++;

	if ((numbytes = sendto(sockfd, (void *)&packed, sizeof(packed), 0,
			 p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}

	//recv req id from server and use that to increment	
	freeaddrinfo(servinfo);

	printf("talker: sent %d bytes to %s\n", numbytes, argv[1]);
	close(sockfd);

	return 0;
}
