/*********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  at_cmd.c
 *    Description:  This file is related to AT command.
 *
 *        Version:  1.0.0(10/07/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "10/07/22 21:55:07"
 *
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "serial_port.h"
#include "at_cmd.h"
#include "logger.h"


/* 
 * =====================================================================================
 *         Name:  send_at_cmd
 *  Description:  Send the AT command and determine whether the received message meets expectations.
 *   Input args:  
 *              fd: File descriptor for serial files
 *              at_cmd: AT command string
 *              expect_recv: The message what you expect to receive.
 *              msgsize: the size of the receive buffer
 *
 *  Output args:
 *              recv_msg: A memory space to memory received message
 *
 * return value:  <0    failure
 *                =0    success
 * =====================================================================================
 */
int send_at_cmd (int fd, char *at_cmd, const char *expect_recv, char *recv_msg, int msgsize)
{
    int rv;
    



    if(!at_cmd || !expect_recv)
    {
        log_error("%s invalid parameter.\n", __func__);
        return -1;
    }


    rv = serial_send(fd, at_cmd, strlen(at_cmd));
    if(rv < 0)
    {
        log_error("%s send AT command failure: %s\n", __func__, strerror(errno));
        return -2;
    }


    memset(recv_msg, 0, msgsize);
    rv = serial_recv(fd, recv_msg, msgsize);
    if(rv < 0)
    {
        log_error("%s receive message failure: %s\n", __func__, strerror(errno));

        return -3;
    }


    if(!strstr(recv_msg, expect_recv))
    {
        log_error("Could not find the string you expected to receive!\n");

        return -4;
    }

    return 0;
} 



int check_serial_ready (int fd)
{
    int     retval;
    char    msg[32] = {0};

    retval = send_at_cmd(fd, "AT\r", "OK", msg, sizeof(msg));


    if(retval < 0)
    {
        log_error("%s function failure!\n", __func__);

        return retval;
    }

    return retval;
} 



int check_SIM_normal (int fd)
{
    int     retval;
    char    msg[32] = {0};

    retval = send_at_cmd(fd, "AT+CPIN?\r", "READY", msg, sizeof(msg));

    if(retval != 0)
    {
        //log_error("No SIM detected\n");
        log_error("%s function failure!\n", __func__);

        return retval;
    }

    return retval;
} 



int check_SIM_register (int fd)
{
    int         retval;
    char        msg[32] = {0};

    retval = send_at_cmd(fd, "AT+CREG?\r", "+CREG", msg, sizeof(msg));

    if(retval < 0)
    {
        //log_error("SIM Card is not registered\n");
        log_error("%s function failure!\n", __func__);

        return retval;
    }

    if(strstr(msg, "0,1") != NULL)
    {
        log_info("SIM card registered and belongs to the local network.\n");
    }
    else if(strstr(msg, "0,5") != NULL)
    {
        log_info("SIM card registered, roaming network\n");
    }


    return retval;
}


int check_SIM_signal (int fd)
{
    int             retval, signal_strength;
    char            *ptr;
    char            *phead;
    char            *ptail;
    char            signalnum[4] = {0};
    char            msg[64] = {0};
    
    retval = send_at_cmd(fd, "AT+CSQ\r", "+CSQ", msg, sizeof(msg));
    if(retval < 0)
    {
        //log_error("SIM signal strength is not well.\n");
        log_error("%s function failure!\n", __func__);

        return -1;
    }

    if(strstr(msg, "+CSQ:") != NULL)
    {
        ptr = strstr(msg, "+CSQ:");
        ptr += strlen("+CSQ:");

        while(isblank(*ptr))
        {
            ptr++;
        }
        phead = ptr;

        while(ptr[0] != 44)             // "," ASCII value is 44D/2CH
        {
            ptr++;
        }
        ptail = ptr;

        memcpy(signalnum, phead, ptail-phead);

        signal_strength = atoi(signalnum);
    }

    if(signal_strength < 8 || signal_strength == 99)
    {
        log_error("Signal Strengh is too low or no signal\n");
        return -2;
    }

    return 0;
} 



int check_all_right (int fd)
{
    if(check_serial_ready (fd) < 0)
        return -1;
    log_info("Serial port ready!\n");


    if(check_SIM_normal (fd) < 0)
        return -2;
    log_info("SIM card can be detected!\n");


    if(check_SIM_login (fd) < 0)
        return -3;
    log_info("SIM card is registered!\n");


    if(check_SIM_signal (fd) < 0)
        return -4;
    log_info("SIM card's signal strength is good\n");


    log_info("-------SIM card all ready!\n");

    return 0;
} 



/* 
 * =====================================================================================
 *         Name:  get_mcc_mnc
 *  Description:  To send AT command to get a number which include mcc number and mnc numer.
 *
 *   Input args:  fd: Serial port file descriptor
 *
 *  Output args:  char *mcc
 *                char *mnc
 * return value:  0    success
 *                -1   failure
 * =====================================================================================
 */
int get_mcc_mnc (int fd, char *mcc, char *mnc)
{
    int             rv, i;
    char            *ptr;
    char            *phead;
    char            *ptail;
    char            msg[512] = {0};

    rv = send_at_cmd(fd, "AT+QNWINFO\r", "+QNWINFO:", msg, sizeof(msg));
    if(rv < 0)
    {
        log_error("%s send AT command failure!\n", __func__);

        return -1;
    }
    else
    {
        ptr = strstr(msg, "\",\"");
        ptr += strlen("\",\"");

        while(isblank(*ptr))
        {
            ptr++;
        }
        phead = ptr;

        for(i = 0; i < 3; i++) 
        {
            ptr++;
        }
        ptail = ptr;

        memcpy(mcc, phead, ptail-phead);
        

        phead = ptail;
        for(i = 0; i < 2; i++) 
        {
            ptr++;
        }
        ptail = ptr;

        memcpy(mnc, phead, ptail-phead);
    }

    return 0;
} 




/* 
 * =====================================================================================
 *         Name:  set_apn
 *  Description:  Send an AT command to set APN(Access Point Name)
 *   Input args:  fd: Serial port file descriptor
 *                apn: an apn string
 *  Output args:  
 * return value:  <0   failure
 *                else  success
 * =====================================================================================
 */
int set_apn (int fd, const char *apn)
{
    int         retval;
    char        at_cmd[32] = {0};
    char        recv_msg[32] = {0};


    snprintf(at_cmd, sizeof(at_cmd), "AT+CGDCONT=1,\"IP\",\"%s\"\r", apn);

    retval = send_at_cmd(fd, at_cmd, "OK", recv_msg, sizeof(recv_msg));
    if(retval < 0)
    {
        log_error("%s function failure!\n", __func__);

        return retval;
    }

    log_debug("%s success.\n", __func__);
    return retval;
} 

