#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int 		g_var = 6;
char 		g_buf[] = "A string write stdout.\n";

int main(int argc, char *argv)
{
	int 		var = 88;
	pid_t 		pid;

	if(write(STDOUT_FILENO, g_buf, sizeof(g_buf)-1)<0)
	{
		printf("write string to stdout error:%s\n", strerror(errno));
		return -1;
	}

	printf("before fork\n");

	if((pid = fork()) < 0)
	{
		printf("fork() error: %s\n", strerror(errno));
		return -2;
	}

	else if(pid == 0)
	{
		printf("child process PID[%d] strat running...\n", getpid());
		g_var++;
		var++;
	}

	else
	{
		printf("parent process PID[%d] running...\n", getpid());
		sleep(1);
	}

	printf("PID = %ld, g_var = %d, var = %d\n", (long)getpid(), g_var, var);

	return 0;
}


