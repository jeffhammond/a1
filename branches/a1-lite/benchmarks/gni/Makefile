SHELL   = /bin/sh
CC = gcc

SRCS = cq_write_test.c
PGMS	= $(SRCS:.c=)
OBJS	= $(SRCS:.c=.o)

PMI_CFLAGS = $(shell pkg-config --cflags cray-pmi)
PMI_LIBS = $(shell pkg-config --libs cray-pmi)
UGNI_CFLAGS = $(shell pkg-config --cflags cray-ugni)
UGNI_LIBS = $(shell pkg-config --libs cray-ugni)

all: $(PGMS)

$(PGMS):
	$(CC) $(CFLAGS) $(PMI_CFLAGS) $(UGNI_CFLAGS) $(PMI_LIBS) $(UGNI_LIBS) -o $@ $@.c 

clean:
	rm -f core $(PGMS) *.o
