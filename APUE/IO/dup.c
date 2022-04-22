#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char *argv)
{
	int 	fd = -1;
	char 	*file_name = "t1.txt";

	fd = open(file_name,O_RDWR|O_CREAT|O_TRUNC,0666);
	if(fd < 0)
	{
		printf("Open file %s failure: %s\n", file_name, strerror(errno));
	}

	close(0);
	close(1);
	close(2);

	dup(fd);
	dup(fd);
	dup(fd);

	printf("fd = %d\n", fd);
	close(fd);

	return 0;
}
