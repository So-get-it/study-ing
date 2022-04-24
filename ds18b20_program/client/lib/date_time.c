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

#include <time.h>
#include "client.h"


void get_date_time(get_d_time *dtime)
{
	time_t 			t = time(NULL);
	struct tm	 	*p = localtime(&t);
	char 			*d_buf = NULL;
	char 			*t_buf = NULL;
	get_d_time 		*dt = dtime;

	d_buf = (char *)malloc(32);
	snprintf(d_buf, 32, "%4d-%02d-%02d", p->tm_year + 1900, p->tm_mon +1, p->tm_mday);
	t_buf = (char *)malloc(32);
	snprintf(t_buf, 32, "%02d:%02d:%02d", p->tm_hour, p->tm_min, p->tm_sec);

//	printf("Local time is: %s\n", asctime(p));

	memset(dt->date, 0, 32);
	memset(dt->time, 0, 32);

	strncpy(dt->date, d_buf, 32);
	strncpy(dt->time, t_buf, 32);

	free(d_buf);
	free(t_buf);
	return ;
} 









