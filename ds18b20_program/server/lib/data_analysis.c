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

#include "data_analysis.h"


void data_analysis(char *data, char *num, char *time, float *temp)
{
	char 		buf[8];
	char 		*ptr;
	char 		*head;
	char 		*tail;

	memset(num, 0, sizeof(num));
	memset(time, 0, sizeof(time));

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

	memcpy(num, head, tail-head);

//	get time
	ptr = strstr(data, "Time:");
	ptr += strlen("Time:");

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

	memcpy(time, head, tail-head);

//	get temperature
	ptr = strstr(data, "ture:");
	ptr += strlen("ture:");
	while(isblank(*ptr))
	{
		ptr++;
	}
	head = ptr;

	memset(buf, 0, sizeof(buf));
	memcpy(buf, head, 5);
	*temp = atof(buf);
}
