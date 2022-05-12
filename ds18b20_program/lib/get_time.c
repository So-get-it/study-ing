/*********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  date_time.c
 *    Description:  This file is to get date and time.
 *                 
 *        Version:  1.0.0(23/04/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "23/04/22 17:36:04"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "get_time.h"



/* 
 * =====================================================================================
 *         Name:  get_time
 *  Description:  get date and time now
 *   Input args:  dtime: date and time
 *  Output args:  date/time with (xxxx-xx-xx/xx:xx:xx)
 * return value:  NULL
 * =====================================================================================
 */
void get_time(char *dtime)
{
	time_t 			t = time(NULL);
	struct tm	 	*p = localtime(&t);
	char 			*t_buf = NULL;

	t_buf = (char *)malloc(32);
	snprintf(t_buf, 64, "%4d-%02d-%02d/%02d:%02d:%02d", p->tm_year+1900, p->tm_mon+1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);

	memset(dtime, 0, sizeof(dtime));

	strncpy(dtime, t_buf, strlen(t_buf));

	free(t_buf);
	return ;
} 

