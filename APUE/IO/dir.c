#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define TEST_DIR "dir"

int main(int argc, char *argv)
{
	int 		rv;
	int 		fd1;
	int 		fd2;
	DIR 		*dirp;
	struct dirent 	*direntp;

	//创建文件夹dir，并设置权限为755
	if( mkdir(TEST_DIR, 0755)<0)
	{
		printf("creat directory '%s'failure:%s\n",TEST_DIR,strerror(errno));
		return -1;
	}

	//更改当前工作路径到文件夹dir下
	if(chdir(TEST_DIR)<0)
	{
		printf("Change directory to '%s' failure:%s\n",TEST_DIR, strerror(errno));
		rv = -2;
	}

	fd1 = creat("file1.txt",0644);
	fd2 = creat("file2.txt",0644);
	
	//更改当前工作路径到父目录
	chdir("../");
	dirp = opendir(TEST_DIR);
	
	while((direntp = readdir(dirp)) != NULL)
	{
		printf("Find file:%s\n",direntp->d_name);
	}

	closedir(dirp);

	return 0;
}
