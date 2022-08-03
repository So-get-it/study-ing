/*********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  serial_port.h
 *    Description:  This file is related to serial port.
 *
 *        Version:  1.0.0(05/07/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "05/07/22 14:44:13"
 *
 ********************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>


#ifndef  _SERIALPORT_H_
#define  _SERIALPORT_H_

#define SERIALNAME_LEN 128

typedef struct attr_s{
	char 		*fname; 			//串口名称
	int 		fd;					//串口文件描述符
	int 		flow_ctrl;			//流控制
	int 		baud_rate;			//波特率
	int 		data_bits;			//数据位
	char 		parity;				//奇偶校验位
	int 		stop_bits;			//停止位
}attr_t;

extern int serial_open(attr_t *attr, struct termios *oldtermios);


extern int serial_close(int fd, struct termios *termios_p);


extern int serial_send(int fd, char *msg, int msg_len);


extern int serial_recv(int fd, char *recv_msg, int size);

#endif

