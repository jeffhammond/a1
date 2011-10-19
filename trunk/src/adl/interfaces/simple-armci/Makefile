CC      = mpicc
COPT    = -g -O0 -std=c99
#CC      = mpixlc_r
#COPT    = -g -O3 -qstrict -qlanglvl=extc99

INC     = -I. -I/bgsys/drivers/ppcfloor/arch/include/
LIBS    = libarmci-bgp.a

LD      = $(CC)
CFLAGS  = $(COPT) $(INC) -DDEBUG_FUNCTION_ENTER_EXIT
LDFLAGS = $(COPT)

TEST_BINARIES := $(patsubst %.c,%.x,$(wildcard test*.c))
TEST_OBJECTS  := $(patsubst %.c,%.o,$(wildcard test*.c))
A1D_OBJECTS   := $(patsubst %.c,%.o,$(wildcard a1d*.c))
A1D_HEADERS   := $(patsubst %.c,%.h,$(wildcard a1d*.c))
OBJECTS       := parmci.o $(A1D_OBJECTS)
HEADERS       := parmci.h $(A1D_HEADERS)

all: $(TEST_BINARIES)
#all: test_rmw.x

%.x: %.o libarmci-bgp.a
	$(LD) $(LDFLAGS) $< $(LIBS) -o $@

libarmci-bgp.a: $(OBJECTS)
	$(AR) $(ARFLAGS) libarmci-bgp.a $(OBJECTS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(RMFLAGS) $(OBJECTS) $(TEST_OBJECTS)

realclean: clean
	$(RM) $(RMFLAGS) $(TEST_BINARIES) libarmci-bgp.a

