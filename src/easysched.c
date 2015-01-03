#include <stdio.h>
#include "easysched.h"
#include "easymem.h"

sched_t *g_sched;

extern int g_trace;
sched_t *easy_get_sched()
{
	sched_t *p = g_sched;
	pthread_t tid = pthread_self();
	while(p)
	{
		if(pthread_equal(tid,p->tid))
			return p;
		p = p->next;	
	}
	
	return p;

}

void easy_trace_con(easy_connection_t *c)
{
	while(c)
	{
		//if(c->status == CON_FREE)
		{
			printf("\n************%s connections************\n",
				(c->status == CON_FREE?"free":(c->status == CON_PENDING?"wating":
				(c->status == CON_READING?"reading":"writing"))));
			printf("connection fd:%d\n",c->remotefd);
			printf("connection keep->alive:%d\n",c->keep_alive);
			if(c->inbuf)
			{
			printf("connection input buf:%d\n",c->inbuf);
			printf("connection input buf content:%c%c%c\n",c->inbuf[0],c->inbuf[1],c->inbuf[2]);
			}
			if(c->outbuf)
			{
			printf("connection output buf:%d\n",c->outbuf);
			printf("connection output buf content:%c%c%c\n",c->outbuf[0],c->outbuf[1],c->outbuf[2]);
			}
		}
		
		
		c = c->next;
		
		
	}
}

easy_connection_t *easy_get_con(sched_t *s,int fd,int status)
{
	easy_connection_t *c = s->cons;
	
	if(g_trace)
		easy_trace_con(c);
	while(c)
	{	//pthread_mutex_lock(&c->lock);
		if(fd == c->remotefd && (status & c->status))
		{
	//	pthread_mutex_unlock(&c->lock);
			return c;
		}
	//	pthread_mutex_unlock(&c->lock);
		c = c->next;
		
	}
	return c;
}

sched_t *easy_create_sched()
{
	sched_t *pnext;
	sched_t *sched = g_sched;
	sched = easy_malloc(sizeof(sched_t));
	easy_init_sched(sched);
	if(!g_sched)
	{
		g_sched = sched;	
		return sched;
	}

	pnext = g_sched;
	while(pnext->next)
	{
		pnext = pnext->next;

	}
	pnext->next = sched;
	return sched;
}

void easy_init_sched(sched_t *sched)
{

	int i = 0;
	for(;i < THREAD_CAPACITY;++i)
		sched->fds[i] =  -1;
}


