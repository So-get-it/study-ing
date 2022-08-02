/*********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  4g_dial.c
 *    Description:  This file is to dialing up.
 *                 
 *        Version:  1.0.0(02/07/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "02/07/22 10:03:13"
 *                 
 ********************************************************************************/

#include <signal.h>
#include <getopt.h>
#include <pthread.h>
#include "serial_port.h"
#include "logger.h"
#include "parse_apn.h"
#include "kill_process.h"
#include "network.h"
#include "at_cmd.h"
#include "4g_dial.h"


//void *thread_pppd(void *args);
void *thread_ping(void *args);
void *thread_kill(void *args);


void sig_stop(int signum);
void usage();
void adjust_buf(char *buf);

int run_stop = 0;

int main(int argc, char **argv)
{
    int                 rv, ch;
    //char                buf[1024] = {0};
    int                 loglevel = LOG_LEVEL_INFO;
    int                 debug = 1;
    char                *logfile = "serial.log";
    //char                send_msg[128];
    //char                recv_msg[128];
    char                mcc[4] = {0};
    char                mnc[4] = {0};
    char                apn[16] = {0};
    //fd_set              fdset;
    attr_t              attr;
    struct termios      oldtio;
    pthread_t           tid1;
    //pthread_t           tid2;
    pthread_attr_t      thread_attr;
    p_lock_t            lock_ctx;
    

    lock_ctx.eth0_flag = enable;
    lock_ctx.wwan0_flag = disable;
    lock_ctx.ppp0_flag = disable;
    lock_ctx.pppd_enabled = disable;

    pthread_mutex_init(&lock_ctx.lock, NULL);

    
    attr.fname = "/dev/ttyUSB3"; //如果未指定，使用该设备节点
    
    struct option    opts[] = {
        {"help"    , no_argument      , NULL, 'h'},
        {"flowctrl", required_argument, NULL, 'f'},
        {"baudrate", required_argument, NULL, 'b'},
        {"databits", required_argument, NULL, 'd'},
        {"parity"  , required_argument, NULL, 'p'},
        {"stopbits", required_argument, NULL, 's'},
        {"name"    , required_argument, NULL, 'n'},
        {NULL      , 0                , NULL,  0 }
    };

    if(argc < 2)
    {
        log_error("WARN: without arguments!");
        usage();
        return -1;
    }

    while((ch = getopt_long(argc,argv,"hf:b:d:p:s:n:",opts,NULL)) != -1)
    {
        switch(ch)
        {
            case 'h':
                usage();
                return 0;

            case 'f':
                attr.flow_ctrl = atoi(optarg);
                break;

            case 'b':
                attr.baud_rate = atoi(optarg);
                break;

            case 'd':
                attr.data_bits = atoi(optarg);
                break;

            case 'p':
                attr.parity = *optarg;
                break;

            case 's':
                attr.stop_bits = atoi(optarg);
                break;
            
            case 'n':
                attr.fname = optarg;
                break;
        }
    }

    if(debug)
    {
        logfile = "stdout";
        loglevel = LOG_LEVEL_DEBUG;
    }


    if( logger_init(logfile, loglevel) < 0 )
    {
        fprintf(stderr, "Initial logger file '%s' failure: %s\n", logfile, strerror(errno));
        return -1;
    }


    if((attr.fd = serial_open(attr.fname)) < 0)
    {
        log_error("Open %s failure: %s\n", attr.fname, strerror(errno));
        return -2;
    }


    if(serial_init(&attr, &oldtio) < 0)
    {
        return -3;
    }


    signal(SIGINT, sig_stop);
    //signal(SIGTERM, sig_stop);


    get_min_metric(&lock_ctx.metric);


    /* Initialize a thread property object */
    if(pthread_attr_init(&thread_attr))
    {
        log_error("pthread_attr_init() failure: %s\n", strerror(errno));
        return -1;
    }


    /* Set the thread stack size */
    if(pthread_attr_setstacksize(&thread_attr, 20*1024))
    {
        log_error("pthread_attr_setstacksize() failure: %s\n", strerror(errno));
        return -1;
    }


    /* Sets the separate state property of the thread */
    if(pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED))
    {
        log_error("pthread_attr_setdetachstate() failure: %s\n", strerror(errno));
        return -1;
    }


    pthread_create(&tid1, &thread_attr, thread_ping, &lock_ctx);
    log_info("Thread_ping tid[%ld] created ok\n", tid1);

    pthread_create(&tid1, &thread_attr, thread_kill, &lock_ctx);
    log_info("Thread_kill tid[%ld] created ok\n", tid1);


    /* pppd call rasppp */
    while(!run_stop)
    {
        pthread_mutex_lock(&lock_ctx.lock); 	//lock up
        log_debug("Pppd call get lock and start running...\n");

        if(lock_ctx.pppd_enabled == enable)
        {
            //rv = check_netcard_exist("ppp0");
			//if(rv > 0)

            if(access("/sys/class/net/ppp0", F_OK))     //ppp0 not exist
            {
                /* Check AT command is available and the SIM card is normal */
                rv = check_all_right(attr.fd);
                if(rv < 0)
                {
                    log_error("check_all_right() check something error!!!");
                }


                /*----------- Set the APN------------ */

                rv = get_mcc_mnc(attr.fd, mcc, mnc);
                if(rv < 0)
                {
                    log_error("get_mcc_mnc() failure!!!");
                }


                rv = parse_apn(APNS_XML, mcc, mnc, apn);
                if(rv < 0)
                {
                    log_error("parse_apn failure!!!");
                }


                log_info("Get the apn: %s\n", apn);


                rv = set_apn(attr.fd, apn);
                if(rv < 0)
                {
                    log_error("set_apn failure!!!");
                }

                /* dial-up */
                system("sudo pppd call rasppp");

                log_info("\"sudo pppd call rasppp\" process start running\n");

            }
            /* ppp0 exist or pppd success*/

            //switch_network_add("ppp0", lock_ctx.metric);

            lock_ctx.ppp0_flag = enable;
            lock_ctx.eth0_flag = disable;
            lock_ctx.wwan0_flag = disable;

            lock_ctx.pppd_enabled = disable;

            log_info("ppp0 working...\n");
        }
		sleep(3);

        pthread_mutex_unlock(&lock_ctx.lock);
        log_debug("Pppd call get UNLOCK success...\n");

		sleep(1);
    }

    
    serial_close(attr.fd, &oldtio);

    log_info("Program '%s' stop running\n", __FILE__);

    logger_term();

    return 0;
}



void *thread_kill(void *args)
{
    //int   rate, rv;
    char                pid_s[8] = {0};

    p_lock_t *k_lock = (p_lock_t *)args;


    while(!run_stop)
    {
		pthread_mutex_lock(&k_lock->lock);  	//lock up
		log_debug("%s get lock and start running...\n", __func__);


		if(0 == k_lock->pppd_enabled)
		{
			/* wwan0 or ppp0 working */
			if(!k_lock->eth0_flag)
			{
				if(get_netstat("eth0") == 0)
				{
					log_info("\"eth0\" in good condition NOW!\n ");

					if(k_lock->ppp0_flag)   //if ppp0 is working
					{
						//switch_network_del("ppp0", k_lock->metric);

						log_info("ppp0 stop working...\n");

						k_lock->ppp0_flag = disable;
						k_lock->eth0_flag = enable;
					}

					if(k_lock->wwan0_flag)  //if wwan0 is working
					{
						//switch_network_del("wwan0", k_lock->metric);

						log_info("wwan0 stop working...\n");

						k_lock->wwan0_flag = disable;
						k_lock->eth0_flag = enable;
					}

					get_pid("pppd", pid_s);
					kill_process("pppd");
				}
			}


			/* ppp0 is working and eth0 isn't in good condition*/
			if(!k_lock->eth0_flag || !k_lock->wwan0_flag)
			{
				if(get_netstat("wwan0") == 0)
				{
					log_info("\"wwan0\" in good condition NOW!\n ");

					//switch_network_del("ppp0", k_lock->metric);
					//switch_network_add("wwan0", k_lock->metric);

					k_lock->ppp0_flag = disable;
					k_lock->wwan0_flag = enable;

					log_info("ppp0 stop working...\n");
				}
			}
		}
		pthread_mutex_unlock(&k_lock->lock);
		log_debug("%s get UNLOCK and stop running...\n", __func__);

		sleep(3);
    }

    pthread_exit(NULL);
}


void *thread_ping(void *args)
{
    int     rate;

    p_lock_t *p_lock = (p_lock_t *)args;


    while(!run_stop)
    {
        pthread_mutex_lock(&p_lock->lock);
        log_debug("%s get lock and start running...\n", __func__);


        if(p_lock->wwan0_flag == 1)
        {

            rate = get_netstat("wwan0");
            if(rate < 0)
            {
                log_error("Get \"wwan0\" network status failure!\n");
            }
            else if(rate == 0)
            {
                log_info("\"wwan0\" in good condition!\n ");
            }
            else
            {

                p_lock->pppd_enabled = enable;

                //switch_network_del("wwan0", p_lock->metric);
            }
        }


        if(p_lock->eth0_flag == enable)
        {
            //rate = get_netstat("eth0");
            rate = 100;
            if(rate < 0)
            {
                log_error("Get \"eth0\" network status failure!\n");
            }
            else if(rate == 0)
            {
                log_info("\"eth0\" in good condition!\n ");
            }
            else
            {
                log_info("\"eth0\" poor network!\n ");

                rate = get_netstat("wwan0");
                if(rate == 0)
                {
                    //switch_network_add("wwan0", p_lock->metric);

                    p_lock->wwan0_flag = enable;
                
                    log_info("\"wwan0\" start working!\n ");
                }
                else if(rate > 0)
                {
                    p_lock->pppd_enabled = enable;
                
                    log_info("\"pppd\" is enabled!\n ");
                }
                
                p_lock->eth0_flag = disable;
            }
        }


        pthread_mutex_unlock(&p_lock->lock);
        log_debug("%s get UNLOCK and stop running...\n", __func__);
        
        sleep(3);
    }

    pthread_exit(NULL);
}




void sig_stop(int signum)
{
    log_info("catch the signal: %d\n", signum);
    run_stop = 1;
}


void adjust_buf(char *buf)
{
    int i = strlen(buf);
    strcpy(&buf[i-1],"\r");
}


void usage()
{
    log_info("-h(--help    ): aply the usage of this file\n");
    log_info("-f(--flowctrl): arguments: 0(no use) or 1(hard) or 2(soft)\n");
    log_info("-b(--baudrate): arguments with speed number\n");
    log_info("-d(--databits): arguments: 5 or 6 or 7 or 8 bits\n");
    log_info("-p(--parity  ): arguments: n/N(null) e/E(even) o/O(odd) s/S(space)\n");
    log_info("-s(--stopbits): arguments: 1 or 2 stopbits\n");
    log_info("-n(--serial  ): arguments: the name of serial port\n");
}
