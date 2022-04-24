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

int s_stop = 0;

void sig_stop(int signum)
{
	s_stop = 1;
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
	char 					str[32];
	char 					d_buf[32];
	char 			 		t_buf[32];
	char                    *IP = NULL;
	char                    **pIP = NULL;
	char 					*domain = NULL;
	struct  sockaddr_in     serv_addr;
	char                    buf[1024];
	char 					msg_buf[128];
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

	client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(client_fd < 0)
	{
		printf("create client socket failure:%s\n", strerror(errno));
		return -1;
	}
	printf("create client socket[fd:%d] scuess\n",client_fd);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port   = htons(port);
	inet_aton(IP, &serv_addr.sin_addr);
/*
	if(connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("connect server [%s:%d] failure:%s\n",IP, port, strerror(errno));
		close(client_fd);
		return -1;
		//client_fd = re_connect(IP, port);
	}
	printf("connect server [%s:%d][client_fd:%d] scuess!\n",IP, port, client_fd);
*/
	while(!s_stop)
	{
		get_temp_and_serialnum(&msg);
//		printf("1\n");

		get_date_time(&dt);
//		printf("2\n");
		

		snprintf(msg_buf, sizeof(msg_buf), "Serial number: %s ===Date-Time: %s/%s ===Temperature: %.2f℃", msg.serial_num, dt.date, dt.time, msg.temp);
/*
		if(connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		{
			printf("connect server [%s:%d] failure:%s\n",IP, port, strerror(errno));
			close(client_fd);
			return -1;
			//client_fd = re_connect(IP, port);
		}
		printf("connect server [%s:%d][client_fd:%d] scuess!\n",IP, port, client_fd);
*/
		printf("running %d times...\n", i);
		i++;
		client_fd = socket_client_init(IP, port);
		rv = write(client_fd, msg_buf, strlen(msg_buf));
//		printf("rv return: %d\n", rv);
		if(rv < 0)
		{
			printf("write failure:%s\n",strerror(errno));
			close(client_fd);
		}
/*
		memset(buf, 0, sizeof(buf));
		if((rv = read(client_fd, buf, sizeof(buf))) < 0)
		{
			printf("write failure:%s\n",strerror(errno));
			close(client_fd);
		}
		else if(rv == 0)
		{
			printf("client connect to server get disconnectd\n");
			close(client_fd);
		}
		printf("read %d bytes data from server and echo it back:%s\n",rv, buf);
*/
		close(client_fd);
		sleep(time);
	}

}

