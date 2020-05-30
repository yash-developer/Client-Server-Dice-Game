#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main (int argc, char *argv[]) {
	struct sockaddr_in serverAddr;
	int client_fd, portNum, currentScore;
	char msg[255], score[255];
	char serverMsgs[3][255] = {"You can now play","Game over: you won the game","Game over: you lost the game"};

	if (argc != 4) {
		fprintf(stderr, "Call Model: %s <Player Name> <IP> <Port #>\n", argv[0]);
		exit(0);
	}
	// Create Socket.
	client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd < 0) {
		fprintf(stderr, "Cannot create socket\n");
		exit(1);
	}
	serverAddr.sin_family = AF_INET;
	sscanf(argv[3], "%d", &portNum);
	serverAddr.sin_port = htons((uint16_t)portNum);
	//Convert IP address to network format.
	if(inet_pton(AF_INET, argv[2], &serverAddr.sin_addr) < 0) {
		fprintf(stderr, "Function inet_pton() failed\n");
		exit(1);
	}
	//Connect to Server
	if(connect(client_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
		fprintf(stderr, "Failed to Connect");
		exit(1);
	}
	printf("Connected.\n");
	//Send Name To Server.
	write(client_fd, argv[1], (strlen(argv[1])+1));

	while(1) {
		if(read(client_fd, msg, 255) < 0 ) {
			fprintf(stderr, "Error.\n");
			exit(0);
		}
		//Play
		if (strcmp(msg, serverMsgs[0]) == 0) {
			printf("Server: %s\n", msg);
			currentScore = (int)time(NULL) % 10  + 1;
			sprintf(score, "%d", currentScore);
			printf("%s: Score: %s\n\n", argv[1], score);
			write(client_fd, score, (strlen(score)+1));
		}
		// Exit Conditions
		if(strcmp(msg, serverMsgs[1]) == 0) {
			printf("I won the game\n\n");
			close(client_fd);
			exit(0);
		}
		if(strcmp(msg, serverMsgs[2]) == 0) {
			printf("I lost the game\n\n");
			close(client_fd);
			exit(0);
		}
	}
	return 0;
}
