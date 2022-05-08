/********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  socket_client.h
 *    Description:  This head file is socket_client_init.h
 *
 *        Version:  1.0.0(04/05/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "04/05/22 19:11:52"
 *                 
 ********************************************************************************/

#ifndef _SOCKET_H_
#define _SOCKET_H_

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


#endif


/* 
 * =====================================================================================
 *         Name:  socket_client_init
 *  Description:  To get socket client fd
 *   Input args:  domain or IP, PORT
 *  Output args:  NULL
 * return value:  int fd
 * =====================================================================================
 */
extern int socket_client_init(char *domain_ip, int port);


/* 
 * =====================================================================================
 *         Name:  SocketConnected
 *  Description:  To judge socket disconnected or not.
 *   Input args:  socket_fd
 *  Output args:  NULL
 * return value:  1 (connected) or 0 (disconnected)
 * =====================================================================================
 */
extern int SocketConnected(int sock);

