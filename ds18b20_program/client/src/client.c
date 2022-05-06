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
#include <sqlite3.h>
#include "sqlite_3.h"
#include "get_date_time.h"
#include "socket_client.h"
#include "ds18b20.h"

int run_stop = 0;

void sig_stop(int signum);
void print_usage(char *program);

int main(int argc, char *argv[])
{
	int 					i = 1;
	int 					rv = -1;
	int 					rec;
	int 					rc;
	int 					row;
	int 					max;
	int 					port = 0;
	int 					ch;
	int 					time;
	int 					client_fd = -1;
	int 					background = 0;
	char 					*sql;
	char 					str[32];
	char 					d_buf[32];
	char 			 		t_buf[8];
	char 					*host = NULL;
	char 					**pIP = NULL;
	char 					buf[1024];
	char 					msg_buf[128];
	char 					sql_msg_buf[128];
	sqlite3 				*db;

	struct sockaddr_in 		serv_addr;
	struct _temp_msg 		msg;
	struct _get_d_time 		dt;
	struct _temp_msg 		sql_msg;
	struct _get_d_time 		sql_dt;
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
				time = atoi(optarg);
				break;
			case 'b':
				background = 1;
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
	openlog("client", LOG_CONS | LOG_PID, 0);

	signal(SIGINT, sig_stop);
	signal(SIGTERM, sig_stop);
	signal(SIGPIPE, SIG_IGN);

	if(!port | !host | !time)
	{
		print_usage(argv[0]);
		return -1;
	}

	rc = sqlite3_open("get_temp.db", &db);
	if( rc )
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
		return -1;
	}

	sqlite_create_table(db);	//创建表

	while(!run_stop)
	{
		get_temp_and_serialnum(&msg);
		get_date_time(&dt);

		memset(msg_buf, 0, sizeof(msg_buf));
		snprintf(msg_buf, sizeof(msg_buf), "Serial number: %s ===Date-Time: %s/%s ===Temperature: %.2f", msg.serial_num, dt.date, dt.time, msg.temp);
		
		if(client_fd < 0)
		{
			client_fd = socket_client_init(host, port);
			
			row = sqlite_data_row(db); 	//读表中有多少组数据
			
			if(client_fd > 0) 	//如果重连成功
			{
				for(i = 0; i < row; i++) 	//如果有表中数据
				{
					get_sql_table_firstvalue(db, &sql_msg, &sql_dt);	//断线重连后如果表中有数据,获取第一组数据

					memset(sql_msg_buf, 0, sizeof(sql_msg_buf));
					snprintf(sql_msg_buf, sizeof(sql_msg_buf), "Serial number: %s ===Date-Time: %s/%s ===Temperature: %.2f", sql_msg.serial_num, sql_dt.date, sql_dt.time, sql_msg.temp);

					rec = SocketConnected(client_fd);
					rv = write(client_fd, sql_msg_buf, strlen(sql_msg_buf));
					if(rv > 0 | rec == 1) 	//如果发送成功
					{
						sqlite_delete(db, sql_dt.time);	//写完依据时间删除表数据
					}
					else
						break;
				}
			}
			syslog(LOG_NOTICE, "Program '%s' connect success,write the table to server OK!\n", __FILE__);

		}

		rec = SocketConnected(client_fd); 	//判断socket是否断开
		rv = write(client_fd, msg_buf, strlen(msg_buf));
		if(rv < 0 | rec == 0)
		{
			printf("write failure:%s\n",strerror(errno));

			syslog(LOG_WARNING, "Program '%s' maybe disconnected, insert the message into the table and try to reconnect...\n", __FILE__);
			
			sqlite_insert(db, msg.serial_num, dt.date, dt.time, msg.temp);	//写失败，记录到表中

			close(client_fd);
			client_fd = -1;
		}
		else
		{
			printf("write %d bytes data success: \n%s\n", rv, msg_buf);

			syslog(LOG_NOTICE, "Program '%s' write the Temperature message to server OK!\n", __FILE__);
		}
		
		sleep(time);
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
