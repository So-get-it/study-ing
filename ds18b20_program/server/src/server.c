/*********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  client.c
 *    Description:  This file is ds18b20_program's server.
 *                 
 *        Version:  1.0.0(22/04/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "22/04/22 14:39:33"
 *                 
 ********************************************************************************/

#include "server.h"

int run_stop = 0;

void sig_stop(int signum)
{
	printf("catch the signal: %d\n", signum);
	run_stop = 1;
}

int main(int argc, char *argv[])
{
	int                     port = 0;
	int                     ch;
	int                     i, j;
	int                     rv;
	int                     listen_fd, client_fd;
	int                     background = 0;
	int                     epollfd;
	int                     events;
	char                    *progname;
	char 					*ptr;
	char 					*serial_num;
	char 					*date;
	char 					*time;
	char 					*temp;
	char                    buf[256];
	char 					t_buf[8];
	temp_msg 				msg;
	get_d_time 				dt;
	struct sockaddr_in      cli_addr;
	struct epoll_event      event;
	struct epoll_event      event_test[MAX_EVENTS];
	struct epoll_event      event_array[MAX_EVENTS];

	struct option           opts[] = {
		{"daemon", required_argument, NULL, 'b'},
		{"port", required_argument, NULL, 'p'},
		{"help", required_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

	while((ch = getopt_long(argc, argv, "bp:h", opts, NULL)) != -1)
	{
		switch(ch)
		{
			case 'b':
				background = 1;
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

	if(1 == background)     //后台运行
    {
    	daemon(0, 0);
    }
	openlog("ds18b20_server", LOG_CONS | LOG_PID, 0);

	signal(SIGINT, sig_stop);
    signal(SIGTERM, sig_stop);

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
	
	sqlite_create_table();
	while(!run_stop)
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

				syslog(LOG_NOTICE, "Program '%s' accept a new client OK!\n", __FILE__);
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
				memset(t_buf, 0, sizeof(t_buf));

				read(event_array[i].data.fd, t_buf, sizeof(t_buf));
				send(event_array[i].data.fd, t_buf, 8, 0);
				rv = recv(event_array[i].data.fd, buf, sizeof(buf), 0);
				if(rv <= 0)
				{
					printf("recv() failure or client get disconnected: %s\n", strerror(errno));
					close(event_array[i].data.fd);
					continue;
				}

				printf("receive %d bytes data from client:\n%s\n", rv, buf);
				syslog(LOG_NOTICE, "Program '%s' receive some message from client OK!\n", __FILE__);
				printf("start to get memsage...\n");
				data_analysis(buf, &msg, &dt);
				sqlite_insert(msg.serial_num, dt.date, dt.time, msg.temp);
			}
		}
	}
	syslog(LOG_NOTICE, "Program '%s' stop running\n", __FILE__);
    closelog();

cleanup:
	close(listen_fd);

	return 0;
}


