
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#ifndef _GET_NETSTAT_H_
#define _GET_NETSTAT_H_


#define LOG_NAME ".netstat.log"

extern int get_netstat(char *netname);

extern int check_netcard_exist(char *netname);


#endif
