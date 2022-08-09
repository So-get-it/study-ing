/********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  4g_dial.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(02/07/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "02/07/22 10:16:44"
 *                 
 ********************************************************************************/


#ifndef _4G_DIAL_H_
#define _4G_DIAL_H_

enum mode
{
	disable,
	enable
};

typedef struct p_lock_s
{
    int                     pppd_enabled;
    int                     eth0_flag;
    int                     wwan0_flag;
    int                     ppp0_flag;
    int                     metric;
    pthread_mutex_t         lock;
}p_lock_t;








#endif
