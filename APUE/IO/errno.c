#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char *argv)
{
	char 	*file_name = "/text.txt";
	int 	fd = -1;

	fd = open(file_name,O_RDONLY);
	if(fd < 0)
	{
		perror("Open file failure");
		printf("Open file %s failure:%s\n",file_name, strerror(errno));
	}
	
	close(fd);
	return 0;

}	
