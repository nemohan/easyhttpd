#ifndef EASYCONF_H_
#define EASYCONF_H_

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "easymem.h"
#define DEFAULT_CONF "./easyconf.config"

typedef struct mime_tag
{
	easy_str_t *ext;
	easy_str_t *type;
	struct mime_tag *next;
}easy_mime_t;

typedef struct control_tag
{
	int test;

}easy_control_t;

typedef struct host_tag
{
	easy_str_t *name;
	easy_str_t *location;
	easy_str_t *error;
	easy_str_t *log;
	easy_control_t *control;	
	struct host_tag *next;
	
}easy_host_t;


typedef struct config_tag
{
	
	easy_str_t *name;
	easy_str_t *location;
	easy_str_t *error;
	easy_str_t *log;
	easy_control_t *control;
	//unsigned int:16 port;
	int debug;
	int port;
}easy_config_t;

typedef struct name_value_tag
{
	char *data;
	struct name_value_tag *next;

}easy_nv_t;

extern easy_mime_t *g_mime;
extern easy_config_t *g_config;
extern easy_host_t *g_host;

void easy_read_config(const char *path);
static void easy_get_mime(easy_nv_t **pnv);
static easy_control_t *easy_get_control(easy_nv_t **pnv);
static easy_host_t *easy_get_host(easy_nv_t **pnv);
static easy_config_t *easy_get_global(easy_nv_t **pnv);
static void easy_parse_block(easy_nv_t *pnv);
static easy_nv_t *easy_get_block(FILE *fs);
static void easy_push(easy_nv_t **pnv,const char *psrc);

#endif
