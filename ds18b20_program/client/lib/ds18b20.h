/********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  ds18b20.h
 *    Description:  This head file ds18b20
 *
 *        Version:  1.0.0(05/05/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "05/05/22 22:24:03"
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


#ifndef _DS18B20_H_
#define _DS18B20_H_

extern void get_temp_and_serialnum(temp_msg *packet);


#endif

