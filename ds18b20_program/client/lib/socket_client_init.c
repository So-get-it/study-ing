/*********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  socket_client_init.c
 *    Description:  This file is to get socket_fd.
 *                 
 *        Version:  1.0.0(24/04/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "24/04/22 09:06:01"
 *                 
 ********************************************************************************/


#include "client.h"


int socket_client_init (char *IP, int port)
{
	int 					rv = 0;
	int 					fd = -1;
	struct  sockaddr_in     serv_addr;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0)
	{
		printf("create client socket failure:%s\n", strerror(errno));
		return -1;
	}
	printf("create client socket[fd:%d] scuess\n",fd);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port   = htons(port);
	inet_aton(IP, &serv_addr.sin_addr);

	rv = connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	error_check(rv, 0, "connect");

//	rv = rv + 1;

	return fd;
} 

