
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>

#include "easyrequest.h"
#include "easyio.h"
#include "easymem.h"
#include "easycomm.h"
#include "easyconf.h"
#include "easysched.h"

extern easy_host_t *g_host;
extern easy_config_t *g_config;
extern easy_mime_t *g_mime;
extern int g_trace;
#define HTTP_SCHEME	"http:"

const char *g_info[] =
{
	"Continue",
	"Switching Protocols"
};

const char *g_success[]=
{
	"OK"
	"Created",
	"Accepted",
	"Non-Authoritation Information",
	"No Content",
	"Reset Content",
	"Partial Content",

};
const char *g_redirection[] =
{
	"Moved Permanently",
	"Not Modified",
	
};
const char *g_client[]=
{
	"Bad Request",
	"Unauthorized",
	"Payment Required",
	"Forbidden",
	"Not Found",
	"Method Not Allowed",
	"Not Acceptable",
	"Proxy Authentication Required",
	"Request Timeout",
	"Conflict",
	"Gone",
	"Length Required",
	"Precondition Failed",
	"Request Entity Too Large",
	"Request-URI Too Long",
	"Unsupported Media Type",
	"Request Range Not Satisfiable",
	"Expectation Failed",
	
};
const char *g_server[] =
{
	"Interal Server Error",
	"Not Implemented",
	"Bad Gateway",
	"Service Unavailable",
	"Gateasy Timeout",
	"HTTP Version Not Supported",
};
void easy_free_header(easy_header_t *p)
{
	if(!p)
		return;
	easy_free(p->name);
	easy_free(p->value);
	easy_free(p);
}

easy_connection_t *easy_create_con()
{
	easy_connection_t *c = easy_malloc(sizeof(*c));
	c->inbuf = easy_malloc(BUFSIZE);
	//pthread_mutex_init(&c->lock,NULL);
	easy_init_con(c);
	return c;
}

void easy_init_con(easy_connection_t *c)
{
	c->remotefd = -1;
	//pthread_mutex_lock(&c->lock);
	c->status = CON_FREE;
	//pthread_mutex_unlock(&c->lock);
	c->keep_alive = CON_NON;
	c->request = NULL;
	if(c->inbuf)	
		memset(c->inbuf,0,BUFSIZE);
	if(c->outbuf)
		memset(c->outbuf,0,BUFSIZE);
}


void easy_free_con(easy_connection_t *c)
{
	easy_request_t *pnext = c->request;
	while(pnext)
	{
		c->request = pnext->next;
		easy_free_request(pnext);
		pnext = c->request;
	}
	easy_free(c);
}


easy_request_t *easy_create_request()
{
	easy_request_t *req = easy_malloc(sizeof(*req));
	
	easy_init_request(req);
	return req;
}

void easy_init_request(easy_request_t *req)
{

}

void easy_free_request(easy_request_t *req)
{
	
	easy_free(req->url);
	easy_str_free(req->host);
	easy_str_free(req->query);
	easy_str_free(req->path);
	easy_free(req);
}


void easy_add_request(easy_connection_t *c,easy_request_t *req)
{
	easy_request_t *pnext = c->request;
	req->con = c;
	
	easy_request_t *t = c->request;
	int i = 1;
	while(t)
	{
		printf("the %d request for connection:%d addr:%d\n",i,c->remotefd,c);
		++i;
		t = t->next;
	}
	if(NULL == c->request)
	{
		printf("there is only one request for connection:%d addr:%d\n",c->remotefd,c);
		c->request = req;
		return;
	}
	while(pnext->next)
		pnext = pnext->next;
	pnext->next = req;
}

///TODO:there are some problem about read;
int easy_process_request(easy_connection_t *c)
{

	int ret,len;
	char *p,*pc;
	char *pmethod,*purl,*pprotocol;
	easy_str *pscheme = NULL;
	easy_request_t *req;
	//easy_request_t *req = easy_create_request(c);
	//easy_add_request(c,req);
	ret = easy_read(c->remotefd,c->inbuf,BUFSIZE);
	if(ret < 13)
	{
		if(0 == ret)
			return -2;
		printf("less than 13:%s\n",c->inbuf);
		return -1;	
	}

	req = easy_create_request(c);
	easy_add_request(c,req);
	//printf("\n*****************recv content****************\n");
	//printf("%s\n",c->inbuf);

	p = c->inbuf;
	while(*p && *p == ' ')
		p++;
	if(!(pc = strchr(p,' ')))
		return -1;
	pmethod = (char*)easy_malloc(pc - p + 1);
	strncpy(pmethod,p,pc - p);
	pmethod[pc - p] = '\0';
	if(UNKNOWN == (req->method = easy_get_method(pmethod)))
	{
		req->status = STATUS_400;
		easy_free(pmethod);
		return 0;
	}
	easy_free(pmethod);

	//url 
	p = pc;
	SKIP_WS(p)
	if(!(pc = strchr(p,' ')))
		return 0;
	req->url = (char*)easy_malloc(pc -p + 1);
	strncpy(req->url,p,pc - p);
	req->url[pc - p] = '\0';
	
	p = req->url;
	pscheme = easy_str_init("http://",strlen("http://"));
	if(easy_strequal(req->url,"http://",pscheme->len))
	{
		p = req->url + pscheme->len;
		while(*p && *p != ':' && *p != '/')
			++p;
		req->host = easy_str_init(req->url + pscheme->len,p - req->url - pscheme->len);
		
	}
	
	easy_str_free(pscheme);
	while(*p &&(*p!= '/'))
		++p;
	
	char *temp = strchr(req->url,'?');
	len = (NULL == temp)?strlen(p):temp - p;
	req->path = easy_str_init(p,len);

	if(g_trace)
		printf("path before translated:%s\n",req->path->data);
	easy_translate_path(req->path);
	
	if(g_trace)
		printf("path after translated:%s\n",req->path->data);
	//query string
	if(NULL != temp)
	{
		req->query = easy_str_init(temp + 1,strlen(temp));
		if(g_trace)
			printf("query string:%s\n",req->query->data);
	}
		

	//http version
	p = pc;
	SKIP_WS(p)
	pprotocol = (char*)easy_malloc(9);
	strncpy(pprotocol,p,8);
	pprotocol[8] = '\0';
	if(UNKNOWN_HTTP == (req->proto = easy_get_http_version(pprotocol)))
	{
		req->status = STATUS_400;
		easy_free(pprotocol);
		return 0;
	}
	easy_free(pprotocol);
	
	req->header = strchr(p,'\n') + 1;
	if(easy_parse_request(req) < 0)
		return 0;

	easy_str *rpath = easy_get_realpath(req);
	if((req->rfd = open(rpath->data,O_RDONLY)) < 0)
	{
		req->con->errfile = easy_str_init(g_config->error->data,g_config->error->len);
		req->status = STATUS_404;
		easy_str_free(rpath);
		return 0;
	}
	easy_str_free(rpath);

	
	return 0;
	

}

/*first,exact match host. then match default host*/
easy_str *easy_get_realpath(easy_request_t *req)
{
	
	easy_str_t *location;
	easy_host_t *host = g_host;
	//there is no host header in req or url path is relative
	if(!req->host)
	{
		location = easy_str_init(g_config->location->data,g_config->location->len
			+ req->path->len);
		goto found;
	}
	
	while(host)
	{
		if(!host->name || !host->name->data || '\0' == host->name->data[0])
			continue;
		if(!strcmp(req->host->data,host->name->data))
			break;	
		host = host->next;
	}

	//default or global host info
	if(!host)
	{
		if(strcmp(req->host->data,g_config->name->data))
		{

		req->status = STATUS_404;
		req->con->errfile = easy_str_init(g_config->error->data,g_config->error->len);	
		return NULL;
		}
		location = easy_str_init(g_config->location->data,g_config->location->len
			+ req->path->len);
	}
	else
		location = easy_str_init(host->location->data,host->location->len
			+ req->path->len);			
				
found:
	
	if(!location->data || '\0' == location->data[0])
	{
		req->status = STATUS_404;
		req->con->errfile = easy_str_init(g_config->error->data,g_config->error->len);
		return NULL;
	}
	/*
	if(location->data[location->len - 1] == '/')
	{
		location->data[host->location->len - 1] = '\0';
		location->len--;
	}
	*/
	
	strcat(location->data,req->path->data);
	return location;
	
}

int easy_get_method(const char *me)
{
	if(!strcmp(me,M_GET))
		return GET;
	if(!strcmp(me,M_PUT))
		return PUT;
	if(!strcmp(me,M_POST))
		return POST;
	if(!strcmp(me,M_HEAD))
		return HEAD;

	return UNKNOWN;	
}

int easy_get_http_version(const char *ver)
{
	if(!strcmp(ver,P_HTTP11))
		return HTTP11;
	else if(!strcmp(ver,P_HTTP10))
		return HTTP10;
	else if(!strcmp(ver,P_HTTP09))
		return HTTP09;
	return UNKNOWN_HTTP;
}


int easy_get_header(const char *h,easy_header_t **header)
{
	const char *p,*psrc;
	int over = 0;
	psrc = h;
	SKIP_WS(h)
	p = h;
	*header = (easy_header_t*)easy_malloc(sizeof(easy_header_t));
	while(*p && !over)
	{
		switch(*p)
		{
		case ':':
			(*header)->name = easy_strncpy(h,p - h + 1);
			++p;
			SKIP_WS(p);
			h = p;
			break;
		case '\r':
			(*header)->value = easy_strncpy(h,p - h);
			over = 1;
			break;
		default:
			++p;
			break;
		
		}

			
	}

	return p + 2 - psrc; 
	
}


int easy_parse_request(easy_request_t *req)
{
	char *p;
	easy_header_t *pheader;
	int ret = 0,offset = 0;
	easy_str *phost = NULL;
	p = req->header;
	
	//2 strlen('\r\n')
	while(2 != (ret = easy_get_header(req->header + offset,&pheader)))
	{
		if(!strcmp(ACCEPT,pheader->name))
		{

		}
		else if(!strcmp(ACCEPT_ENCODING,pheader->name))
		{
			
		}
		else if(!strcmp(ACCEPT_LANGUAGE,pheader->name))
		{

		}
		else if(!strcmp(CONNECTION,pheader->name))
		{
			if(easy_strequali("keep-alive",pheader->value))
				req->con->keep_alive = CON_KEEP_ALIVE;
			if(easy_strequali("close",pheader->value))
				req->con->keep_alive = CON_CLOSE;	
		}
		else if(!strcmp(COOKIE,pheader->name))
		{
			
		}
		else if(!strcmp(HOST,pheader->name))
		{
			phost = easy_str_init(pheader->value,strlen(pheader->value));
			//req->host = easy_strncpy(pheader->value,strlen(pheader->value));
		}
		else if(!strcmp(USER_AGENT,pheader->name))
		{
		}
		else if(!strcmp(IFMATCH,pheader->name))
		{

		}
		
		if(g_trace)
			printf("%s  %s \n",pheader->name,pheader->value);

		easy_free_header(pheader);
		offset += ret;
	}

	//http/1.1 must has host header 
	if((HTTP11 == req->proto && !phost)
	 ||((NULL != req->host)
	 && (NULL !=  phost) && !strcasecmp(req->host->data,phost->data)))
	{
		req->status = STATUS_400;
		return -1;
	}
	if(!req->host && phost)
	{

		req->host = easy_str_init(phost->data,phost->len);
	}
	easy_str_free(phost);
	req->status = STATUS_200;
	return 0;
}

int easy_process_path()
{
	return 0;
}

#define ISHEX(x) ((x>='0' && x<='9') || (x>='A' && x<='F') || (x>='a'&&x<='f'))
#define A2HEX(x) ((x>='0' && x<='9')?x-'0':((x>='A' && x <= 'F')? x-'A' + 10:x-'a'))
int easy_translate_path(easy_str *ps)
{
	char *pc = ps->data;
	char *dst = ps->data;
	int c;
	while(*pc)
	{
		c = *pc;
		switch(c)
		{
		case '%':
			if(ISHEX(*(pc + 1)) && ISHEX(*(pc + 2)))
			{
				c = A2HEX(*(pc+1));
				*dst = (c<<4) + A2HEX(*(pc+1));
			}
			else
				return -1;
			pc+=2;
			break;
		default:
			*dst = *pc;
			break;
		}
		++pc;
		++dst;
		
	}
	*dst = '\0';
	return 0;
}


int easy_process_response(easy_connection_t *c)
{
	char *p;
	easy_request_t *next;
	if(!c->outbuf)
		c->outbuf = easy_malloc(BUFSIZE);
	p = c->outbuf;
		
	easy_request_t *req = c->request;
	char *ptime = (char*)easy_malloc(64);
	int fd;
	if(req->rfd != -1)
	{
		fd = req->rfd;
	}
	else
		fd = open(c->errfile->data,O_RDONLY);
	struct stat info;
	fstat(fd,&info);
	int ret;
	while(req)
	{
		printf("processing response for connection:%d\n",c->remotefd);
		p+= sprintf(p,"%s %d\r\n",P_HTTP11,req->status);
		p+= sprintf(p,"Content: text/html\r\n");
		p+= sprintf(p,"Content-Length: %d\r\n",info.st_size);
		p+= sprintf(p,"Connection: %s\r\n",c->keep_alive == CON_CLOSE?"Close":"Keep-alive");
		
		easy_gettime(ptime,64);
		p+= sprintf(p,"Date: %s\r\n",ptime);
		p+= sprintf(p,"Server: easyhttpd\r\n\r\n");
		read(fd,p,info.st_size);
		p+= info.st_size;
		printf("file fd:%d file size:%d, \n inbuf:%s",fd,info.st_size,c->inbuf);
		p+= sprintf(p,"\r\n\r\n");
		
		easy_write(c->remotefd,c->outbuf,strlen(c->outbuf));
		
		next = req->next;
		easy_free_request(req);
		req = next;
		p = c->outbuf;
		memset(p,0,BUFSIZE);
	}
	assert(c->outbuf > 0);
	easy_free(ptime);
	c->status = CON_PENDING;
	c->request = NULL;
	memset(c->inbuf,0,BUFSIZE);
	//
	//close connection
	if(CON_CLOSE == c->keep_alive)
		easy_close_con(c);

	return 0;
}

void easy_close_con(easy_connection_t *c)
{
	printf("easy_close_con:%d\n",c->remotefd);
	easy_request_t *req,*next;
	assert(NULL != c);

	easy_sched_t *sched = easy_get_sched();
	assert(NULL != sched);	
	easy_epoll_del(sched->epfd,c->remotefd);
	close(c->remotefd);
	req = c->request;
	while(req)
	{
		next = req->next;
		easy_free_request(req);
		req = next;

	}
	
	easy_init_con(c);	
	printf("********************************easy_close_con end*********************\n");
}
