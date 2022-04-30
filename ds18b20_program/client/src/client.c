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

#include "client.h"

int run_stop = 0;

void sig_stop(int signum)
{
	printf("catch the signal: %d\n", signum);
	run_stop = 1;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int main(int argc, char *argv[])
{
	int 					i = 1;
	int                     client_fd = -1;
	int                     rv = -1;
	int                     port = 0;
	int 					background = 0;
	int                     ch;
	int 					time;
	char 					*sql;
	char 					str[32];
	char 					d_buf[32];
	char 			 		t_buf[32];
	char                    *IP = NULL;
	char                    **pIP = NULL;
	char 					*domain = NULL;
	char                    buf[1024];
	char 					msg_buf[128];
	struct  sockaddr_in     serv_addr;
	struct hostent          *host = NULL;  //定义hostent结构体
	struct _temp_msg 		msg;
	struct _get_d_time 		dt;
	struct option           opts[] = {
		{"ipaddr", required_argument, NULL, 'i'},
		{"port", required_argument, NULL, 'p'},
		{"domain", required_argument, NULL, 'd'},//域名
		{"time", required_argument, NULL, 't'},//采样间隔时间
		{"daemon", no_argument, NULL, 'b'},//后台运行与否
		{NULL, 0, NULL, 0}
	};

	while((ch = getopt_long(argc, argv, "i:d:p:t:b", opts, NULL)) != -1)
	{
		switch(ch)
		{
			case 'i':
				IP = optarg;
				break;
			case 'd':
				domain = optarg;
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

	signal(SIGINT, sig_stop);
	signal(SIGTERM, sig_stop);
	signal(SIGPIPE, SIG_IGN);

	if(!port | !(!IP ^ !domain))
	{
		print_usage(argv[0]);
		return -1;
	}
	if(!IP)
	{
		if((host = gethostbyname(domain)) == NULL)
		{
			printf("gethostbyname error: %s\n", strerror(errno));
			return -1;
		}
		switch(host->h_addrtype)
		{
			case AF_INET:
			case AF_INET6:
				pIP = host->h_addr_list;
				for(; *pIP != NULL; pIP++)
					printf("IP address:%s\n", inet_ntop(host->h_addrtype, *pIP, str, sizeof(str)));
				IP = str;
				break;
			default:
				printf("unknown address type\n");
				break;
		}
	}

	if(1 == background)	//后台运行
	{
		daemon(0, 0);
	}
	

	client_fd = socket_client_init(IP, port);
	printf("connect server socket success: fd[%d]\n", client_fd);
	sqlite_create_table();	//创建表

	while(!run_stop)
	{
		get_temp_and_serialnum(&msg);
		get_date_time(&dt);

		snprintf(msg_buf, sizeof(msg_buf), "Serial number: %s ===Date-Time: %s/%s ===Temperature: %.2f℃", msg.serial_num, dt.date, dt.time, msg.temp);
		
		printf("running the %d time...\n", i);
		i++;

		if(i != 2)
		{
			if(rv <= 0)
			{
				client_fd = socket_client_init(IP, port);
				printf("connect server socket: fd[%d]\n", client_fd);
			}
		}
		rv = write(client_fd, msg_buf, strlen(msg_buf));
		
		if(rv < 0)
		{
			printf("write failure:%s\n",strerror(errno));
			close(client_fd);
//			sqlite_insert1();
			sqlite_insert(msg.serial_num, dt.date, dt.time, msg.temp);
		}
		else
		{
			printf("write %d bytes data success: %d\n", rv);
		}
		
		if(1 == run_stop)
		{
			sqlite_drop_table();	//删除表
		}
		sleep(time);
	}

}

