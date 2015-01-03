#ifndef EASYMEM_H_
#define EASYMEM_H_
#include <sys/types.h>
#include <unistd.h>


typedef struct str_tag
{
	char *data;
	int len;
}easy_str;
typedef easy_str easy_str_t;

easy_str *easy_str_init(const char *data,int len);
void easy_str_free(easy_str *p);
easy_str *easy_str_create();

int easy_strequali(const char *dst,const char *src);
int easy_strequal(const char *dst,const char *src,int n);
char *easy_strncpy(const char *src,int len);
void *easy_malloc(size_t n);
void *easy_realloc(void *p,size_t n);
void easy_free(void *p);

#endif
