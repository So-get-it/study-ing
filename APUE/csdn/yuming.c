#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
 
int main(int argc, char **argv)
{
	char 		*ptr, **pptr;
	char   		IP[32];
	struct hostent 	*hptr;
	ptr = argv[1];

	if((hptr = gethostbyname(ptr)) == NULL)
	{
		printf("gethostbyname error for host:%s\n", ptr);
		return 0;
	}

	printf("official hostname:%s\n",hptr->h_name);
	for(pptr = hptr->h_aliases; *pptr != NULL; pptr++)
		printf("alias:%s\n",*pptr);

	switch(hptr->h_addrtype)
	{
		case AF_INET:
		case AF_INET6:
			pptr = hptr->h_addr_list;
			for(; *pptr != NULL; pptr++)
				printf("IP address:%s\n",inet_ntop(hptr->h_addrtype, *pptr, IP, sizeof(IP)));
			break;
		default:
	    		printf("unknown address type\n");
			break;
	}

	return 0;
}
