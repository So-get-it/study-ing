#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <getopt.h>
#include <libgen.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/epoll.h>

#define ARRAY_SIZE(x) 	(sizeof(x)/sizeof(x[0]))
#define MAX_EVENTS 	1024

int socket_server_init(char *ip, int port);
void print_usage(char *program)
{
        printf("%s usage:\n", program);
        printf("-d(--daemon): no argument \n");
        printf("-p(--port  ): sepcify argument port\n");
        printf("-h(--help  ): no argument\n");
}

int main(int argc, char *argv[])
{
        int                     port = 0;
        int                     ch;
	int 			i, j;
	int 			rv;
	int 			listen_fd, client_fd;
	int 			daemon;
	int 			epollfd;
	int 			events;
        char                    *IP = NULL;
        char                    *progname;
	char 			buf[256];
	struct sockaddr_in 	cli_addr;
	struct epoll_event 		event;
	struct epoll_event 		event_test[MAX_EVENTS];
	struct epoll_event 		event_array[MAX_EVENTS];

        struct option           opts[] = {
                {"daemon", required_argument, NULL, 'd'},
                {"port", required_argument, NULL, 'p'},
                {"help", required_argument, NULL, 'h'},
                {NULL, 0, NULL, 0}
        };

        while((ch = getopt_long(argc, argv, "dp:h", opts, NULL)) != -1)
        {
                switch(ch)
                {
                        case 'd':
                                daemon = 1;
                                break;
                        case 'p':
                                port = atoi(optarg);
                                break;
			case 'h':
                		print_usage(progname);
				break;
                        default:
                                printf("Unknown return val: %d\n", ch);
                }
        }

        progname = basename(argv[0]);

        if(!port)
        {
                print_usage(progname);
                return -1;
        }

	if((listen_fd = socket_server_init(NULL, port)) < 0)
	{
		printf("ERROR: %s server listen port[%d] failure.\n", progname, port);
		goto cleanup;
	}

	if((epollfd = epoll_create(MAX_EVENTS)) < 0)
	{
		printf("epoll_create() failure: %s\n", strerror(errno));
		return -1;
	}

	printf("epollfd: %d\n", epollfd);
	event.data.fd = listen_fd;
	event.events = EPOLLIN | EPOLLET;

	if(epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_fd, &event) < 0)
	{
		printf("epoll_ctl() failure: %s\n", strerror(errno));
		return -2;
	}

	while(1)
	{
		if((events = epoll_wait(epollfd, event_array, MAX_EVENTS, -1)) < 0)
		{
			printf("epoll_wait() failure: %s\n", strerror(errno));
			break;
		}
		else if(0 == events)
		{
			printf("epoll_wait() get timeout...(it's impossible)\n");
			continue;
		}
		//else events > 0
		for(i = 0; i < events; i++)
		{	
			if((event_array[i].events & EPOLLERR) || (event_array[i].events & EPOLLHUP))
			{
				printf("epoll_wait get error on fd[%d]: %s\n", event_array[i].data.fd, strerror(errno));
				epoll_ctl(epollfd, EPOLL_CTL_ADD, event_array[i].data.fd, &event);
				close(event_array[i].data.fd);
			}
			if(event_array[i].data.fd == listen_fd)
			{
				if((client_fd = accept(listen_fd, (struct sockaddr*)NULL, NULL)) < 0)
				{
					printf("accept new client failure: %s\n", strerror(errno));
					continue;
				}

				printf("client_fd: %d\n", client_fd);
				event.data.fd = client_fd;
				event.events = EPOLLIN|EPOLLET;	
				if(epoll_ctl(epollfd, EPOLL_CTL_ADD, client_fd, &event) < 0)
				{
					printf("epoll add client socket failure: %s\n", strerror(errno));
					close(event_array[i].data.fd);
					continue;
				}
				printf("epoll add new client socket[%d] ok.\n", client_fd);
			}
			else
			{
				memset(buf, 0, sizeof(buf));
				rv = read(event_array[i].data.fd, buf, sizeof(buf));
				if(rv <= 0)
				{
					printf("read() failure or client get disconnected: %s\n", strerror(errno));
					close(event_array[i].data.fd);
					continue;
				}

				printf("read %d bytes data from client: %s\n", rv, buf);
/* 
				for(j = 0; j < rv; j++)
				{
					buf[j] = toupper(buf[j]);
				}

				if(write(event_array[i].data.fd, buf, rv) < 0)
				{
					printf("write failure: %s\n", strerror(errno));
					close(event_array[i].data.fd);
				}
				printf("write back to client: %s\n", buf);
*/
			}
		}
	}
cleanup:
	close(listen_fd);

	return 0;
}


int socket_server_init(char *ip, int port)
{
	struct 	sockaddr_in 	serv_addr;
	int 			listen_fd;
	int 			on = 1;

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_fd < 0)
	{
		printf("create socket failure: %s\n", strerror(errno));
		return -1;
	}

	printf("create socket[fd:%d] scuess\n", listen_fd);

	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	
	if(!ip)
	{
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else  		//只监听指定的一个IP
	{
		inet_pton(AF_INET, ip, &serv_addr.sin_addr);
	}

	if(bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("bind() port failure: %s\n", strerror(errno));
		return -1;
	}

	if(listen(listen_fd, 13) < 0)
	{
		printf("listen error: %s\n", strerror(errno));
		return -1;
	}

	return listen_fd;
}
