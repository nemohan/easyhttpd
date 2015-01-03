#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "easyhttp.h"
#include "easyerr.h"
#include "easypoll.h"
#include "easysched.h"
#include "easymem.h"
#include "easysta.h"
//#include "easyconnection.h"

#define BACKLOG 20
int serverfd;
int fds[MAXFD];
extern int errno;
extern sched_t *g_sched;
server_sta_t g_statistics;

void easy_add_client(sched_t *s,int fd)
{
	easy_connection_t *con;
	easy_connection_t *pcon = s->cons;
	while(pcon)
	{
	//	pthread_mutex_lock(&pcon->lock);
		if(CON_FREE == pcon->status)
		{
	//		pthread_mutex_unlock(&pcon->lock);
			break;
		}
	//	pthread_mutex_unlock(&pcon->lock);
		pcon = pcon->next;
		
	}
	if(pcon)
	{
		pcon->status = CON_PENDING;
		pcon->remotefd = fd;
		return;	
	}

	con = easy_create_con();
	con->remotefd = fd;
	con->status = CON_PENDING;
	if(!s->cons)
	{
		s->cons = con;
		return;
	}
	pcon = s->cons;	
	while(pcon->next)
	{
		pcon = pcon->next;
	}
	
	pcon->next = con;

}


void easy_start_server()
{
	struct sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(80);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(((serverfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	|| bind(serverfd,(struct sockaddr*)&addr,sizeof(addr)) < 0
	|| listen(serverfd,BACKLOG) < 0)
		easy_exit("Failed to start server");
	
	fcntl(serverfd,F_SETFL,O_NONBLOCK);

	
}

void easy_http_loop()
{
	static int freepos = 0;
	int epfd,ret;
	int cfd,i,j;
	struct sockaddr_in cliaddr;
	socklen_t len;
	sched_t *sched;
	int num = 0;
	struct epoll_event *events;

	epfd = easy_epoll_create(BACKLOG);
	easy_epoll_add(epfd,serverfd,
		EASY_EPOLL_IN,EASY_EPOLL_TRIGGER_DEFAULT);
	events = (struct epoll_event*)easy_malloc(BACKLOG * sizeof(*events));
	len = sizeof(cliaddr);
	memset(&cliaddr,0,sizeof(cliaddr));
	sched = g_sched;

	printf("server start......serverfd:%d\n",serverfd);
	
	while(1)
	{
		
		ret = easy_epoll_wait(epfd,events,BACKLOG,-1);
		if(ret < 0)
		{
			if(errno == EINTR)
				continue;
			else
				easy_exit("Failed to wait");
		}
		
		
		for(j = 0;j != ret;++j)
		{
		cfd = accept(serverfd,(struct sockaddr*)&cliaddr,&len);
		if(cfd < 0 && (EAGAIN == errno || EWOULDBLOCK == errno))  
			continue;
		else if(cfd < 0)
			return;

		fcntl(cfd,F_SETFL,O_NONBLOCK);
		printf("recv con cfd:%d ret:%d\n",cfd,ret);
		
		
		easy_add_client(sched,cfd);
		easy_epoll_add(sched->epfd,cfd,EASY_EPOLL_IN | EASY_EPOLL_OUT,EASY_EPOLL_TRIGGER_DEFAULT);		
						
		sched = (NULL == sched->next)?g_sched:sched->next;
		
		g_statistics.con++;
		}
	}
}


