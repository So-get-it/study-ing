/********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  socket_server_init.h
 *    Description:  This head file socket_server_init
 *
 *        Version:  1.0.0(06/05/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "06/05/22 17:06:40"
 *                 
 ********************************************************************************/

#ifndef _SOCKET_INIT_H_
#define _SOCKET_INIT_H_

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
#include <signal.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <libgen.h>
#include <netdb.h>
#include <dirent.h>
#include <fcntl.h>
#include <sqlite3.h>
#include <sys/epoll.h>

#define ARRAY_SIZE(x)   (sizeof(x)/sizeof(x[0]))
#define MAX_EVENTS      1024

#endif


extern int socket_server_init(char *IP, int port);

