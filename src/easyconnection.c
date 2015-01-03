
#include "easyconnection.h"

easy_connnection_t *g_connections;

void easy_close_con(easy_connection_t *pcon)
{


}

easy_connection_t *easy_create_con()
{
	easy_connection_t *c = easy_malloc(sizeof(*c));
	c->inbuf = easy_malloc(BUFSIZE);
	return c;
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
