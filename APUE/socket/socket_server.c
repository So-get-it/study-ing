#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>


#define PORT 	5678
#define BACKLOG 100
#define BACK    "Receive your ask!"

int main(int argc, char *argv[])
{
	int 			listen_fd = -1;
	int 			client_fd = -1;
	int 			rv = -1;
	int 			on = 1;
	pid_t 			pid;
	struct 	sockaddr_in 	serv_addr;
	struct 	sockaddr_in 	cli_addr;
	socklen_t		addrlen = 64;
	char 			buf[1024];

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_fd < 0)
	{
		printf("craete socket failure:%s\n",strerror(errno));
		return -1;
	}

	printf("create socket[fd:%d] scuess\n",listen_fd);

	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port   = htons(PORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("bind port failure:%s\n", strerror(errno));
		return -1;
	}
	
	if(listen(listen_fd, BACKLOG) < 0)
	{
		printf("listen error:%s\n", strerror(errno));
		return -1;
	}

	while(1)
	{
		printf("\nwaiting new client connect...\n");
		client_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &addrlen);
		if(client_fd < 0)
		{
			printf("accept new socket failure:%s\n", strerror(errno));
			return -1;
		}
		printf("Accept new socket[%s:%d] success!\n",inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
		
		pid = fork();
		if(pid < 0)
		{
			printf("fork() failure: %s\n", strerror(errno));
			return -1;
		}

		else if(pid == 0)  //子进程
		{
			close(listen_fd);
			printf("child process start to communicate socket client...\n");
			memset(buf, 0, sizeof(buf));
			if((rv = read(client_fd, buf, sizeof(buf))) < 0)
			{
				printf("read data from client socket[%s] failure:\n",strerror(errno));
				close(client_fd);
				continue;
			}
			else if(rv == 0)
			{
				printf("client socket[%d] disconnected:\n",client_fd);
				close(client_fd);
				continue;
			}
			
			printf("read %d bytes data from client:%s\n", rv, buf);

			if(write(client_fd, BACK, strlen(BACK)) < 0 )
			{
				printf("write failure:%s\n",strerror(errno));
				return -1;
			}
			close(client_fd);
			break;
		}

		else    //父进程
		{
			close(client_fd);
			sleep(1);
		}
	}

	close(listen_fd);
}
