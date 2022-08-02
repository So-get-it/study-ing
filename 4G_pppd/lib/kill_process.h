/*********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  kill_process.h
 *    Description:  This file is related to network.
 *
 *        Version:  1.0.0(15/07/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "15/07/22 22:05:34"
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
#include <signal.h>
#include <unistd.h>



#ifndef _GET_PID_H_
#define _GET_PID_H_

extern int kill_process (const char *procs);


extern int get_pid (const char *procs, char *msg);


#endif
