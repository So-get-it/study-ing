/********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  get_date_time.h
 *    Description:  This head file get_date_time
 *
 *        Version:  1.0.0(05/05/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "05/05/22 22:17:12"
 *                 
 ********************************************************************************/


#ifndef _DATE_TIME_H_
#define _DATE_TIME_H_

typedef struct _get_d_time
{
        char    date[32];
        char    time[32];
}get_d_time;

#endif


#ifndef _get_d_time_H_
#define _get_d_time_H_

extern void get_date_time(get_d_time *dt);

#endif

