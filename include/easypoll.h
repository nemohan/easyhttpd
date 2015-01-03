#ifndef EPOLL_H_
#define EPOLL_H_

#include <sys/epoll.h>

#define EASY_EPOLL_ADD EPOLL_CTL_ADD
#define EASY_EPOLL_DEL EPOLL_CTL_DEL
#define EASY_EPOLL_MOD EPOLL_CTL_MOD

#define EASY_EPOLL_IN EPOLLIN
#define EASY_EPOLL_OUT EPOLLOUT
#define EASY_EPOLL_ERR EPOLLERR
#define EASY_EPOLL_HUP EPOLLHUP

#define EASY_EPOLL_TRIGGER_DEFAULT 0
#define EASY_EPOLL_TRIGGER_ET  EPOLLET

#define EASY_EPOLL_TIMEOUT 2000

int easy_epoll_create(int max);
int easy_epoll_wait(int efd,struct epoll_event *events,int max_event,int timeout);

int easy_epoll_add(int epfd,int fd,int event,int trigger);
int easy_epoll_del(int epfd,int fd);
int easy_epoll_modify(int epfd,int fd,int event,int trigger);



#endif
