#include <stdlib.h>
#include <unistd.h>

#include "easyerr.h"
void easy_exit(const char *msg)
{
	perror(msg);
	exit(1);
}
