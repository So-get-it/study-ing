/*********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  kill_process.c
 *    Description:  This file is related to network.
 *
 *        Version:  1.0.0(15/07/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "15/07/22 22:02:16"
 *
 ********************************************************************************/


#include "kill_process.h"
#include "logger.h"


/* 
 * =====================================================================================
 *         Name:  kill_process
 *  Description:  Find the process number of the process associated with the parameter and terminate
 *   Input args:  procs: The string associated with the process
 *  Output args:
 * return value:  0    success
 *                -1   failure
 * =====================================================================================
 */
int kill_process (const char *procs)
{
    int             retval, pid;
    FILE            *ppid;
    char            buf[64] = {0};
    char            msg[16] = {0};
    char            cmd[32] = {0};


    snprintf(buf, sizeof(buf), "ps -C %s | grep %s | awk \'{print $1}\'", procs, procs);

    log_debug("pid_get snprintf buf: %s\n", buf);

    fflush(stdout);

    if((ppid = popen(buf, "r")) == NULL)
    {
        log_error("%s popen() failure: %s\n", __func__, strerror(errno));
        return -1;
    }


    while(fgets(msg, sizeof(msg), ppid))
    {
#if 1

        snprintf(cmd, sizeof(cmd), "sudo kill -15 %s", msg);
        log_info("command: %s\n", cmd);

        system(cmd);

#endif

#if 0
        pid = atoi(msg);

        if(kill(pid, SIGTERM) < 0)
        {
            log_error("kill the process[%d] failure: %s\n", pid, strerror(errno));
            
            //pclose(ppid);
            //return -1;
        }

#endif
    }


    pclose(ppid);
    return 0;
} 



int get_pid (const char *procs, char *msg)
{
    int             retval, pid;
    FILE            *ppid;
    char            buf[64] = {0};
    //char            msg[128] = {0};


    snprintf(buf, sizeof(buf), "ps -C %s | grep %s | awk \'{print $1}\'", procs, procs);

    log_debug("pid_get snprintf buf: %s\n", buf);

    if((ppid = popen(buf, "r")) == NULL)
    {
        log_error("%s popen() failure: %s\n", __func__, strerror(errno));
        return -1;
    }


    while(fgets(msg, sizeof(msg), ppid))
    {
        //pid = atoi(msg);
        log_info("PID: %s\n", msg);



#if 0
        if(kill(pid, SIGTERM) < 0)
        {
            log_error("kill the process[%d] failure: %s\n", pid, strerror(errno));
            
            //pclose(ppid);
            //return -1;
        }

#endif
    }


    pclose(ppid);
    return 0;
    return 0;
} 

