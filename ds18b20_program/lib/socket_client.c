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

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/tcp.h>

#include "socket_client.h"
#include "logger.h"

/*
 * =====================================================================================
 *         Name:  socket_client_init
 *  Description:  To get socket client fd
 *   Input args:  
 *   				domain_ip: the hostname for client
 *   					 port: connect server port for client
 *
 * return value:  <0: failure 	>0:ok
 * =====================================================================================
 */
int socket_client_init (char *domain_ip, int port)
{
	int 					rv = 0;
	int 					fd = -1;
	char 					str[32];
	char 					**pIP = NULL;
	
	struct sockaddr_in 		serv_addr;
	struct hostent 			*host = NULL;

	if( inet_aton(domain_ip, &serv_addr.sin_addr) == 0 )
	{
		log_debug("--------域名解析...\n");

		if((host = gethostbyname(domain_ip)) == NULL)
		{
			log_error("gethostbyname error: %s\n", strerror(errno));
			return -1;
		}
		switch(host->h_addrtype)
		{
			case AF_INET:
			case AF_INET6:
				pIP = host->h_addr_list;
				for(; *pIP != NULL; pIP++)
					log_debug("IP address:%s\n", inet_ntop(host->h_addrtype, *pIP, str, sizeof(str)));
				domain_ip = str;
				break;
			default:
				log_warn("unknown address type\n");
				break;
		}
	}

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0)
	{
		log_error("create client socket failure:%s\n", strerror(errno));
		return -1;
	}
	log_info("create client socket[fd:%d] scuess\n",fd);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port   = htons(port);
	inet_aton(domain_ip, &serv_addr.sin_addr);

	rv = connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if(rv < 0)
	{
		log_error("connect() failure: %s\n", strerror(errno));
		close(fd);
		fd = -1;
	}

	return fd;
} 

/*
 * =====================================================================================
 *         Name:  SocketConnected
 *  Description:  To judge socket disconnected or not.
 *   Input args:  socket file description
 * return value:  1 (connected) or 0 (disconnected)
 * =====================================================================================
 */
int SocketConnected(int sock)
{
	if (sock <= 0)
		return 0;
	struct tcp_info info;
	int len = sizeof(info);

	getsockopt(sock, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *) & len);
	if ((info.tcpi_state == 1))
	{
		return 1;	//myprintf("socket connected\n");
	}
	else
	{
		return 0;	//myprintf("socket disconnected\n");
	}
}
