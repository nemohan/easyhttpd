#include "easypoll.h"

int easy_epoll_create(int max)
{
	int efd;
	if((efd = epoll_create(max)) < 0)
	{

		easy_exit("Failed to create epoll");
	}

	return efd;
}

int easy_epoll_add(int efd,int fd,int event,int trigger)
{
	struct epoll_event e;
	int ret;
	e.data.fd = fd;
	e.events = event;
	if(EASY_EPOLL_TRIGGER_ET == trigger)
		e.events |= EPOLLET;
	if((ret = epoll_ctl(efd,EPOLL_CTL_ADD,fd,&e)) < 0)
		easy_exit("epoll_ctl failed");

	return ret;

}

int easy_epoll_modify(int epfd,int fd,int event,int trigger)
{
	struct epoll_event e;
	int ret;
	e.data.fd = fd;
	e.events = event;
	if(EASY_EPOLL_TRIGGER_ET == trigger)
		e.events |= EPOLLET;
	if(ret = epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&e))
		easy_exit("epoll modify failed");
	return ret;
}

int easy_epoll_del(int epfd,int fd)
{
	struct epoll_event e;
	return epoll_ctl(epfd,EPOLL_CTL_DEL,fd,&e);

}


int easy_epoll_wait(int efd,struct epoll_event *events,int max_event,int timeout)
{
	return epoll_wait(efd,events,max_event,timeout);
}
