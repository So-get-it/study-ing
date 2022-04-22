#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/resource.h>

void print_limits(char *name, int resource)
{
	struct rlimit limit;
	if(getrlimit(resource, &limit) != 0)
	{
		printf("getrlimit for %s failure: %s\n", name, strerror(errno));
		return;
	}
	
	printf("%-15s ", name);

	//常量RLIM_INFINITY指定了一个无限量的限制
	if(limit.rlim_cur == RLIM_INFINITY)
	{
		printf("(infinite)     ");
	}
	else
	{
		printf("%-15ld", limit.rlim_cur);
	}

	if(limit.rlim_max == RLIM_INFINITY)
	{
		printf("(infinite)     ");
	}
	else
	{
		printf("%-15ld", limit.rlim_max);
	}
	printf("\n");
}

int main(void)
{
	struct rlimit limit;

	//每个实际用户ID可拥有的最大子进程数
	print_limits("RLIMIT_NPROC", RLIMIT_NPROC);
	//数据段的最大字节长度
	print_limits("RLIMIT_DATA", RLIMIT_DATA);
	//栈的最大字节长度
	print_limits("RLIMIT_STACK", RLIMIT_STACK);
	//每个进程能打开的最大文件数
	print_limits("RLIMIT_NOFILE", RLIMIT_NOFILE);

	printf("\nAfter set RLIMIT_NOFILE: \n");
	getrlimit(RLIMIT_NOFILE, &limit );
	limit.rlim_cur = limit.rlim_max;
	setrlimit(RLIMIT_NOFILE, &limit );
	
	print_limits("RLIMIT_NOFILE", RLIMIT_NOFILE);

	return 0;
}
