/********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  client.h
 *    Description:  This head file is for client.
 *
 *        Version:  1.0.0(22/04/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "22/04/22 14:42:04"
 *                 
 ********************************************************************************/
#ifndef _HEAD_H_
#define _HEAD_H_

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
#include <libgen.h>
#include <netdb.h>
#include <dirent.h>
#include <fcntl.h>

#endif



#ifndef _TEMP_H_
#define _TEMP_H_

typedef struct _temp_msg
{
	char    serial_num[32];
	float   temp;
}temp_msg;

#endif

#ifndef _DATE_TIME_H_
#define _DATE_TIME_H_

typedef struct _get_d_time
{
	char 	date[32];
	char 	time[32];
}get_d_time;

#endif



#ifndef _CLIENT_H_
#define _CLIENT_H_

extern void get_temp_and_serialnum(temp_msg *packet);

extern void get_date_time(get_d_time *dt);

extern void sig_stop(int signum);
extern void print_usage(char *program);

extern int socket_client_init(char *IP, int port);
extern int re_connect(char *IP, int port);

extern void error_check(int fd, int cmp, char *str);
#endif

