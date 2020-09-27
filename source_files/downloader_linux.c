#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include "transfer.h"

#define portnum 12345
#define FILE_SIZE 500 
#define BUFFER_SIZE 1024

int writeFile(int sockfd, FILE* fp) {
	int status = 0;
	char buff[BUFFER_SIZE] = { 0 };
	ssize_t n;

	while ((n = recv(sockfd, buff, BUFFER_SIZE, 0)) > 0) {
		if (n < 0) {
			printf("Receive file status: failed\n");
			exit(0);
		}

		if (fwrite(buff, sizeof(char), n, fp) != n) {//fwrite
			//printf("Write file status: failed\n");
			status = -1;
			exit(0);
		}

		printf(buff);
		bzero(buff, BUFFER_SIZE);//clear buffer for new content
	}

	return status;
}

int main() {
	//create socket
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		printf("socket creation error: %s(errno: %d)\n", strerror(errno), errno);
		exit(0);
	}

	bzero((char*)& servaddr, sizeof(servaddr));

	//input IP adress
	char ipAdd[16];
	//printf("IP Address: ");
	scanf("%s", &ipAdd[0]);

	//assign IP address and port
	struct sockaddr_in servaddr, cli;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ipAdd);
	servaddr.sin_port = htons(portnum);
	
	//connect client socket to server socket
	int connection_status = connect(sockfd, (struct sockaddr*) & servaddr, sizeof(servaddr);
	printf("Connection status: ");
	if (connection_status < 0) {
		printf("failed\n");
		exit(0);
	}
	else {
		printf("IP Address: %s    Port Number: %d\n", ipAdd, portnum);
		printf("successful\n");
	}
	
	//send request
	char file_name[FILE_SIZE];
	char message[50];
	while (1) {
		bzero(file_name, sizeof(file_name));
		printf("Input the file name to be requested from the server:\n");
		scanf("%s", file_name);
		printf("Send status: ");
		if (write(sockfd, file_name, sizeof(file_name)) < 0) {
			printf("failed\n");
			exit(0);
		}
		else {
			printf("successful\n");
		}
		

		//read message, if any
		bzero(message, sizeof(message));
		read(sockfd, message, sizeof(message));
		if (message[0] != 0)
			printf("From server: %s", message);

		//create a file for storage
		printf("Open file status: ");
		FILE* fp = fopen(file_name, "w+");
		if (fp == NULL) {
			printf("failed\n");
			exit(0);
		}
		else
			printf("successful\n");
		
		//receive file
		printf("Received text: ");
		int status = writeFile(sockfd, fp);
		printf("Save text status: ");
		if (status < 0) {
			printf("successful\n");
			printf("Receive file %s from server successfully!\n");
		}
		else
			printf("failed\n");

		if (strncmp(file_name, "exit", 4) == 0) {
			break;
		}
	}

	//close socket
	fclose(fp);
	close(sockfd);
	return 0;
}