#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
	struct stat stbuf;

	stat("t1.txt", &stbuf);
	printf("file mode: %o  real size: %luB  space size:%luB\n",
			stbuf.st_mode,stbuf.st_size,stbuf.st_blksize);

	return 0;
}
