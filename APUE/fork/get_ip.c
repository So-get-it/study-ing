#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define TMP_FILE "/tmp/.ifconfig.log"

int main(int argc, char *argv[])
{
	int 		fd = -1;
	int 		rv = -1;
	FILE 		*fp;
	char 		buf[1024];
	char 		ipaddr[32];
	char 		*ptr;
	char 		*ip_head;
	char 		*ip_tail;
	pid_t 		pid;

	fd = open(TMP_FILE, O_RDWR|O_CREAT|O_TRUNC, 0666);
	if(fd < 0)
	{
		printf("open fail failure: %s\n", strerror(errno));
		return -1;
	}

	pid = fork();

	if(pid < 0)
	{
		printf("fork() fail...\n");
		return -2;
	}

	else if(pid == 0)
	{
		printf("child process strat excute ifconfig program\n");
		dup2(fd, STDOUT_FILENO);
		execl("/sbin/ifconfig", "ifconfig", "ens33", NULL);
		printf("If you see this tip, it means execl() error...\n");
		return -3;
	}

	else
	{
		sleep(3);
	}
	//子进程调用execl()函数会丢掉父进程的文本段,所以子进程不会执行到这里
	memset(buf, 0, sizeof(buf));
	lseek(fd, 0, SEEK_SET);
	rv = read(fd, buf, sizeof(buf));
	if(rv < 0)
	{
		printf("read file failure...\n");
		return -4;
	}
	printf("get buf:\n%s\n", buf);
	printf("read %d bytes data from ifconfig program.\n", rv);

	//fdopen()函数可以将文件描述符fd转成文件流fp
	fp = fdopen(fd, "r");
	//设置文件偏移量到文件头
	fseek(fp, 0, SEEK_SET);
	while(fgets(buf, sizeof(buf), fp) != NULL)
	{
		if(strstr(buf, "netmask"))
		{
			if(strstr(buf, "inet") != NULL)
			{
				ptr = strstr(buf, "inet");
			}
			
			ptr += strlen("inet");
	//isblank()函数用来检测一个字符是否是一个空白符
	//参数为要检测的字符的ASCII值
	//返回值为真表示为空白符，返回值为假（0）表示不是空白符
			while(isblank(*ptr))
			{
				ptr++;
			}
			ip_head = ptr;

			while(!isblank(*ptr))
			{
				ptr++;
			}
			ip_tail = ptr;
			memset(ipaddr, 0, sizeof(ipaddr));
			memcpy(ipaddr, ip_head, ip_tail-ip_head);
		}
	}

	printf("get IP address: %s\n", ipaddr);
	fclose(fp);
	unlink(TMP_FILE);
}
