#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFSIZE 1024
#define STR 	"Hello World\n"

int main(int argc, char *argv[])
{
	int 	fd = -1;
	int 	rv = -1;
	char 	buf[BUFSIZE];

	fd = open("test.txt", O_RDWR|O_CREAT|O_TRUNC,0666);
	if(fd < 0)
	{
		perror("Open file test.txt failure");
		return 0;
	}
	printf("Open file returned file descriptor [%d]\n", fd);

	if( (rv = write(fd,STR,strlen(STR))) < 0)
	{
		printf("Write %d bytes into file failure: %s\n", rv, strerror(errno));
		goto cleanup;
	}
	
	lseek(fd, 6, SEEK_SET);
	memset(buf, 0, sizeof(buf));

	if((rv = read(fd,buf,sizeof(buf)))<0)
	{
		printf("Read data from file failure: %s\n",strerror(errno));
		goto cleanup;
	}

	printf("Read %d bytes data from file: %s\n",rv,buf);

cleanup:
	close(fd);

	return 0;
}


