/********* Added by iceobelisk ****************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

extern int server_sock;

void connectToHost() {
	struct sockaddr_in server;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock == -1) {
       printf("Could not create socket\n");
       exit(-1);
    }
    printf("Socket created\n");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(7777);

    //Connect to remote server
    if(connect(server_sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
    	printf("Connect failed, ERROR\n");
    	exit(1);
    }
    printf("Server connected\n\n");
}
/************************************************/
