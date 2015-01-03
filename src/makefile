SRCDIR=./
INCLUDEDIR=../include

CFLAGS=-I$(INCLUDEDIR)
CC=gcc

OBJS=easyhttp.o easyerr.o easymain.o easypoll.o \
easythread.o easymem.o easysched.o \
easyio.o easyrequest.o easycomm.o easyconf.o

all:easyhttpd

easyhttpd:$(OBJS)
	$(CC) -g -lpthread $(CFLAGS) -o $@ $(OBJS)
$(OBJS):%.o:%.c
	$(CC) -g -c $(CFLAGS) $< 

.PHONY:clean
clean:
	rm -f *.o easyhttpd
