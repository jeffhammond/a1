CC      = mpicc
COPT    = -g -O0 -std=c99
DEVICE  = NONE

ifeq ($(TARGET),BGP)
#CC      = mpicc
#COPT    = -g -O0 -std=c99
CC      = mpixlc_r
COPT    = -g -O3 -qstrict -qlanglvl=extc99
# the last part is a hack and should not be necessary
INC     = -I. -DDCMFD -I/bgsys/drivers/ppcfloor/arch/include/
DEVICE  = dcmfd
endif

ifeq ($(TARGET),XE)
CC      = cc
COPT    = -g -O0
INC     = -I. -DDMAPPD
DEVICE  = dmappd
endif

ifeq ($(DEVICE),NONE) 
@ You need to set TARGET={BGP,XE}
endif

LIBS    = libparmci.a $(DEVICE)/liba1d.a

LD      = $(CC)
CFLAGS  = $(COPT) $(INC)
LDFLAGS = $(COPT)

TEST_BINARIES := $(patsubst %.c,%.x,$(wildcard test*.c))
TEST_OBJECTS  := $(patsubst %.c,%.o,$(wildcard test*.c))
OBJECTS       := parmci.o
HEADERS       := parmci.h

all: liba1d.a libparmci.a $(TEST_BINARIES)

%.x: %.o
	$(LD) $(LDFLAGS) $< $(LIBS) -o $@

libparmci.a: parmci.o
	$(AR) $(ARFLAGS) $@ $<

liba1d.a:
	make -C $(DEVICE)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(RMFLAGS) $(OBJECTS) $(TEST_OBJECTS)

realclean: clean
	$(RM) $(RMFLAGS) $(TEST_BINARIES) libparmci.a
	make -C $(DEVICE) realclean

