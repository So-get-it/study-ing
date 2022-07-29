


#include "logger.h"
#include "network.h"

/* 
 * =====================================================================================
 *         Name:  get_netstat
 *  Description:  Obtain the network status of a network by the packet loss rate.
 *   Input args:  A network name
 *  Output args:  
 * return value:  =0		Network in good condition
 * 				  <0		Get status failure
 * 				  >0		The network status is poor or unavailable
 * =====================================================================================
 */
int get_netstat (char *netname)
{
	//int 		fd, rv;
	int 		rate;
	FILE 		*fp;
	char 		*ptr;
	char 		*phead;
	char 		*ptail;
	char 		status[4] = {0};
	char 		buf[256] = {0};
	//char 		msg[1024] = {0};
	

/*
	snprintf(buf, sizeof(buf), "ping baidu.com -I %s -c 3|grep loss > %s", netname, LOG_NAME);

	fd = open(LOG_NAME, O_RDWR|O_CREAT|O_TRUNC, 0666);
    if(fd < 0)
    {
        log_error("%s open fail failure: %s\n", __func__, strerror(errno));
        return -1;
    }

	system(buf);
	
	rv = read(fd, msg, sizeof(msg));
	if(rv < 0)
	{
        log_error("%s read netstat from file failure: %s\n", __func__, strerror(errno));

		return -1;
	}
*/

	snprintf(buf, sizeof(buf), "ping baidu.com -I %s -c 3", netname);

	if((fp = popen(buf, "r")) == NULL)
    {
    	log_error("popen() to excute command '%s' failure: %s\n", buf, strerror(errno));
        return -1;
    }


	while(fgets(buf, sizeof(buf), fp))
    {
        if(strstr(buf, "packets"))
        {
			if(strstr(buf, "errors") != NULL)
			{
				ptr = strstr(buf, "errors,");
				ptr += strlen("errors,");

				while(isblank(*ptr))
				{
					ptr++;
				}
				phead = ptr;

				while(ptr[0] != 37)		// "%" ASCII value is 37D/25H
				{
					ptr++;
				}
				ptail = ptr;

				memcpy(status, phead, ptail-phead);

				rate = atoi(status);
			}
			else if(strstr(buf, "received") != NULL)
			{
				ptr = strstr(buf, "received,");
				ptr += strlen("received,");

				while(isblank(*ptr))
				{
					ptr++;
				}
				phead = ptr;

				while(ptr[0] != 37)		// "%" ASCII value is 37D/25H
				{
					ptr++;
				}
				ptail = ptr;

				memcpy(status, phead, ptail-phead);

				rate = atoi(status);
			}
		}
	}

	pclose(fp);

	//unlink(LOG_NAME);

	return rate;
} 



/* 
 * =====================================================================================
 *         Name:  check_netcard_exist
 *  Description:  Check the network card exist or not.
 *   Input args:  The network name
 *  Output args:
 * return value:  <0    popen error
 *                =0    exist
 *                =1    not exist
 * =====================================================================================
 */
int check_netcard_exist (char *netname)
{

#if 1

	int 		retval;
	FILE        *fp;
	char 		buf[256] = {0};
	char 		msg[1024] = {0};


	snprintf(buf, sizeof(buf), "ifconfig %s", netname);
	
	log_debug("snprintf buf: %s\n", buf);

	fflush(stdout);

	if((fp = popen(buf, "r")) == NULL)
    {
    	log_error("popen() to excute command '%s' failure: %s\n", buf, strerror(errno));
        return -1;
    }


	while(fgets(msg, sizeof(msg), fp))
    {
		log_debug("Network buf: %s\n", msg);

		if(strstr(msg, "mtu"))
		{
			log_debug("Check network card exist!\n");

			pclose(fp);
			return 0;	//exist
		}
	}
	
	log_debug("Check network card not exist!\n");
	log_debug("Network buf Outside: 0x%x\n", msg[0]);

	pclose(fp);
	return 1;

#endif


#if 0	
	int 		retval, fd, rv;
	char 		buf[32] = {0};
	char 		msg[128] = {0};


	snprintf(buf, sizeof(buf), "ifconfig %s > %s", netname, LOG_NAME);
	//snprintf(buf, sizeof(buf), "ping baidu.com -I %s -c 3|grep loss > %s", netname, LOG_NAME);

	fd = open(LOG_NAME, O_RDWR|O_CREAT|O_TRUNC, 0666);
    if(fd < 0)
    {
        log_error("%s open fail failure: %s\n", __func__, strerror(errno));
        return -1;
    }

	system(buf);
	
	rv = read(fd, msg, sizeof(msg));
	if(rv < 0)
	{
        log_error("%s read netstat from file failure: %s\n", __func__, strerror(errno));

		return -1;
	}

	if(strstr(msg, "mtu"))
	{
		log_debug("Network card is exist\n");

		close(fd);
		unlink(LOG_NAME);

		return 0;		//exist
	}
	else
	{
		log_debug("Check network card not exist!\n");

		close(fd);
		unlink(LOG_NAME);

		return 1;		//not exist
	}

#endif

} 




int get_min_metric (int *min_metric)
{
	int 		got_metric;
	FILE        *fp;
	char 		*ptr;
	char 		*phead;
	char 		*ptail;
	char 		metric[8] = {0};
	char 		msg[256] = {0};

	*min_metric = 9999;

	if((fp = popen("ip route", "r")) == NULL)
    {
    	log_error("%s popen() failure: %s\n", __func__, strerror(errno));
        return -1;
    }


	while(fgets(msg, sizeof(msg), fp))
    {
		log_debug("IP ROUTE buf: %s\n", msg);

		if(strstr(msg, "metric"))
		{
			log_debug("msg: %s\n", msg);

			ptr = strstr(msg, "metric");

			ptr += strlen("metric");

			while(isblank(*ptr))
			{
				ptr++;
			}
			phead = ptr;

			while(!isblank(*ptr))
			{
				ptr++;
			}
			ptail = ptr;

			snprintf(metric, sizeof(metric), phead, ptail-phead);

			got_metric = atoi(metric);

			if(got_metric < *min_metric)
			{
				*min_metric = got_metric;
			}
			
		}
	}
	
	(*min_metric)--;

	pclose(fp);
	return 0;
} 




int switch_network_add (char *netname, int metric)
{
	char 		buf[128] = {0};


	snprintf(buf, sizeof(buf), "sudo ip route add default dev %s  metric %d", netname, metric);

	system(buf);

	return 0;
} 



int switch_network_del (char *netname, int metric)
{
	char 		buf[128] = {0};


	snprintf(buf, sizeof(buf), "sudo ip route del default dev %s  metric %d", netname, metric);

	system(buf);

	return 0;
} 
