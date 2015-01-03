
#include <sys/types.h>
#include <unistd.h>
#include <sys/errno.h>
#include <stdio.h>
#include "easyio.h"
extern int g_trace;
ssize_t easy_read(int fd,void *buf,size_t len)
{
	ssize_t ret,rdlen = 0;
	while(len > 1)
	{
		if(g_trace)
		{
			printf("\n************recv content*********\n");
			printf("%s\n",buf - rdlen);
			printf("\n***********recv end*************\n");
		}
		ret = read(fd,buf + rdlen,len);
		if(ret <= 0 /*&& (errno == EAGAIN || errno == EWOULDBLOCK)*/)
			return rdlen;
		len -= ret;
		rdlen += ret;
		
		
	}

	return rdlen;
}

ssize_t easy_write(int fd,void *buf,size_t len)
{
	int ret,wrlen = 0;
	while(len > 0)
	{
		ret = write(fd,buf + wrlen,len);
		if(ret < 0)
		 	return wrlen;
		len -= ret;
		wrlen += ret;
	}
	
	return wrlen;

}
