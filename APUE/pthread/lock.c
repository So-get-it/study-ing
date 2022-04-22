#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

typedef struct worker_plock_s
{
	int 			shared_var;
	pthread_mutex_t 	lock;
}worker_plock_t;

void *thread_worker1(void *args);
void *thread_worker2(void *args);

int main(int argc, char *argv[])
{
	pthread_t 		tid;
	pthread_attr_t 		thread_attr;
	worker_plock_t 		worker_try;
	
	worker_try.shared_var = 1000;
	pthread_mutex_init(&worker_try.lock, NULL);

	if(pthread_attr_init(&thread_attr))
	{
		printf("pthread_attr_init() failure: %s\n", strerror(errno));
		return -1;
	}
	
	if(pthread_attr_setstacksize(&thread_attr, 20*1024))
	{
		printf("pthread_attr_setstacksize() failure: %s\n", strerror(errno));
		return -1;
	}


	if(pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED))
	{
		printf("pthread_attr_setdetachstate() failure: %s\n", strerror(errno));
		return -1;
	}

	pthread_create(&tid, &thread_attr, thread_worker1, &worker_try);
	printf("Thread worker1 tid[%ld] created ok\n", tid);
	
	pthread_create(&tid, NULL, thread_worker2, &worker_try);
	printf("Thread worker2 tid[%ld] created ok\n", tid);

	pthread_attr_destroy(&thread_attr);
	pthread_join(tid, NULL); 	

	while(1)
	{
		printf("Control thread shared_var: %d\n", worker_try.shared_var);
		sleep(5);
	}
}

void *thread_worker1(void *args)
{
	worker_plock_t 	*ptr = (worker_plock_t *)args;
	if(!args)
	{
		printf("%s() get invalid arguments\n", __FUNCTION__);
		pthread_exit(NULL);
	}

	printf("Thread worker1 [%ld] start running...\n", pthread_self());

	while(1)
	{	//阻塞锁
		pthread_mutex_lock(&ptr->lock);
		printf("+++: %s before shared_var++: %d\n", __FUNCTION__, ptr->shared_var);
		(ptr->shared_var)++;
		sleep(2);
		printf("+++: %s after shared_var++: %d\n", __FUNCTION__, ptr->shared_var);
		pthread_mutex_unlock(&ptr->lock);
		sleep(2);
	}
	printf("Thread worker1 exit...\n");

	return NULL;
}

void *thread_worker2(void *args)
{
	worker_plock_t 	*ptr = (worker_plock_t *)args;
	if(!args)
	{
		printf("%s() get invalid arguments\n", __FUNCTION__);
		pthread_exit(NULL);
	}

	printf("Thread worker2 [%ld] start running...\n", pthread_self());

	while(1)
	{	//非阻塞锁
		if(0 != pthread_mutex_trylock(&ptr->lock))
		{
			continue;
		}
		printf("---: %s before shared_var++: %d\n", __FUNCTION__, ptr->shared_var);
		(ptr->shared_var)++;
		sleep(2);
		printf("---: %s after shared_var++: %d\n", __FUNCTION__, ptr->shared_var);
		pthread_mutex_unlock(&ptr->lock);
		sleep(2);
	}
	printf("Thread worker2 exit...\n");

	return NULL;
}
