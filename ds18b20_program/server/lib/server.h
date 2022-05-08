/********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  server.h
 *    Description:  This head file is server
 *
 *        Version:  1.0.0(08/05/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "08/05/22 09:27:01"
 *                 
 ********************************************************************************/


#ifndef _SERVER_H_
#define _SERVER_H_

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
#include <sqlite3.h>

#endif


#ifndef _SAMPLE_H_
#define _SAMPLE_H_

typedef struct _get_sample_msg
{
    char    serial_num[32];
    char    time[64];
    float   temp;
}sample_msg;

#endif

