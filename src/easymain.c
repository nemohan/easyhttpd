#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "easyhttp.h"
#include "easythread.h"
#include "easyconf.h"
#include "easycomm.h"
//#include "easymem.h"

int g_trace = 0;
char *g_config_file;
int easy_get_thread_num()
{
	int cpu;
	cpu = sysconf(_SC_NPROCESSORS_CONF);
	return (1==cpu?2:cpu);
}

void easy_print_version()
{
	fprintf(stdout,"easyhttpd version:0.1\n");
	fprintf(stdout,"this program under GNU license\n");
	fprintf(stdout,"author:nemohan\n");
	fflush(stdout);
}

void easy_print_usage()
{
	fprintf(stdout,"usage:[-v|-d|-t|-c configfile]\n");
	fprintf(stdout,"-v show version of easyhttpd.\n");
	fprintf(stdout,"-c configfile specify config file for easyhttpd.\n");
	fprintf(stdout,"-d run ad deamon.\n");
	fprintf(stdout,"-t trace program.\n");
	fflush(stdout);
}


int main(int argc,char *argv[])
{
	int i;
	int worker_num;
	int opt;
	int deamon = 0;
	opterr = 0;
	while(-1 != (opt = getopt(argc,argv,"c:vdt")))
	{

		switch(opt)
		{
		case 'c':
			g_config_file = (char*)easy_malloc(strlen(optarg)+1);
			strcpy(g_config_file,optarg);
			g_config_file[strlen(optarg)] = '\0';
			//printf("%s\n",g_config_file);
			//exit(0);
			break;

		case 'v':
			easy_print_version();
			exit(0);
			break;
		case 'd':
			deamon = 1;
			break;
		case 't':
			g_trace = 1;
			break;
		case '?':
			easy_print_usage();
			exit(0);
			break;

		}

	}
	
	easy_read_config((NULL==g_config_file?DEFAULT_CONF:g_config_file));	
	if(deamon)
	{
	}
//	easy_setup_signal_handler(SIGSEGV,easy_catch_segment);
	pthread_key_create(&g_conn_key,NULL);
	worker_num = easy_get_thread_num();
	easy_spawn_workers(worker_num);	
	easy_start_server();
	easy_http_loop();
	
	return 0;
}
