#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>


#define MSG_STR "Hello, world! HELLO, UNIX!"

void print_usage(char *program)
{
	printf("%s usage: \n", program);
	printf("-i(--ipaddr): sepcify server IP address\n");
	printf("-p(--port): sepcify server port\n");
	printf("-d(--domain):sepcify server domain\n");
}

int main(int argc, char *argv[])
{
	int 			client_fd = -1;
	int 			rv = -1;
	int 			PORT = 0;
	int 			ch;
	char 			*IP = NULL;
	char 			**pIP = NULL;
	char 			buf[1024];
	char 			str[32];
	char 			*domain = NULL;
	struct 	sockaddr_in 	serv_addr;
	struct hostent 		*host = NULL;  //定义hostent结构体
	struct option 		opts[] = {
		{"ipaddr", required_argument, NULL, 'i'},
		{"port", required_argument, NULL, 'p'},
		{"domain", required_argument, NULL, 'd'},
		{NULL, 0, NULL, 0}
	};
	
	while((ch = getopt_long(argc, argv, "i:p:d:", opts, NULL)) != -1)
	{
		switch(ch)
		{
			case 'i':
				IP = optarg;
				break;
			case 'p':
				PORT = atoi(optarg);
				break;
			case 'd':
				domain = optarg;
				break;
			default:
				printf("Unknown return val: %d\n", ch);
		}
	}
	if(!PORT | !(!IP ^ !domain))
	{ 
		print_usage(argv[0]);
		return -1;
	}

	if(!IP)
	{
		if((host = gethostbyname(domain)) == NULL)
		{
			printf("gethostbyname error: %s\n", strerror(errno));
			return -1;
		}
		switch(host->h_addrtype)
	     	{
	     		case AF_INET:
	        		pIP = host->h_addr_list;
	        		for(; *pIP != NULL; pIP++)
	        			printf("IP address:%s\n", inet_ntop(host->h_addrtype, *pIP, str, sizeof(str)));
				IP = str;
	     	   		break;
	     		case AF_INET6:
	    		default:
				printf("unknown address type\n");
	        		break;
		}
	}
	printf("IP: %s\n", IP);
	client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(client_fd < 0)
	{
		printf("create client socket failure:%s\n", strerror(errno));
		return -1;
	}
	printf("create client socket[fd:%d] scuess\n",client_fd);
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port   = htons(PORT);	
	inet_aton(IP, &serv_addr.sin_addr);
	endhostent();	//释放结构体
	host = NULL;	//防止野指针
	if(connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("connect server [%s:%d] failure:%s\n", IP, PORT, strerror(errno));
		return -1;
	}
	printf("connect server [%s:%d] scuess!\n", IP, PORT);

	if(write(client_fd, MSG_STR, strlen(MSG_STR)) < 0)
	{
		printf("write failure:%s\n",strerror(errno));
		goto cleanup;
	}

	memset(buf, 0, sizeof(buf));
	if((rv = read(client_fd, buf, sizeof(buf))) < 0)
	{
		printf("write failure:%s\n",strerror(errno));
		goto cleanup;
	}
	else if(rv == 0)
	{
		printf("client connect to server get disconnectd\n");
		goto cleanup;
	}
	printf("read %d bytes data from server and echo it back:%s\n",rv, buf);

cleanup:
	close(client_fd);


}
