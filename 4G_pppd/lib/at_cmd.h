


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "logger.h"
#include "serial_port.h"



#ifndef _AT_CMD_H_
#define _AT_CMD_H_


extern int at_send (int fd, char *buf_AT);

extern int send_at_cmd (int fd, char *at_cmd, const char *expect_recv, char *recv_msg, int msgsize);

extern int check_serial_ready (int fd);

extern int check_SIM_normal (int fd);

extern int check_SIM_login (int fd);

extern int check_SIM_signal (int fd);

extern int check_all_right (int fd);

extern int get_mcc_mnc (int fd, char *mcc, char *mnc);

extern int set_apn(int fd, const char *apn);
#endif




