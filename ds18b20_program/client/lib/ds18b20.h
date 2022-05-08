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




#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


#ifndef _DS18B20_H_
#define _DS18B20_H_

extern void get_temp_and_serialnum(char *number, float *temper);


#endif

