CC      = mpixlc_r 
COPT    = -O3 -qstrict

INCLUDE = -I. -I/bgsys/drivers/ppcfloor/arch/include/
LIBS    = libbench.a

LD      = $(CC)
CFLAGS  = $(COPT) $(INCLUDE)
LDFLAGS = $(COPT)

TESTS := $(patsubst %.c,%.x,$(wildcard *.c))
OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))

all: libbench.a $(TESTS)

libbench.a: bench_impl.o
	$(CC) $(CFLAGS) -c $< -o $@
	$(AR) $(ARFLAGS) libbench.a bench_impl.o

%.x: %.o
	$(LD) $(LDFLAGS) $< $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(RMFLAGS) $(OBJECTS)

realclean: clean
	$(RM) $(RMFLAGS) $(TESTS) libbench.a

