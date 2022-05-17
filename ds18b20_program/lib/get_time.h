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



#ifndef _get_d_time_H_
#define _get_d_time_H_

/*
 * =====================================================================================
 *         Name:  get_time
 *  Description:  get date and time now
 *   Input args:  dtime: date and time
 *  Output args:  date/time with (xxxx-xx-xx/xx:xx:xx)
 * return value:  NULL
 * =====================================================================================
 */

extern time_t get_dtime(char *dtime);


/* 
 * =====================================================================================
 *         Name:  check_timeout
 *  Description:  check time out or not
 *   Input args:
 *   				sample_time: time when sample
 *   				    timeout: how many seconds is timeout
 *
 * return value:  1: time not out    0: timeout
 * =====================================================================================
 */
extern int check_timeout(time_t sample_time, int timeout);

#endif

