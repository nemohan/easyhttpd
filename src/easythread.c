#include <stdio.h>
#include <unistd.h>
#include "easythread.h"
#include "easypoll.h"
#include "easyhttp.h"
#include "easysched.h"
#include "easyrequest.h"
#include "easyerr.h"
#include "easycomm.h"
//#include "easyconnection.h"

pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;
/*
void easy_create_worker(worker_func worker,void *arg)
{
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetatchstate(&attr,PTHREAD_CREATE_DETACHED);
	if(pthread_create(&tid,&attr,worker,arg) < 0)
		easy_exit("Failed to create thread\n");

}
*/
int easy_spawn_workers(int capacity)
{
	int i,ret;
	sched_t *sched;
	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	for(i=0;i != capacity;++i)
	{
		sched = easy_create_sched();
		sched->epfd = easy_epoll_create(THREAD_CAPACITY);
		if((ret = pthread_create(&sched->tid,&attr,easy_worker,i)) < 0)
			break;
	}
	return ret;
}

void *easy_worker(void *arg)
{
	int fd,i,ret,connum,retval;
	struct epoll_event *events;
	sched_t *sched = easy_get_sched();
	easy_connection_t *pcon = NULL;
	int thid = (int)arg;
	
	easy_setup_signal_handler(SIGSEGV,easy_catch_segment);
	ret = 0;
	pthread_setspecific(g_conn_key,pcon);
	events = (struct epoll_event*)easy_malloc(sizeof(*events) * THREAD_CAPACITY);
	while(1)
	{	
			
		ret = easy_epoll_wait(sched->epfd,events,THREAD_CAPACITY,-1);
		if(ret < 0)
		{
			
			return NULL;
		}

		for( i=0; i != ret;++i)
		{
			sched = easy_get_sched();
			if(events[i].events & EASY_EPOLL_IN)
			{
				
				if(NULL != (pcon = easy_get_con(sched,events[i].data.fd,CON_PENDING)))
				{	
					printf("\nstart process new con:%d\n",pcon->remotefd);	
				//	pthread_mutex_lock(&pcon->lock);	
					pcon->status = CON_READING;
				//	pthread_mutex_unlock(&pcon->lock);
					if(0 == (retval = easy_process_request(pcon)))
					{
				//		pthread_mutex_lock(&pcon->lock);
						pcon->status = CON_WRITING;
				//		pthread_mutex_unlock(&pcon->lock);
					}
					else if(-2 == retval)
					{
						easy_close_con(pcon);
				//		pthread_mutex_lock(&pcon->lock);
						//pcon->status = CON_PENDING;
				//		pthread_mutex_unlock(&pcon->lock);
					}
				}
				
				
			}
		
			if(events[i].events & EASY_EPOLL_OUT)
			{
				if(NULL != (pcon = easy_get_con(sched,events[i].data.fd,CON_WRITING)))
					easy_process_response(pcon);
				/*
				easy_epoll_del(sched->epfd,pcon->remotefd);
				close(pcon->remotefd);
				pcon->remotefd = -1;
				*/
			}
			if(events[i].events & (EASY_EPOLL_HUP | EASY_EPOLL_ERR))
			{
				if(NULL != (pcon = easy_get_con(sched,events[i].data.fd,CON_STATUS)))
					easy_close_con(pcon);
			}
			
		}
	}
}

