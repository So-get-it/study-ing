


#include "ds18b20.h"


void get_temp_and_serialnum(char *number, float *temper)
{
	char 			buf[128];
	char 			n_buf[32];
	int 			fd;
	int 			rv = -1;
	float 			temp1;
	char 			path[64] = "/sys/bus/w1/devices/";
	char 			*temp = NULL;
	DIR 			*dirp;
	struct dirent 	*direntp;

	dirp = opendir(path);
	if(!dirp)
	{
		printf("open directary %s failure:%s", path, strerror(errno));
		return ;
	}
	
	memset(n_buf, 0, sizeof(n_buf));
	while(NULL != (direntp = readdir(dirp)))
	{
		if( strstr(direntp->d_name,"28-"))
		{
			strncpy(n_buf,direntp->d_name,sizeof(n_buf));
		}
	}

	closedir(dirp);
	
	strncat(path,n_buf,sizeof(n_buf));
	strncat(path,"/w1_slave",sizeof(path)-strlen("/w1_slave"));
	
	fd = open(path, O_RDONLY);

	if(fd < 0)
	{
		printf("open directary file:%s\n",strerror(errno));
		return ;
	}

	memset(buf, 0, sizeof(buf));

	rv = read(fd, buf, sizeof(buf));
	if(rv <= 0)
	{
		printf("read something failure:%s\n",strerror(errno));
		return ;
	}
	temp = strstr(buf, "t=");
	temp = temp + 2;
	*temper = atof(temp)/1000;
	
	memset(number, 0, sizeof(number));
	strncpy(number, "czy-001", strlen("czy-001"));

	close(fd);

	return ;
}

