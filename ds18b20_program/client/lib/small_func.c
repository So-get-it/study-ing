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

void print_usage(char *program)
{
        printf("%s usage: \n", program);
        printf("-i(--ipaddr): sepcify server IP address\n");
        printf("-p(--port): sepcify server port\n");
        printf("-t(--time): dwell time in second\n");
        printf("-d(--domain): sepcify server domain\n");
        printf("-b(--port): without arguments\n");
}


