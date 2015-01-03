#ifndef EASYTHREAD_H_
#define EASYTHREAD_H_
#include <pthread.h>
pthread_key_t g_conn_key;
int easy_spawn_workers(int capacity);
void *easy_worker(void *arg);
typedef void*(*worker_func)(void *arg);
#endif
