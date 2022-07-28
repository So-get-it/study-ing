

#include "kill_process.h"
#include "logger.h"


int kill_process (const char *procs)
{
	int             retval, pid;
	FILE        	*ppid;
	char            buf[64] = {0};
	char            msg[16] = {0};


	snprintf(buf, sizeof(buf), "ps aux | grep %s | awk \'{print $2}\'", procs);

	log_debug("pid_get snprintf buf: %s\n", buf);

	fflush(stdout);

	if((ppid = popen(buf, "r")) == NULL)
	{
		log_error("%s popen() failure: %s\n", __func__, strerror(errno));
		return -1;
    }


	while(fgets(msg, sizeof(msg), ppid))
	{
		pid = atoi(msg);

		if(kill(pid, SIGTERM) < 0)
		{
			log_error("kill the process[%d] failure: %s\n", pid, strerror(errno));
			
			//pclose(ppid);
			//return -1;
		}
	}


	pclose(ppid);
	return 0;
} 

