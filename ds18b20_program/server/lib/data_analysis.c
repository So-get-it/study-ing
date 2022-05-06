/*********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  small_func.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(23/04/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "23/04/22 21:55:07"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include "data_analysis.h"


void data_analysis(char *data, temp_msg *msg, get_d_time *dt)
{
	char 		buf[8];
	char 		*ptr;
	char 		*head;
	char 		*tail;

	memset(msg->serial_num, 0, sizeof(msg->serial_num));
	memset(dt->date, 0, sizeof(dt->date));
	memset(dt->time, 0, sizeof(dt->time));

//	printf("data: %s\n", data);
//	get serial_number
	ptr = strstr(data, "number:");
	ptr += strlen("number:");
	while(isblank(*ptr))
	{
		ptr++;
	}
	head = ptr;

	while(!isblank(*ptr))
	{
		ptr++;
	}
	tail = ptr;

	memcpy(msg->serial_num, head, tail-head);

//	get date
	ptr = strstr(data, "Time:");
	ptr += strlen("Time:");
	while(isblank(*ptr))
	{
		ptr++;
	}
	head = ptr;

	while(*ptr != '/')
	{
		ptr++;
	}
	tail = ptr;

	memcpy(dt->date, head, tail-head);

//	get time
	ptr = strstr(data, "/");
	ptr += strlen("/");

	head = ptr;

	while(!isblank(*ptr))
	{
		ptr++;
	}
	tail = ptr;

	memcpy(dt->time, head, tail-head);

//	get temperature
	ptr = strstr(data, "ture:");
	ptr += strlen("ture:");
	while(isblank(*ptr))
	{
		ptr++;
	}
	head = ptr;
/* 
	while(isblank(*ptr) != '℃')
	{
		ptr++;
	}
	tail = ptr;
*/
	memset(buf, 0, sizeof(buf));
	memcpy(buf, head, 5);
	msg->temp = atof(buf);
}
