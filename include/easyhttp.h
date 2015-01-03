#ifndef EASYHTTP_H_
#define EASYHTTP_H_

#include <pthread.h>

#define MAXFD	100

extern int fds[MAXFD];
void easy_start_server();
void easy_http_loop();

#endif
