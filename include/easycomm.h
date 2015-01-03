#ifndef EASYCOMM_H_
#define EASYCOMM_H_

#include <signal.h>
#define SKIP_WS(p) \
{	\
	while(*p && *p == ' ')	\
		++p;		\
}	

typedef void(*easy_sa_handler)(int,siginfo_t*,void*);

void easy_catch_segment(int signo,siginfo_t *si,void *context);
void easy_setup_signal_handler(int signo,easy_sa_handler handler);
void easy_gettime(char *buf,int len);
#endif
