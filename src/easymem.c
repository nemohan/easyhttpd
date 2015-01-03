#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "easymem.h"
//#include "easyrequest.h"
#include "easyerr.h"

#define TOUPPER(x) (x>='a' && x<= 'z'?x - 32:x)
easy_str *easy_str_create()
{
	easy_str *p = easy_malloc(sizeof(easy_str));
	return p;
}

int easy_strequali(const char *dst,const char *src)
{
	int c,d;
	while(*dst && *src)
	{
		if((c=TOUPPER(*dst)) != (d=TOUPPER(*src)))
			return 0;
		++dst;
		++src;
	}
	if(*dst || *src)
		return 0;
	return 1;

}
easy_str *easy_str_init(const char *data,int len)
{
	easy_str *pstr = NULL;
	assert(NULL != data && '\0' != data[0]);
	
	pstr = easy_malloc(sizeof(easy_str));
	pstr->len = len <= 0?strlen(data):len;
	
	pstr->data = easy_malloc(pstr->len + 1);
	strncpy(pstr->data,data,pstr->len);
	pstr->data[pstr->len] = '\0';
	return pstr;
}

void easy_str_free(easy_str *p)
{
	if(!p)
		return;
	easy_free(p->data);
	easy_free(p);
}

//[beg,end)

char *easy_strncpy(const char *src,int len)
{
	char *dst = NULL;
	if(!len)
		return dst;
	dst = easy_malloc(len + 1);
	strncpy(dst,src,len);
	dst[len] = '\0';
	
	return dst;

}

int easy_strequal(const char *dst,const char *src,int n)
{
	for(;(n > 0)&& (*dst && *src)&&(*dst++ == *src++);n--);
	
	if(0 == n)
		return 1;
	else 
		return 0;
	
}


void *easy_malloc(size_t n)
{
	void *p = malloc(n);
	if(!p)
	{
		easy_exit("failed to malloc mem");
	}		
	memset(p,0,n);
	return p;
}

void easy_free(void *p)
{
	if(NULL == p)
		return;
	free(p);
}

void *easy_realloc(void *p,size_t n)
{
	return NULL;
}
