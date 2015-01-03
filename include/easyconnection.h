#ifndef EASYCONNECTION_H_
#define EASYCONNECTION_H_


//#include "easyrequest.h"
#include "easymem.h"

struct easy_request_t;
typedef struct connection_tag
{

#define CON_KEEP_ALIVE 1
#define CON_CLOSE 2

#define CON_FREE 0
#define CON_PENDING 1
#define CON_READING 2
#define CON_WRITING 3
	easy_str_t *logfile;
	easy_str_t *errfile;
	char *inbuf;
	char *outbuf;	
	int remotefd;
	int status:16;
	int keep_alive:16;
	easy_request_t *request;
//	struct in_addr localaddr;
	struct connection_tag *next;
}easy_connection_t;

extern easy_connection_t *g_connections;

#define BUFSIZE 4096

void easy_close_connection(easy_connection_t *pcon);
easy_connection_t *easy_create_con();
void easy_free_con(easy_connection_t *c);

#endif

