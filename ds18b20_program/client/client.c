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
void packet_msg(char *buf_msg, int size, sample_msg_s msg);

int main(int argc, char *argv[])
{
	int 					i = 1;
	int 					rv = -1;
	int 					port;
	int 					ch;
	int 					sleep_time;
	int 					client_fd = -1;
	int 					background = 0;
	int 					sample_flag;
	int 					debug = 1;
	int 					loglevel = LOG_LEVEL_INFO;
	char 					*logfile = "client.log";
	char 					d_buf[32];
	char 					*host = NULL;
	char 					msg_buf[128];
	char 					sql_msg_buf[128];
	time_t 					time_last = 0;
	sqlite3 				*db;

	struct sockaddr_in 		serv_addr;
	struct sample_msg_t 	msg;
	struct sample_msg_t 	sql_msg;
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

	if(!port | !host | !sleep_time)
	{
		print_usage(argv[0]);
		return -1;
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

	if(1 == background)	//后台运行
	{
		log_info("daemon run...\n");
		if(daemon(1, 0) < 0)
		{
			printf("daemon() failure: %s\n", strerror(errno));
			return -1;
		}
	}

	signal(SIGINT, sig_stop);
	signal(SIGTERM, sig_stop);
	signal(SIGPIPE, SIG_IGN);

	sqlite_create_table("get_temp.db", &db);	//创建表
	memset(msg.serial_num, 0, sizeof(msg.serial_num));
	strncpy(msg.serial_num, "czy-001", strlen("czy-001"));

	while(!run_stop)
	{
		sample_flag = 0;
		if(check_timeout(time_last, sleep_time) > 0)
		{
			if(get_temperature(&msg.temp) < 0)
			{
				log_error("Sample failure: %s\n", strerror(errno));
			}
			else
			{
				log_info("Sample successfully!\n");
				sample_flag = 1; 	//已采样
			}
			time_last = get_dtime(msg.time); 	//采样时间

			packet_msg(msg_buf, sizeof(msg_buf), msg);
		}


		/*connect server*/
		if(client_fd < 0)
		{
			if((client_fd = socket_client_init(host, port)) > 0)
			{
				log_info("Program '%s' connect success!\n", __FILE__);
			}
		}

		/*check socket connected or not*/
		rv = SocketConnected(client_fd);
		if(rv == 0) 	//socket disconnected
		{
			if(client_fd > 0)
			{
				log_error("Program '%s' maybe disconnected, insert temp message into the table and try to reconnect...\n", __FILE__);
				socket_term(&client_fd);
			}
		}

		if(client_fd < 0)
		{
			if(sample_flag)
			{
				sqlite_insert(db, msg.serial_num, msg.time, msg.temp);	//写失败,记录到表中
			}

			sleep(1);
			continue;
		}

		if(sample_flag)
		{
			if(socket_write(client_fd, msg_buf) < 0)
			{
				log_error("Sample values write failure: %s\n", strerror(errno));

				socket_term(&client_fd);
			}
			else
			{
				log_info("Program '%s' write Sample message to server OK!\n", __FILE__);
			}
			log_debug("Write Sample Msg: \n%s\n", msg_buf);
		}

		if(sqlite_data_row(db) > 0)  //数据库中有数据
		{
			get_sql_table_firstvalue(db, msg.serial_num, msg.time, &msg.temp);	//获取第一组数据

			packet_msg(msg_buf, sizeof(msg_buf), msg);

			log_debug("table: %s\n", msg_buf);
			if(socket_write(client_fd, msg_buf) > 0) 	//如果发送成功
			{
				sqlite_delete(db, msg.time);	//写完依据时间删除表数据
			}
			else
			{
				socket_term(&client_fd);
				break;
			}
		}

		sleep(1);
	}

	close(client_fd);
	sqlite3_close(db);

	log_info("Program '%s' stop running\n", __FILE__);
	closelog();
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

void packet_msg(char *buf_msg, int size, sample_msg_s msg)
{
	memset(buf_msg, 0, size);
	snprintf(buf_msg, size, "Serial number: %s ===Date-Time: %s ===Temperature: %.2f", msg.serial_num, msg.time, msg.temp);

}
