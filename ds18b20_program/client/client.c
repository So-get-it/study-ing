/*********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  client.c
 *    Description:  This file is ds18b20_program's client.
 *                 
 *        Version:  1.0.0(22/04/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "22/04/22 14:39:33"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <syslog.h>
#include <libgen.h>
#include <netdb.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include <sqlite3.h>

#include "sqlite_func.h"
#include "get_time.h"
#include "socket_client.h"
#include "ds18b20.h"
#include "logger.h"


int run_stop = 0;

void sig_stop(int signum);
void print_usage(char *program);

int main(int argc, char *argv[])
{
	int 					i = 1;
	int 					rv = -1;
	int 					port;
	int 					ch;
	int 					sleep_time;
	int 					client_fd = -1;
	int 					background = 0;
	int 					sample_flag = 0;
	int 					debug = 1;
	int 					loglevel = LOG_LEVEL_INFO;
	char 					*logfile = "client.log";
	char 					d_buf[32];
	char 					*host = NULL;
	char 					msg_buf[128];
	char 					sql_msg_buf[128];
	time_t 					time_sample;
	time_t 					time_now;
	sqlite3 				*db;

	struct sockaddr_in 		serv_addr;
	struct _get_sample_msg 	msg;
	struct _get_sample_msg	sql_msg;
	struct option 			opts[] = {
		{"host", required_argument, NULL, 'h'},  //IP地址或域名
		{"port", required_argument, NULL, 'p'},
		{"time", required_argument, NULL, 't'},  //采样间隔时间
		{"daemon", no_argument, NULL, 'b'},      //后台运行与否
		{NULL, 0, NULL, 0}
	};

	while((ch = getopt_long(argc, argv, "h:p:t:b", opts, NULL)) != -1)
	{
		switch(ch)
		{
			case 'h':
				host = optarg;
				break;
			case 'p':
				port = atoi(optarg);
				break;
			case 't':
				sleep_time = atoi(optarg);
				break;
			case 'b':
				background = 1;
				debug = 0;
				break;
			default:
				printf("Unknown return val: %d\n", ch);
		}
	}

	if(1 == background)	//后台运行
	{
		printf("daemon run...\n");
		if(daemon(1, 0) < 0)
		{
			printf("daemon() failure: %s\n", strerror(errno));
			return -1;
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
        return 1;
    }

	signal(SIGINT, sig_stop);
	signal(SIGTERM, sig_stop);
	signal(SIGPIPE, SIG_IGN);

	if(!port | !host | !sleep_time)
	{
		print_usage(argv[0]);
		return -1;
	}

	sqlite_create_table("get_temp.db", &db);	//创建表
	memset(msg.serial_num, 0, sizeof(msg.serial_num));
	strncpy(msg.serial_num, "czy-001", strlen("czy-001"));

	while(!run_stop)
	{
		get_temperature(&msg.temp);
		time_sample = get_dtime(msg.time); 	//采样时间

		memset(msg_buf, 0, sizeof(msg_buf));
		snprintf(msg_buf, sizeof(msg_buf), "Serial number: %s ===Date-Time: %s ===Temperature: %.2f", msg.serial_num, msg.time, msg.temp);

		log_info("Sample successfully!\n");
		sample_flag = 1; 	//已采样


		/*first connect*/
		if(client_fd < 0)
		{
			if((client_fd = socket_client_init(host, port)) > 0)
			{
				log_info("Program '%s' connect success!\n", __FILE__);
			}
		}

		while( check_timeout(time_sample, sleep_time) ) 	//没有超过采样间隔时间
		{
			/*if disconnect*/
			if(client_fd < 0)
			{
				if((client_fd = socket_client_init(host, port)) > 0)
				{
					log_info("Program '%s' reconnect success,write the table to server OK!\n", __FILE__);
				}
			}

			if(client_fd > 0)
			{
				if(sample_flag) 	//有采样值
				{
					rv = SocketConnected(client_fd); 	//判断socket是否断开
					if(rv == 0) 	//socket断开
					{

						log_warn("Program '%s' maybe disconnected, insert temp message into the table and try to reconnect...\n", __FILE__);

						sqlite_insert(db, msg.serial_num, msg.time, msg.temp);	//socket断开,记录到表中

						close(client_fd);
						client_fd = -1;
					}
					else
					{
						rv = write(client_fd, msg_buf, strlen(msg_buf));
						if(rv < 0)
						{
							log_error("Sample values write failure:%s\n", strerror(errno));
							sqlite_insert(db, msg.serial_num, msg.time, msg.temp);	//写失败,记录到表中

							close(client_fd);
							client_fd = -1;
						}
						else
						{
							log_info("Program '%s' write Sample message to server OK!\n", __FILE__);
						}
						log_debug("Write Sample Msg: \n%s\n", msg_buf);
					}

					sample_flag = 0; 	//采样值已处理
				}

				if(!sample_flag) 	//无采样值
				{
					rv = sqlite_data_row(db); 	//读表中有没有数据
					if(rv > 0) 	
					{
						get_sql_table_firstvalue(db, sql_msg.serial_num, sql_msg.time, &sql_msg.temp);	//获取第一组数据

						memset(sql_msg_buf, 0, sizeof(sql_msg_buf));
						snprintf(sql_msg_buf, sizeof(sql_msg_buf), "Serial number: %s ===Date-Time: %s ===Temperature: %.2f", sql_msg.serial_num, sql_msg.time, sql_msg.temp);

						log_debug("table: %s\n", sql_msg_buf);
						rv = write(client_fd, sql_msg_buf, strlen(sql_msg_buf));
						if(rv > 0) 	//如果发送成功
						{
							sqlite_delete(db, sql_msg.time);	//写完依据时间删除表数据
						}
						else
						{
							close(client_fd);
							client_fd = -1;
							break;
						}
					}
				}
			}

			sleep(1);

		}
		if(sample_flag)
		{
			sqlite_insert(db, msg.serial_num, msg.time, msg.temp);	//网络一直处于断开状态，有采样值，记录到表中

			sample_flag = 0;
		}
	}

	if(1 == run_stop)	//想要结束进程时，关闭套接字、删除表、关闭日志系统
	{
		close(client_fd);
		sqlite3_close(db);

		syslog(LOG_NOTICE, "Program '%s' stop running\n", __FILE__);
		closelog();
	}

}


void sig_stop(int signum)
{
	printf("catch the signal: %d\n", signum);
	run_stop = 1;
}

void print_usage(char *program)
{
    printf("%s usage: \n", program);
    printf("-h(--host): sepcify server IP address or domain\n");
    printf("-p(--port): sepcify server port\n");
    printf("-t(--time): dwell time in second\n");
    printf("-b(--daemon): without arguments\n");
}
