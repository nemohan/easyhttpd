#ifndef EASYREQUEST_H_
#define EASYREQUEST_H_

#include <arpa/inet.h>
#include <pthread.h>
#include "easymem.h"
//#include "easysched.h"
//#include "easyconnection.h"

#define BUFSIZE 4096

#define M_GET "GET"
#define M_PUT "PUT"
#define M_HEAD "HEAD"
#define M_POST "POST"

#define GET 0
#define PUT 1
#define HEAD 2
#define POST 3
#define UNKNOWN -1

#define P_HTTP11 "HTTP/1.1"
#define P_HTTP10 "HTTP/1.0"
#define P_HTTP09 "HTTP/0.9"

#define HTTP11 11
#define HTTP10 10
#define HTTP09 9
#define UNKNOWN_HTTP -1

/*********generic header*********/
#define CONNECTION "Connection:"
#define DATE	"Date:"
#define MIMEVER "MIME-Version:"
#define TRANSFER_ENCODING "Transfer-Encoding:"

/**************request headers***********/
#define HOST 		"Host:"
#define USER_AGENT 	"User-Agent:"
#define ACCEPT 		"Accept:"
#define ACCEPT_CHARSET	"Accept-Charset:"
#define ACCEPT_ENCODING "Accept-Encoding:"
#define ACCEPT_LANGUAGE "Accept-Language:"
#define AUTHORIZATION 	"Authorization:"
#define COOKIE		"Cookie:"

#define CONTENT_BASE 	"Content-Base:"
#define CONTENT_ENCODING "Content-Encoding:"
#define CONTENT_LANGUAGE "Content-Language:"
#define CONTENT_LENGTH	"Content-Length:"
#define CONTENT_LOCATION "Content-Location:"
#define CONTENT_RANGE 	"Content-Range:"
#define CONTENT_TYPE	"Content-type:"
#define EXPIRES		"Expires:"
#define LAST_MODIFIED 	"Last-Modified:"

/*************conditional request header**********/
#define IFMATCH		"If-Match:"
#define IFMODIFIED_SINCE "If-Modified-Since:"
#define IFNONE_MATCH 	"If-None-Match:"
#define IFRANGE		"If-Range:"
#define IFUNMODIFIED_SINCE "If-Unmodified-Since:"
#define RANGE		"Range:"

#define STATUS_100 100

#define STATUS_200 200
#define STATUS_201 201
#define STATUS_202 202
#define STATUS_204 204
#define STATUS_205 205

#define STATUS_301 301
#define STATUS_302 302
#define STATUS_303 303
#define STATUS_304 304
#define STATUS_307 307

#define STATUS_400 400
#define STATUS_403 403
#define STATUS_404 404

typedef struct status_tag
{
	int status;
	const char *phrase;
}easy_status_t;

typedef struct header_tag
{
	char *name;
	char *value;
	struct header_tag *next;

}easy_header_t;


typedef struct request_tag
{
	int status;
	int  proto:16;
	int method:16;
	int rfd;
	easy_str *realpath;
	easy_str *path;
	easy_str *host;
	easy_str *query;
	char *url;
	char *header;
	char *body;
	char *inbuf;
	char *para;
	easy_header_t *headers;
	struct connection_tag *con;
	struct request_tag *next;
}easy_request_t;


typedef struct connection_tag
{

#define CON_NON 0
#define CON_KEEP_ALIVE 1
#define CON_CLOSE 2

#define CON_FREE 1ul
#define CON_PENDING 2ul
#define CON_READING 4ul
#define CON_WRITING 8ul
#define CON_STATUS (CON_PENDING | CON_READING | CON_WRITING)

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



void easy_free_header(easy_header_t *p);
easy_request_t *easy_create_request();
void easy_init_request(easy_request_t *p);
void easy_free_request(easy_request_t *req);


easy_connection_t *easy_create_con();
void easy_free_con();
void easy_init_con(easy_connection_t *c);
void easy_close_con(easy_connection_t *c);

easy_str_t *easy_get_realpath(easy_request_t *req);
int easy_process_request(easy_connection_t *c);
int easy_get_method(const char *me);
int easy_parse_request();
int easy_get_header(const char *h,easy_header_t **header);
int easy_get_http_version(const char *ver);
int easy_translate_path(easy_str *ps);
#endif
