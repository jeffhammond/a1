# Environment variable A1_INSTALL should point to the install directory of A1 

FC      = mpixlf77_r 
CC      = mpixlc_r 
COPT    = -O3 -qtune=450 -qarch=450d -qstrict
FOPT    = -O3 -qtune=450 -qarch=450d -qstrict

INCLUDE = -I/bgsys/drivers/ppcfloor/arch/include/
LIBS    = 

CFLAGS  = $(COPT) $(INCLUDE)
FFLAGS  = $(FOPT) $(INCLUDE)

LD      = $(CC)
LDFLAGS = $(COPT)

TESTS := $(patsubst %.c,%.x,$(wildcard *.c))
ASSEMBLY := $(patsubst %.c,%.s,$(wildcard *.c))
COBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))
FOBJECTS := $(patsubst %.f,%.o,$(wildcard *.f))

all: tests

tests: $(TESTS)
asm: $(ASSEMBLY)

%.x: %.o
	$(LD) $(LDFLAGS) $< $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.f
	$(FC) $(FFLAGS) -c $< -o $@

%.s: %.c
	$(CC) $(CFLAGS) -S $< -o $@


clean:
	$(RM) $(RMFLAGS) $(OBJECTS)

realclean: clean
	$(RM) $(RMFLAGS) $(TESTS) $(ASSEMBLY)

