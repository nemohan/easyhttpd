#include "easycomm.h"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>

#define SIZE 1024
void *buffer[SIZE];

void easy_gettime(char *buf,int len)
{
	struct tm *ptm;
	time_t t = time(NULL);
	ptm = gmtime(&t);
	strftime(buf,len,"%a,%d %b %G %T GMT",ptm);

}

void easy_catch_segment(int signo,siginfo_t *si,void *context)
{
	int i,num;
	char **calls;
	printf("\nFault address:%X\n",si->si_addr);
	num = backtrace(buffer,SIZE);
	calls = backtrace_symbols(buffer,num);
	printf("%d\n",num);
	for(i=0;i != num;++i)
		printf("%s\n",calls[i]);
	abort();

}
void easy_setup_signal_handler(int signo,easy_sa_handler handler)
{
	struct sigaction act;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = handler;
	sigaction(signo,&act,NULL);
	
}
