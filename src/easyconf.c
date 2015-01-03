#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "easyconf.h"
#include "easymem.h"
#include "easyerr.h"

easy_config_t *g_config;
easy_host_t *g_host;
easy_mime_t *g_mime;
extern int g_trace;
void easy_read_config(const char *path)
{
	
	char *pl;
	FILE *fs;
	easy_nv_t *pnv;
	if(NULL == (fs = fopen(path,"r")))
	{
		easy_exit("config file is invalid");
		
	}
	
	while(NULL != (pnv = easy_get_block(fs)))
	{

		easy_parse_block(pnv);
		easy_nv_t *p;
		while(pnv)
		{
			if(g_trace)
				printf("%s\n",pnv->data);
			p = pnv->next;
			free(pnv->data);
			free(pnv);
			pnv = p;
		}
		pnv = NULL;	
	}

}

	
static void easy_get_mime(easy_nv_t **pnv)
{
	easy_nv_t *next = *pnv;
	char *pc;
	easy_mime_t *mime;
	while(next)
	{
		pc = strchr(next->data,'=');
		if(!pc)
		{

			if(!strcmp(next->data,"{"))
				goto next;
			else if(!strcmp(next->data,"}"))
			{
				break;
			}

		}
		
		*pc = '\0';
		mime = easy_malloc(sizeof(*mime));
		mime->ext = easy_str_init(next->data,strlen(next->data));
		mime->type = easy_str_init(pc+1,strlen(pc+1));
		
		if(!g_mime)
			g_mime = mime;
		else
		{
			mime->next = g_mime;
			g_mime = mime;
		}
		*pc = '=';
		
	next:
		next = next->next;
	}

	
	
}

static easy_control_t *easy_get_control(easy_nv_t **pnv)
{
	easy_nv_t *next = *pnv;
	char *pc;
	easy_control_t *pctrl = malloc(sizeof(*pctrl));
	while(next)
	{
		pc = strchr(next->data,'=');
		if(!pc)
		{

			if(!strcmp(next->data,"{"))
				goto next;
			else if(!strcmp(next->data,"}"))
			{
				break;
			}

		}
		*pc = '\0';
		if(!strcmp(next->data,"test"))
		{
			pctrl->test = atoi(pc+1);
		}
		*pc = '=';
	next:
		next = next->next;
	}

	*pnv = next;
	return pctrl;
}


static easy_host_t *easy_get_host(easy_nv_t **pnv)
{
	easy_nv_t *next = *pnv;
	char *pc;
	easy_str_t *pstr;
	easy_control_t *pctrl;
	easy_control_t *pt;
	easy_host_t *pconfig = (easy_host_t*)malloc(sizeof(*pconfig));
	easy_nv_t *ptemp;
	while(next)
	{
		pc=strchr(next->data,'=');
		if(!pc)
		{
			if(!strcmp(next->data,"{"))
			{
				goto next;
			}
			else if(!strcmp(next->data,"control"))
			{
				
				ptemp = next->next;
				pconfig->control = easy_get_control(&ptemp);
				next = ptemp;
				goto next;
			}
			else if(!strcmp(next->data,"}"))
				break;
				goto next;

		}
		*pc = '\0';
		if(!strcmp(next->data,"name"))
		{
			pconfig->name = easy_str_init(pc + 1,strlen(pc+1));

		}
		else if(!strcmp(next->data,"location"))
		{
			pconfig->location = easy_str_init(pc+1,strlen(pc+1));
		}
		else if(!strcmp(next->data,"error"))
		{
			pconfig->error = easy_str_init(pc+1,strlen(pc+1));

		}
		else if(!strcmp(next->data,"log"))
		{

			pconfig->log = easy_str_init(pc+1,strlen(pc+1));
		}
		*pc = '=';
		
	next:

		next = next->next;

	}

	
	return pconfig;
}


static easy_config_t *easy_get_global(easy_nv_t **pnv)
{
	easy_nv_t *next = *pnv;
	char *pc;
	easy_str_t *pstr;
	easy_control_t *pctrl;
	easy_config_t *pconfig = (easy_config_t*)malloc(sizeof(*pconfig));
	//pconfig->host = (easy_host_t*)easy_malloc(sizeof(easy_host_t));
	easy_nv_t *ptemp;
	while(next)
	{
		pc=strchr(next->data,'=');
		if(!pc)
		{
			if(!strcmp(next->data,"{"))
			{
				goto next;
			}
			else if(!strcmp(next->data,"control"))
			{
				ptemp = next->next;
				pconfig->control = easy_get_control(&ptemp);
				next = ptemp;
				goto next;
			}
			else if(!strcmp(next->data,"}"))
				break;
				goto next;

		}
		*pc = '\0';
		if(!strcmp(next->data,"port"))
		{
			pconfig->port = atoi(pc + 1);

		}
		else if(!strcmp(next->data,"debug"))
		{

			if(!strcmp(pc+1,"on"))
				pconfig->debug = 1;
			else if(!strcmp(pc+1,"off"))
				pconfig->debug = 0;
		}
		
		else if(!strcmp(next->data,"name"))
		{
			pconfig->name = easy_str_init(pc + 1,strlen(pc+1));

		}
		else if(!strcmp(next->data,"location"))
		{
			pconfig->location = easy_str_init(pc+1,strlen(pc+1));
		}
		else if(!strcmp(next->data,"error"))
		{
			pconfig->error = easy_str_init(pc+1,strlen(pc+1));

		}
		else if(!strcmp(next->data,"log"))
		{

			pconfig->log = easy_str_init(pc+1,strlen(pc+1));
		}
		*pc ='=';
		
	next:

		next = next->next;

	}

	
	return pconfig;
}

static void easy_parse_block(easy_nv_t *pnv)
{

	int i = 0;
	easy_nv_t *next = pnv;
	easy_host_t *phost,*nexthost;
	{
		if(!strcmp(next->data,"global"))
		{
			g_config = easy_get_global(&(next->next));
		}		
		else if(!strcmp(next->data,"host"))
		{
			
			phost = easy_get_host(&(next->next));
			if(!g_host)
				g_host = phost;
			else
			{
			nexthost = g_host;
			while(nexthost->next)
				nexthost = nexthost->next;
			nexthost->next = phost;
			}
				
		}
		else if(!strcmp(next->data,"mime"))
			easy_get_mime(&(next->next));
			
	
	}




}



static void easy_push(easy_nv_t **pnv,const char *psrc)
{

	easy_nv_t *pn;
	easy_nv_t *next;
	
	pn = malloc(sizeof(*pn));
	memset(pn,0,sizeof(*pn));
	pn->data = malloc(strlen(psrc) + 1);
	
	strcpy(pn->data,psrc);
	pn->data[strlen(psrc)] = '\0';
	if(!(*pnv))
		*pnv = pn;
	else
	{	next = *pnv;
		while(next->next)
			next=next->next;
		next->next = pn;
		
	}
		

	

}

static easy_nv_t *easy_get_block(FILE *fs)
{
	int c,begin,end,com;
	int status = 0;
	char *p = (char*)easy_malloc(1024);
	char *psrc = p;
	begin = 0;	
	easy_nv_t *pnv = NULL,*pn,*pp;
	
	while(EOF != (c = fgetc(fs)))
	{
	switch(status)
	{
	case 0:
		switch(c)
		{
		case ' ':
		case '\t':
		case '\r':
			break;
		case '#':
			status = 1;
		default:
			ungetc(c,fs);
			status = 2;
			break;
		}
		break;
	case 1:
		switch(c)
		{
		case '\n':
			status = 0;
			break;
		default:
			break;
		}
		break;
	case 2:
		switch(c)
		{
		case '{':
			*p++ = c;	
			easy_push(&pnv,psrc);
			memset(psrc,0,1024);
			p = psrc;	
			begin++;
			status = 0;
			
			break;
		case '}':
			*p++ = c;	
			if(*psrc)
			{
				easy_push(&pnv,psrc);
					
				memset(psrc,0,1024);
				p = psrc;
			}
			if(--begin == 0)
				goto out;
			status = 0;
		case '\n':
			if(*psrc)
			{
				easy_push(&pnv,psrc);
				memset(psrc,0,1024);
				p = psrc;
			}
			status = 0;
			break;
		case '\t':
		case ' ':
		case '\r':
			status = 0;
			break;
		default:
			*p++ = c;
			break;
		}
		break;
	}
	
	
	}
	
	out:
	
		
		free(psrc);
		return pnv;
}

//#define DEBUG 1
#ifdef DEBUG
int main(int argc,char *argv[])
{
	FILE *fs = fopen("./easyconf.config","r");
	easy_nv_t *pnv = NULL;
	while(NULL != (pnv = easy_get_block(fs)))
	{
//		pnv = easy_get_block(fs);
		easy_parse_block(pnv);


		easy_nv_t *p;
		while(pnv)
		{
		printf("%s\n",pnv->data);
		p = pnv->next;
		free(pnv->data);
		free(pnv);
		pnv = p;
		}
		pnv = NULL;
	
	}
/*
	easy_host_t *host = g_host;
	while(host)
	{
		printf("name:%s\n",g_host->name->data);
		printf("log:%s\n",host->log->data);
		printf("error:%s\n",host->error->data);
		printf("location:%s\n",host->location->data);
		host = host->next;
	}

	easy_mime_t *m = g_mime;
	while(m)
	{
		printf("ext:%s  type:%s\n",m->ext->data,m->type->data);
		m = m->next;
	}
*/	
	return 0;
}
#endif
