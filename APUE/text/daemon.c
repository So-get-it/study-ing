#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <libgen.h>


int main(int argc, char *argv[])
{
	char 	*progname = basename(argv[0]);
	if( daemon(0, 0) < 0)
	{
		printf("daemon() failure: %s\n", strerror(errno));
		return -1;
	}

	openlog("daemon", LOG_CONS | LOG_PID, 0);
	syslog(LOG_NOTICE, "program '%s' start running\n", progname);
	syslog(LOG_WARNING, "program '%s' running with a warning message\n", progname);
	syslog(LOG_EMERG, "program '%s' running with a emergency message\n", progname);

	while(1)
	{
		printf("hahaha...\n");
	}
	syslog(LOG_NOTICE, "Program '%s' stop running\n", progname);
	closelog();

	return 0;
}
