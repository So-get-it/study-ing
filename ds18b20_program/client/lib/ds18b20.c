#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "client.h"


void get_temp_and_serialnum(temp_msg *packet)
{
	char 			buf[128];
	int 			fd;
	int 			rv = -1;
	temp_msg		*msg = packet;	
	float 			temp1;
	char 			path[64] = "/sys/bus/w1/devices/";
	char 			*temp = NULL;
	DIR 			*dirp;
	struct dirent 	*direntp;

	dirp = opendir(path);
	if(!dirp)
	{
		printf("open directary %s failure:%s", path, strerror(errno));
		//return -1;
	}
	
	memset(msg->serial_num, 0, 32);
	while(NULL != (direntp = readdir(dirp)))
	{
		if( strstr(direntp->d_name,"28-"))
		{
			strncpy(msg->serial_num,direntp->d_name,sizeof(msg->serial_num));
			//printf("%s: %s\n", direntp->d_name, msg->serial_num);
		}
		//printf("%s: %s\n", direntp->d_name, msg->serial_num);
	}

	closedir(dirp);
	
	strncat(path,msg->serial_num,sizeof(msg->serial_num));
	strncat(path,"/w1_slave",sizeof(path)-strlen("/w1_slave"));
	
	//printf("path: %s\n", path);
	fd = open(path, O_RDONLY);

	if(fd < 0)
	{
		printf("open directary file:%s\n",strerror(errno));
		//return -1;
	}

	memset(buf, 0, sizeof(buf));

	rv = read(fd, buf, sizeof(buf));
	if(rv <= 0)
	{
		printf("read something failure:%s\n",strerror(errno));
		//return -2;
	}
	temp = strstr(buf, "t=");
	temp = temp + 2;
	msg->temp = atof(temp)/1000;

	
	close(fd);
	//return msg;

}

