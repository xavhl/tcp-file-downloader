#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")

#define portnum 12345
#define FILE_SIZE 500 
#define BUFFER_SIZE 1024

int writeFile(int sockfd, FILE* fp);

int main() {
	WSADATA wsa;
	SOCKET sockfd;

	//initialise WinSock	
	printf("Initialise WinSock status: ");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("failed\n");
		return 1;
	}
	printf("sucessful\n");//else

	//create socket
	printf("Socket creation status: ");
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == INVALID_SOCKET) {
		printf("failed\n");
		return 1;
	}
	printf("successful\n");

	//input IP address
	char ipAdd[16];
	printf("Input IP Address: ");
	scanf("%s", &ipAdd[0]);

	//assign IP address and port
	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ipAdd);
	servaddr.sin_port = htons(portnum);
	
	//connect client socket to server socket
	int connection_status = connect(sockfd, (struct sockaddr*) & servaddr, sizeof(servaddr));
	
	if (connection_status < 0) {
		puts("Connection status: failed\n");
		return 1;
	}
	printf("IP Address: %s    Port Number: %d\n", ipAdd, portnum);
	printf("Connection status: successful\n");
	
	//send request
	char file_name[FILE_SIZE];

	while (1) {
		memset(file_name, 0, sizeof(file_name));
		printf("Input the file name to be requested from the server:\n");
		scanf("%s", file_name);

		printf("Send status: ");
		if (send(sockfd, file_name, sizeof(file_name), 0) < 0) {
			printf("failed\n");
			return 1;
		}
		printf("successful\n");

		if (strncmp(file_name, "exit", 4) == 0)
			break;

		//create a file for storage
		printf("Open file status: ");
		FILE* fp = fopen(file_name, "w+");
		if (fp == NULL) {
			printf("failed\n");
			exit(0);
		}
		printf("successful\n");
		
		//receive file
		printf("Received text: ");
		int status = writeFile(sockfd, fp);
		fclose(fp);//save file

		printf("\nSave text status: ");
		if (status >= 0) {
			printf("successful\n");
			printf("Receive file %s from server successfully!\n", file_name);
		}
		else
			printf("failed\n");
	}

	//terminate
	closesocket(sockfd);
	WSACleanup();

	return 0;
}

int writeFile(int sockfd, FILE* fp) {
	int status = 0;//normal status
	char buff[BUFFER_SIZE];
	int n;

	u_long iMode = 1;//switch to non-blocking to avoid infinite while loop by recv()
	ioctlsocket(sockfd, FIONBIO, &iMode);

	while ((n = recv(sockfd, buff, BUFFER_SIZE, 0)) > 0) {
		if (n == SOCKET_ERROR) {
			printf("\nReceive file status: failed\n");
			status = -1;//error status
			break;
		}

		if (strlen(buff) == 0)//check empty string and finish transfer
			break;

		if (fwrite(buff, sizeof(char), n, fp) != n) {//write file
			printf("Write file status: failed\n");
			status = -1;
			break;
		}

		printf("%s", buff);//result
		memset(buff, 0, BUFFER_SIZE);//clear buffer for new content
	}

	return status;
}