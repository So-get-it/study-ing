#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include "serial_port.h"

#define SERIAL_DEBUG

#ifdef SERIAL_DEBUG
#define serial_print(format, args...) printf(format,##args)
#else
#define serial_print(format, args...) do{} while(0)
#endif

void sig_stop(int signum);
void usage();

int run_stop = 0;

int main(int argc, char **argv)
{
    int 				fd, rv, ch;
    char 				*fname = "/dev/ttyUSB5"; // 设备节点
    char 				buf[64] = {0};
	char 				send_msg[128];
	char 				recv_msg[128];
	fd_set 				fdset;
	attr_t 				attr;
    struct termios 		oldtio;

    
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
        }
    }

    if((fd = serial_open(fname)) < 0)
    {  
        return -1;
    }

	if(serial_init(fd, &oldtio, &attr) < 0)
	{
		return -2;
	}

	signal(SIGINT, sig_stop);
	signal(SIGTERM, sig_stop);

    serial_print("c_iflag: %d\n", oldtio.c_iflag);
    serial_print("c_oflag: %d\n", oldtio.c_oflag);
    serial_print("c_cflag: %d\n", oldtio.c_cflag);
    serial_print("c_lflag: %d\n\n", oldtio.c_lflag);

    serial_print("c_ispeed: %d\n", oldtio.c_ispeed);
    serial_print("c_ospeed: %d\n\n", oldtio.c_ospeed);

	while(!run_stop)
    {
        FD_ZERO(&fdset);	//清空所有文件描述符
        FD_SET(STDIN_FILENO,&fdset);	//添加标准输入到fdset中
        FD_SET(fd,&fdset);				//添加文件描述符fd到fdset中

        /*  使用select多路复用监听标准输入和串口fd */
        rv = select(fd + 1, &fdset, NULL, NULL, NULL);
        if(rv < 0)
        {
            serial_print("Select failure......\n");
            break;
        }

        if(rv == 0)
        {
            serial_print("Time Out.\n");
            goto cleanup;
        }

		//有事件发生
        if(FD_ISSET(STDIN_FILENO,&fdset))
        {
            memset(send_msg, 0, sizeof(send_msg));

            /* 从标准输入读取命令 */
            fgets(send_msg, sizeof(send_msg), stdin);
            /* 处理要发送的数据 */
            //adjust_buf(send_msg);

			serial_print("The message will send: %s\n", send_msg);
            if(serial_send(fd, send_msg, strlen(send_msg)) < 0)
            {
                serial_print("Write failed.\n");
                goto cleanup;
            }

			serial_print("INFO:------------Write success!\n");
            //fflush(stdin);

        }

        if(FD_ISSET(fd,&fdset))
        {
            memset(recv_msg, 0, sizeof(recv_msg));

            rv = serial_recv(fd, recv_msg, sizeof(recv_msg));
            if(rv <= 0)
            {
                serial_print("Read failed: %s\n",strerror(errno));
                break;
            }

            serial_print("Receive %d bytes data: %s",rv, recv_msg);
            //fflush(stdout);
        }

		sleep(5);
    }
    
cleanup:
    serial_close(fd, &oldtio);

	return 0;
}

void sig_stop(int signum)
{
	serial_print("catch the signal: %d\n", signum);
	run_stop = 1;
}

void usage()
{
    serial_print("-h(--help    ): aply the usage of this file\n");
    serial_print("-f(--flowctrl): arguments: 0(no use) or 1(hard) or 2(soft)\n");
    serial_print("-b(--baudrate): arguments with speed number\n");
    serial_print("-d(--databits): arguments: 5 or 6 or 7 or 8 bits\n");
    serial_print("-p(--parity  ): arguments: n/N(null) e/E(even) o/O(odd) s/S(space)\n");
    serial_print("-s(--stopbits): arguments: 1 or 2 stopbits\n");
}
