# Environment variable A1_INSTALL should point to the install directory of A1 

CC      = mpixlc_r 
COPT    = -g -O3

#INCLUDE = -I$(A1_INSTALL)/include 
#LIBS    = -L$(A1_INSTALL)/lib -la1 

ARMCI_INSTALL = /home/projects/nwchem/ga-4-3
INCLUDE = -I$(ARMCI_INSTALL)/armci/src
LIBS    = -L$(ARMCI_INSTALL)/lib/BGP -larmci

LD      = $(CC)
CFLAGS  = $(COPT) $(INCLUDE)
LDFLAGS = $(COPT)

TESTS := $(patsubst %.c,%.x,$(wildcard *.c))
OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))

all: $(TESTS)

%.x: %.o
	$(LD) $(LDFLAGS) $< $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(RMFLAGS) $(OBJECTS)

realclean: clean
	$(RM) $(RMFLAGS) $(TESTS)

