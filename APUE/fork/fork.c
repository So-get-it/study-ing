#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv)
{
	pid_t 		pid;
	pid = fork();
	if(pid < 0)
	{
		printf("fork() create child process failure:%s\n", strerror(errno));
		return -1;
	}

	else if(pid == 0)
	{
		printf("child process PID[%d] start running.My parent PID:%d\n",getpid(), getppid());
	}

	else 
	{
		printf("parent process PID[%d] continue running.and child process PID:%d\n",getpid(), pid);
		pid = wait(NULL);
		printf("PID: %d", pid);
	}

}
