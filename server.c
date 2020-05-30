#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

void servicePlayers(int, int);

int main (int argc, char *argv[]) {
	struct sockaddr_in servAddr;
	int server_fd, portNum, player1, player2;
	
	if (argc != 2) {
		fprintf(stderr, "Call model: %s <Port #>\n", argv[0]);
		exit(0);
	}
	//Create Socket.
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		fprintf(stderr, "Could not Create Socket.\n");
		exit(1);
	}
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sscanf(argv[1], "%d", &portNum);
	servAddr.sin_port = htons((uint16_t)portNum);
	//Bind Socket.
	if (bind(server_fd, (struct sockaddr *) &servAddr, sizeof(servAddr))<0) {
		fprintf(stderr, "Bind Failure.\n");
		exit(1);
	}
	//Server Listening.
	if(listen(server_fd, 3) < 0) {
		fprintf(stderr, "Listen Failure");
		exit(1);
	}
	printf("Bind Successful.\nListening for Players.\n");
	
	while(1) {
		player1 = accept(server_fd, (struct sockaddr *)NULL, NULL);
		if (player1 <0) {
			fprintf(stderr, "Could Not Accept Connection");
			continue;
		}
		printf("A Player Connected\n");

		player2 = accept(server_fd, (struct sockaddr *)NULL, NULL);
		if (player2 < 0) {
			close(player1);
			fprintf(stderr, "Could Not Accept Connection\n");
			continue;
		}
		printf("Another Player Connected\n");
		printf("\nGot two players, Starting Game.\n\n");

		// Forking...
		if (!fork()) {
			servicePlayers(player1,player2);
		}
	}
	return 0;
}

void servicePlayers(int player1, int player2) {
	int players[2] = {player1, player2};
	int total[2] = {0,0}, currentScore, i;
	char msg[255], cMsg[255], namePlayers[2][255];
	char serverMsgs[3][255] = {"You can now play","Game over: you won the game","Game over: you lost the game"};
	//Read Names.
	for (i = 0; i < 2; i++) {
		if (read(players[i], namePlayers[i], 255)<0) {
			close(players[i]);
			fprintf(stderr, "Read error.");
			exit(1);
		}
	}

	while(1) {
		for (i = 0; i < 2; i++) {
			strcpy(msg, serverMsgs[0]);
			write(players[i], msg, (strlen(msg)+1));
			//Reading Player Score
			if (read(players[i], cMsg, 255) <0) {
				close(players[i]);
				fprintf(stderr, "Read error.\n");
				exit(1);
			}
			sscanf(cMsg, "%d", &currentScore);
			total[i] += currentScore;
			printf("%s: Total Score: %d\n", namePlayers[i], total[i]);
			sleep(1);

			// Exit Conditions:
                	if (total[i] >= 100) {
				if(i==0) {
                        		strcpy(msg, serverMsgs[1]);
                		        write(players[0], msg, (strlen(msg)+1));
        	                	strcpy(msg, serverMsgs[2]);
	                        	write(players[1], msg, (strlen(msg)+1));

					printf("%s v/s %s : Game Ended", namePlayers[0], namePlayers[1]);
                        		close(players[0]);
                        		close(players[1]);
					exit(0);
				}
				if(i==1) {
                                        strcpy(msg, serverMsgs[1]);
                                        write(players[1], msg, (strlen(msg)+1));
                                        strcpy(msg, serverMsgs[2]);
                                        write(players[0], msg, (strlen(msg)+1));

					printf("%s v/s %s : Game Ended", namePlayers[0], namePlayers[1]);
                                        close(players[0]);
                                        close(players[1]);
					exit(0);
				}
                	}
		}
	}
}
