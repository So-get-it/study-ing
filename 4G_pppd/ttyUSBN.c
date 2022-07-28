#include <signal.h>
#include <getopt.h>
#include <pthread.h>
#include "serial_port.h"
#include "logger.h"
#include "parse_apn.h"
#include "kill_process.h"
#include "get_network_status.h"
#include "at_cmd.h"



#define SERIAL_DEBUG

/*

#ifdef SERIAL_DEBUG
#define serial_print(format, args...) printf(format, ##args)
#else
#define serial_print(format, args...) do{} while(0)
#endif

*/


typedef struct p_lock_s
{
    int                     flag;
    pthread_mutex_t         lock;
}p_lock_t;


//void *thread_pppd(void *args);
void *thread_ping(void *args);
void *thread_kill(void *args);


void sig_stop(int signum);
void usage();
void adjust_buf(char *buf);

int run_stop = 0;

int main(int argc, char **argv)
{
	int	                rv, ch;
	//char                buf[1024] = {0};
	int 				loglevel = LOG_LEVEL_INFO;
	int 				debug = 1;
	char 				*logfile = "serial.log";
	//char                send_msg[128];
	//char                recv_msg[128];
	char 				mcc[4] = {0};
	char 				mnc[4] = {0};
	char 				apn[16] = {0};
	//fd_set              fdset;
	attr_t              attr;
	struct termios	    oldtio;
	pthread_t           tid1;
	//pthread_t           tid2;
    pthread_attr_t      thread_attr;
    p_lock_t      		lock_ctx;
	

	lock_ctx.flag = 0;
	pthread_mutex_init(&lock_ctx.lock, NULL);

	
	attr.fname = "/dev/ttyUSB3"; //如果未指定，使用该设备节点
	
	struct option	 opts[] = {
		{"help"    , no_argument	  , NULL, 'h'},
		{"flowctrl", required_argument, NULL, 'f'},
		{"baudrate", required_argument, NULL, 'b'},
		{"databits", required_argument, NULL, 'd'},
		{"parity"  , required_argument, NULL, 'p'},
		{"stopbits", required_argument, NULL, 's'},
		{"name"    , required_argument, NULL, 'n'},
		{NULL	   , 0				  , NULL,  0 }
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


	//创建第一个子线程，设置为相分离状态
    pthread_create(&tid1, &thread_attr, thread_ping, &lock_ctx);
    log_info("Thread worker1 tid[%ld] created ok\n", tid1);

	//创建第二个子线程，设置为相分离状态
    pthread_create(&tid1, &thread_attr, thread_kill, &lock_ctx);
    log_info("Thread worker1 tid[%ld] created ok\n", tid1);


	while(!run_stop)
	{
		pthread_mutex_lock(&lock_ctx.lock);

		log_debug("Pppd call get lock and start running...\n");
		
		if(lock_ctx.flag == 1)
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


			log_debug("The apn: %s\n", apn);


			rv = set_apn(attr.fd, apn);
			if(rv < 0)
			{
				log_error("set_apn failure!!!");
			}

			system("sudo pppd call rasppp");
			
			log_info("\"sudo pppd call rasppp\" process start running\n");

			lock_ctx.flag = 0;
		}

		pthread_mutex_unlock(&lock_ctx.lock);
		log_debug("Pppd call get UNLOCK success...\n");

		sleep(3);
	}
#if 0

	//创建第二个子线程
    pthread_create(&tid2, &thread_attr, thread_pppd, &lock_ctx);
    printf("Thread worker2 tid[%ld] created ok\n", tid2);

#endif


#if 0

	while(!run_stop)
	{
		FD_ZERO(&fdset);	//清空所有文件描述符
		FD_SET(STDIN_FILENO, &fdset);	//添加标准输入到fdset中
		FD_SET(attr.fd, &fdset);				//添加文件描述符fd到fdset中

		/*	使用select多路复用监听标准输入和串口fd */
		rv = select(attr.fd + 1, &fdset, NULL, NULL, NULL);
		if(rv < 0)
		{
			log_error("Select failure......\n");
			break;
		}

		if(rv == 0)
		{
			log_error("Time Out.\n");
			goto cleanup;
		}

		//有事件发生
		if(FD_ISSET(STDIN_FILENO,&fdset))
		{
			memset(send_msg, 0, sizeof(send_msg));

			/* 从标准输入读取命令 */
			fgets(send_msg, sizeof(send_msg), stdin);
			
			/* 把命令最后的\n转换成\r */
			adjust_buf(send_msg);

			if((rv = serial_send(attr.fd, send_msg, strlen(send_msg))) < 0)
			{
				log_error("Write failed.\n");
				goto cleanup;
			}


#ifndef SERIAL_DEBUG

			for(i = 0; i < rv; i++)
			{
				log_debug("Byte: %c\t ASCII: 0x%x\n", send_msg[i], (int)send_msg[i]);
			}

			log_debug("Write success!\n\n");
#endif

			//fflush(stdin);

		}

		if(FD_ISSET(attr.fd,&fdset))
		{
			memset(recv_msg, 0, sizeof(recv_msg));

			rv = read(attr.fd, recv_msg, sizeof(recv_msg));
			if(rv <= 0)
			{
				log_error("Read failed: %s\n",strerror(errno));
				break;
			}

			printf("%s", recv_msg);

#ifndef SERIAL_DEBUG
			log_debug("Receive %d bytes data: %s",rv, recv_msg);

			for(i = 0; i < rv; i++)
			{
				log_debug("Byte: %c\t ASCII: 0x%x\n", recv_msg[i], (int)recv_msg[i]);
			}
#endif

			//fflush(stdout);
		}

		//sleep(1);
	}

#endif
	
//cleanup:
	serial_close(attr.fd, &oldtio);

	log_info("Program '%s' stop running\n", __FILE__);

	logger_term();

	return 0;
}



void *thread_kill(void *args)
{
	int 	rate, rv;

	p_lock_t *kill = (p_lock_t *)args;


	while(!run_stop)
	{

		pthread_mutex_lock(&kill->lock);
		log_debug("%s get lock and start running...\n", __func__);

		rv = check_netcard_exist("ppp0");
		if(rv == 0)
		{
			log_debug("ppp0 exist or not(0-yes/1-no): %d\n", rv);
			
			rate = get_netstat("eth0");
			if(rate < 0)
			{
				log_error("Get \"eth0\" network status failure!\n");

				pthread_exit(NULL);
			}
			else if(rate == 0)
			{
				log_info("\"eth0\" in good condition!\n ");

				//get_pid("pppd");
				kill_process("pppd");
			}
			else
			{
				/*  The network card does not exist*/
				;
			}
		}

		pthread_mutex_unlock(&kill->lock);
		
		log_debug("%s get UNLOCK and stop running...\n", __func__);
		
		sleep(3);
	}

	pthread_exit(NULL);
}


void *thread_ping(void *args)
{
	int 	rate, rv;

	p_lock_t *ping = (p_lock_t *)args;


	while(!run_stop)
	{

		pthread_mutex_lock(&ping->lock);

		log_debug("%s get lock and start running...\n", __func__);

		rate = get_netstat("wwan0");
		if(rate < 0)
		{
			log_error("Get \"wwan0\" network status failure!\n");

			pthread_exit(NULL);
		}
		else if(rate == 0)
		{
			log_info("\"wwan0\" Network in good condition!\n ");
		}
		else
		{
			/*  The network card does not exist*/
			rv = check_netcard_exist("ppp0");

			log_debug("ppp0 exist or not(0-yes/1-no): %d\n", rv);

			if(rv == 1) 	//not exist
			{
				ping->flag = 1;
			}
		}


		pthread_mutex_unlock(&ping->lock);
		
		log_debug("%s get UNLOCK and stop running...\n", __func__);
		
		sleep(3);
	}

	pthread_exit(NULL);
}


#if 0

void *thread_pppd(void *args)
{
	p_lock_t *pppd= (p_lock_t *)args;

	while(!run_stop)
	{
		pthread_mutex_lock(&pppd->lock);

		if(pppd->flag == 1)
		{
			/* Check AT command is available and the SIM card is normal */

			check_all_right(pppd->fd);
			if(rv < 0)
			{
				log_error("check_all_right() check something error!!!");
			}


			/*----------- Set the APN------------ */

			get_mcc_mnc(pppd->fd, mcc, mnc);
			if(rv < 0)
			{
				log_error("get_mcc_mnc() failure!!!");
			}


			parse_apn(APNS_XML, mcc, mnc, apn);
			if(rv < 0)
			{
				log_error("parse_apn failure!!!");
			}


			log_debug("The apn: %s\n", apn);


			set_apn(pppd->fd, apn);
			if(rv < 0)
			{
				log_error("set_apn failure!!!");
			}

			system("sudo pppd call rasppp");

			pppd->flag = 0;
		}

		pthread_mutex_unlock(&pppd->lock);

		sleep(3);
	}
	pthread_exit(NULL);
}

#endif




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
