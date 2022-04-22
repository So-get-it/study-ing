#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int get_ipaddr(char *interface, char *ipaddr, int ip_size);

int main(int argc, char *argv[])
{
	char 		ipaddr[16];
	char 		*interface = "ens33";

	memset(ipaddr, 0, sizeof(ipaddr));
	if(get_ipaddr(interface, ipaddr, sizeof(ipaddr)) < 0)
	{
		printf("ERROR: get %s IP address failure.\n", interface);
		return -1;
	}
	printf("get %s IP address: %s\n", interface, ipaddr);
	

}

int get_ipaddr(char *interface, char *ipaddr, int ip_size)
{
	char 		buf[1024];
	FILE 		*fp;
	char 		*ptr;
	int 		rv;
	char 		*ip_start;
	char 		*ip_end;


	if(!interface | !ipaddr | (ip_size < 16))
	{
		printf("Invalid input arguments.\n");
		return -1;
	}

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "ifconfig %s", interface);
	if((fp = popen(buf, "r")) == NULL)
	{
		printf("popen() to excute command '%s' failure: %s\n", buf, strerror(errno));
		return -1;
	}
	rv = -1;
	while(fgets(buf, sizeof(buf), fp))
	{
		if(strstr(buf, "netmask"))
                {
                        if(strstr(buf, "inet") != NULL)
                        {
                                ptr = strstr(buf, "inet");
                        }

                        ptr += strlen("inet");
                        while(isblank(*ptr))
                        {
                                ptr++;
                        }
                        ip_start = ptr;

                        while(!isblank(*ptr))
                        {
                                ptr++;
                        }
                        ip_end = ptr;
                        memset(ipaddr, 0, sizeof(ipaddr));
                        memcpy(ipaddr, ip_start, ip_end-ip_start);
			rv = 0;
			break;
                }
        }
	pclose(fp);
	return rv;
}
