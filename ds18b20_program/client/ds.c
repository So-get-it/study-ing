#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include "client.h"

typedef struct _temp_msg
{
        char    *serial_num;
        char    *temp;
}temp_msg;


temp_msg 		msg;	
temp_msg get_temp_and_serialnum()
{
	char 			buf[128];
	int 			fd;
	int 			rv = -1;
	float 			temp1;
	char 			path[64] = "/sys/bus/w1/devices/";
	char 			d_path[32];
	DIR 			*dirp;
	struct dirent 	*direntp;

	dirp = opendir(path);
	if(!dirp)
	{
		printf("open directary %s failure:%s", path, strerror(errno));
		//return -1;
	}

	while(NULL != (direntp = readdir(dirp)))
	{
		if( strstr(direntp->d_name,"28-"))
		{
			strncpy(d_path,direntp->d_name,sizeof(d_path));
			msg.serial_num = d_path;
			//msg.serial_num = direntp->d_name;
			//printf("%s----serial: %s\n", direntp->d_name, msg.serial_num);
		}
		//printf("%s----d_path: %s\n", direntp->d_name, d_path);
	}
	printf("----d_path: %s\n", d_path);
	//printf("----serial: %s\n", msg.serial_num);

	closedir(dirp);
	
	strncat(path,d_path,sizeof(d_path));
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
	msg.temp = strstr(buf, "t=");
	msg.temp = msg.temp + 2;
	temp1 = atof(msg.temp)/1000;
	//printf("Temperature:%.2f\n",temp1);
	snprintf(msg.temp, 6, "%.2f", temp1);
	//printf("Temperature:%s\n",msg.temp);
	//printf("Temperature:%s\n",msg.serial_num);
	
	close(fd);
	return msg;

}


int main (int argc, char **argv)
{
	char 			msg_buf[256];
	//temp_msg 		msg;
	msg = get_temp_and_serialnum();

	printf("temp: %s\n", msg.temp);
	printf("serial: %s\n", msg.serial_num);
	
	snprintf(msg_buf, sizeof(msg_buf), "Serial number: %s", msg.serial_num);
	printf("msg_buf:%s\n", msg_buf);
	return 0;
} 

