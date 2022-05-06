/********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  data_analysis.h
 *    Description:  This head file is to analysis temperature message.
 *
 *        Version:  1.0.0(06/05/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "06/05/22 16:56:10"
 *                 
 ********************************************************************************/

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
	char    date[32];
	char    time[32];
}get_d_time;

#endif



extern void data_analysis(char *data, temp_msg *msg, get_d_time *dt);
