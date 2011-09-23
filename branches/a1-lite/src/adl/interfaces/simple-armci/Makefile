#CC      = mpixlc_r 
#COPT    = -g -O3 -qstrict
CC      = mpicc
COPT    = -g -O2 -Wall

INCLUDE = -I. -I/bgsys/drivers/ppcfloor/arch/include/
LIBS    = libarmci-bgp.a

LD      = $(CC)
CFLAGS  = $(COPT) $(INCLUDE)
LDFLAGS = $(COPT)

TESTS   := $(wildcard test*.c)
OBJECTS := $(patsubst %.c,%.o,$(wildcard a1d*.c))
HEADERS := $(wildcard a1d*.h)

all: libarmci-bgp.a $(TESTS)

libarmci-bgp.a: $(OBJECTS)
	$(AR) $(ARFLAGS) libarmci-bgp.a $(OBJECTS)
	$(RM) $(OBJECTS)

%.x: %.o
	$(LD) $(LDFLAGS) $< $(LIBS) -o $@

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(RMFLAGS) $(OBJECTS)

realclean: clean
	$(RM) $(RMFLAGS) $(TESTS) libarmci-bgp.a

