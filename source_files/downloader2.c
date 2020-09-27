#include <stdio.h>
#include <string.h>

#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

//#include <windows.h>
//#include <WS2tcpip.h>

#define portnum 12345
#define FILE_SIZE 500 
#define BUFFER_SIZE 1024

int writeFile(int sockfd, FILE* fp) {
	int status = 0;
	char buff[BUFFER_SIZE] = { 0 };
	SSIZE_T n;

	while ((n = recv(sockfd, buff, BUFFER_SIZE, 0)) > 0) {
		if (n < 0) {
			printf("Receive file status: failed\n");
			status = -1;
			break;
		}

		if (fwrite(buff, sizeof(char), n, fp) != n) {//fwrite
			printf("Write file status: failed\n");
			status = -1;
			break;
		}

		printf(buff);
		memset(buff, 0, sizeof(buff));//clear buffer for new content		
	}

	return status;
}

int main() {
	WSADATA wsa;
	SOCKET sockfd;

	//initialise Winsock	
	printf("Initialise Winsock status: ");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("failed\n");
		return 1;
	}
	//else
	printf("sucessful\n");

	//create socket	
	printf("socket creation status: ");
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == INVALID_SOCKET)////////////////
		printf("failed\n");
	printf("successful\n");

	//input IP adress
	char ipAdd[16];
	printf("IP Address: ");
	scanf("%s", &ipAdd[0]);

	//assign IP address and port
	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ipAdd);
	servaddr.sin_port = htons(portnum);
	
	//connect client socket to server socket
	int connection_status = connect(sockfd, (struct sockaddr*) & servaddr, sizeof(servaddr));
	printf("Connection status: ");
	if (connection_status < 0) {
		//printf("failed\n");
		puts("failed\n");
		return 1;
	}
	//else
	printf("IP Address: %s    Port Number: %d\n", ipAdd, portnum);
	printf("successful\n");
	
	//send request
	char file_name[FILE_SIZE];
	char message[50];
	FILE* fp;

	while (1) {
		memset(file_name, 0, sizeof(file_name));
		printf("Input the file name to be requested from the server:\n");
		scanf("%s", file_name);
		printf("Send status: ");
		if (send(sockfd, file_name, sizeof(file_name), 0) < 0) {
			printf("failed\n");
			return 1;
		}
		//else
		printf("successful\n");

		//read message, in case of any errors
		memset(message, 0, sizeof(message));
		int recv_size = recv(sockfd, message, sizeof(message), 0);
		if (recv_size != SOCKET_ERROR && message[0] != 0) {
			message[recv_size] = '\0';
			printf("From server: %s\n", message);//put \0 if necessary
			continue;
		}
			

		//create a file for storage
		printf("Open file status: ");
		fp = fopen(file_name, "w+");
		if (fp == NULL) {
			printf("failed\n");
			exit(0);
		}
		//else
		printf("successful\n");
		
		//receive file
		printf("Received text: ");
		int status = writeFile(sockfd, fp);
		printf("Save text status: ");
		if (status >= 0) {
			printf("successful\n");
			printf("Receive file %s from server successfully!\n", file_name);
		}
		else
			printf("failed\n");

		if (strncmp(file_name, "exit", 4) == 0)
			break;
	}

	//terminate
	fclose(fp);
	closesocket(sockfd);
	WSACleanup();

	return 0;
}