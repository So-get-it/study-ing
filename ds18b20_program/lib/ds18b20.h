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


#ifndef _SAMPLE_H_
#define _SAMPLE_H_

typedef struct _get_sample_msg
{
    char    serial_num[32];
    char    time[64];
    float   temp;
}sample_msg;

#endif


#ifndef _DS18B20_H_
#define _DS18B20_H_

/*
 * =====================================================================================
 *         Name:  get_temperature
 *  Description:  get temperature from ds18b20
 *   Input args:
 *                $temper: address for temperature
 * return value:  NULL
 * =====================================================================================
 */

extern int get_temperature(float *temper);


#endif

