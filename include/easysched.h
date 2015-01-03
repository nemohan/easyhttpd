#ifndef EASYSCHED_H_
#define EASYSCHED_H_

#include "easyrequest.h"
#include <pthread.h>

#define THREAD_CAPACITY 20

typedef struct sched_tag
{
	pthread_t tid;
	int fds[THREAD_CAPACITY];
	struct sched_tag *next;
	int epfd;
	easy_connection_t *cons;
//	pthread_mutex_t mlock;
}sched_t;

extern sched_t *g_sched;

typedef sched_t easy_sched_t;

easy_connection_t *easy_get_con(sched_t *s,int fd,int status);
sched_t *easy_get_sched();
sched_t* easy_create_sched();
void easy_init_sched(sched_t *sched);
#endif
