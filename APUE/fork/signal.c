#include <stdio.h>
#include <signal.h>

int g_stop = 0;

void sig_handle(int signum)
{
	printf("Catch signal [%d]\n", signum);
//	g_stop = 1;
}

int main(int argc, char *argv[])
{
	signal(SIGINT, sig_handle);
	signal(SIGTERM, sig_handle);

	while(!g_stop)
	{
		;
	}

	printf("power off gprs\n");
}

