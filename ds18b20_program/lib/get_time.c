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
#include "logger.h"



/* 
 * =====================================================================================
 *         Name:  get_dtime
 *  Description:  get date and time now
 *   Input args:  dtime: date and time
 *  Output args:  date/time with (xxxx-xx-xx/xx:xx:xx)
 * return value:  time_t t;how many sonecds
 * =====================================================================================
 */
time_t get_dtime(char *dtime)
{
	time_t 			t = time(NULL);
	struct tm	 	*p = localtime(&t);
	char 			t_buf[32];

	if(!dtime)
	{
		log_error("ERROR: Invalid input arguments\n");
		return -1;
	}
//	t_buf = (char *)malloc(32);
	memset(t_buf, 0, 32);
	snprintf(t_buf, 32, "%4d-%02d-%02d/%02d:%02d:%02d", p->tm_year+1900, p->tm_mon+1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);

	memset(dtime, 0, 32);
	strncpy(dtime, t_buf, strlen(t_buf));

//	free(t_buf);
	return t;
} 

/*
 * =====================================================================================
 *         Name:  check_timeout
 *  Description:  check time out or not
 *   Input args:
 *                              sample_time: time when sample
 *                                  timeout: how many seconds is timeout
 *
 * return value:  1: time not out    0: timeout
 * =====================================================================================
 */
int check_timeout(time_t time_last, int timeout)
{
	time_t 			t = time(NULL);

	if((t - time_last) >= timeout)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


