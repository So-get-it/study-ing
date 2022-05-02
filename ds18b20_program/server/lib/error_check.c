/*********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  error_check.c
 *    Description:  This file can check error.
 *                 
 *        Version:  1.0.0(24/04/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "24/04/22 11:33:19"
 *                 
 ********************************************************************************/



#include <stdio.h>
#include <string.h>
#include <errno.h>



void error_check (int fd, int cmp, char *str)
{
	if(fd < cmp)
	{
		printf("%s() failure: %s\n", str, strerror(errno));
	}
	return ;
} 

