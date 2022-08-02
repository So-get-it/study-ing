/*********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  network.h
 *    Description:  This file is related to network.
 *
 *        Version:  1.0.0(15/07/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "15/07/22 17:07:16"
 *
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#ifndef _GET_NETSTAT_H_
#define _GET_NETSTAT_H_


#define LOG_NAME ".netstat.log"

extern int get_netstat(char *netname);

extern int check_netcard_exist(char *netname);

extern int get_min_metric (int *min_metric);

extern int switch_network_add (char *netname, int metric);

extern int switch_network_del (char *netname, int metric);

extern int get_working_netname (char *netname, int size, int metric);
#endif
