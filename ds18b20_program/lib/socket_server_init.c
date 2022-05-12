/*********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  socket_server_init.c
 *    Description:  This file is server_init.
 *                 
 *        Version:  1.0.0(30/04/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "30/04/22 22:05:12"
 *                 
 ********************************************************************************/


#include "socket_server_init.h"


int socket_server_init(char *ip, int port)
{
	struct  sockaddr_in     serv_addr;
	int                     listen_fd;
	int                     on = 1;

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
	else      //只监听指定的一个IP
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

