#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
 
#define portnum 12345
#define FILE_SIZE 500 
#define BUFFER_SIZE 1024
 

void *client_fun(void * fd);
 
int main()
{
	int new_fd;
	pthread_t thread_id;
	int server_fd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==server_fd)
	{
		perror("socket");
		exit(1);
	}

	struct sockaddr_in server_addr;   
	server_addr.sin_family=AF_INET;   
	server_addr.sin_port=htons(portnum);  
	(server_addr.sin_addr).s_addr=htonl(INADDR_ANY);
	if(-1==bind(server_fd,(struct sockaddr *)&server_addr,sizeof(server_addr)))  
	{
		perror("bind");
		close(server_fd);
		exit(6);
	}

	if(-1==listen(server_fd,5)) 
	{
		perror("listen");
		close(server_fd);
		exit(7);
	}
	
	while(1)
	{
		struct sockaddr_in client_addr;
		int size=sizeof(client_addr);
		new_fd=accept(server_fd,(struct sockaddr *)&client_addr,&size);  
 
		if(new_fd < 0)
		{
			perror("accept");
			continue;      
		}
		//printf("accept client ip：%s:%d\n",inet_ntoa(client_addr.sin_addr),client_addr.sin_port);
		
		//printf("new_fd=%d\n",new_fd);
		if (new_fd > 0)
		{
			pthread_create(&thread_id, NULL, client_fun, (void *)&new_fd);  //创建线程
			pthread_detach(thread_id);
		}
		
	}
	close(server_fd);

	return 0;
}

void *client_fun(void *arg)
{
	int new_fd = *((int *)arg);
	int file2_fp;
	int len;


    char buffer[BUFFER_SIZE];
    memset( buffer,0, sizeof(buffer) );	

	while(1)
	{
			
		if((len=recv(new_fd, buffer, sizeof(buffer), 0)) <= 0)
		{
			continue;
		}

        char file_name[FILE_SIZE]; 
		memset( file_name,0, sizeof(file_name) );	
        strncpy(file_name, buffer, strlen(buffer)>FILE_SIZE?FILE_SIZE:strlen(buffer)); 
		memset( buffer,0, sizeof(buffer) );
        printf("Client requests file %s\n", file_name); 
		
		if( strcmp(file_name,"exit")==0 )
	    {
		   break;
	    }
		

         file2_fp = open(file_name,O_RDONLY,0777); 
         if(file2_fp<0) 
         { 
            printf("File %s Not Found\n", file_name);
            char* err_info = "File not found\n";
			if (write(new_fd, err_info, sizeof(err_info)) < 0)
			{
				printf("Send error information failed\n");
				break;
			}
            continue;
         } 
         else 
         { 
            int length = 0; 
			memset( buffer,0, sizeof(buffer) );

            while( (length = read(file2_fp, buffer, sizeof(buffer))) > 0  )    
            {   
                if( write(new_fd, buffer, length) < 0) 
                { 
                    printf("Send File %s Failed.\n", file_name); 
                    break; 
                } 
                memset( buffer,0, sizeof(buffer) );
            } 

             close(file2_fp); 
             printf("Transfer file %s successfully!\n", file_name);
         }   
	}
	close(new_fd);
 
}