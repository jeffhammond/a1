FC      = ftn 
CC      = cc 
COPT    = -g
FOPT    = -g

# CSCS paths
DMAPP_PATH=/opt/cray/dmapp/2.2-1.0301.2349.6.6.gem
GNIH_PATH=/opt/cray/gni-headers/2.0-1.0301.2302.4.1.gem
PMI_PATH=/opt/cray/pmi/1.0-1.0000.7901.22.1.gem

INCLUDE = -I$(DMAPP_PATH)/include -I$(GNIH_PATH)/include -I$(PMI_PATH)/include
LIBS    = -L$(DMAPP_PATH)/lib64 -L$(GNIH_PATH)/lib64 -L$(PMI_PATH)/lib64 -ldmapp -lpmi

CFLAGS  = $(COPT) $(INCLUDE)
FFLAGS  = $(FOPT) $(INCLUDE)

CLD      = $(CC)
CLDFLAGS = $(COPT)

FLD      = $(FC)
FLDFLAGS = $(FOPT)

CTESTS := $(patsubst %.c,%.x,$(wildcard *.c))
COBJ   := $(patsubst %.c,%.o,$(wildcard *.c))
CASM   := $(patsubst %.c,%.s,$(wildcard *.c))

SUBMIT := qsub -n 1 --mode smp -t 5 

all: tests

tests: $(CTESTS)
asm: $(CASM)

%.x: %.o
	$(CLD) $(CLDFLAGS) $< $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.s: %.c
	$(CC) $(CFLAGS) -qlist -qsource -qreport -S $< -o $@

%.submit: %.x
	$(SUBMIT) $<
    
clean:
	$(RM) $(RMFLAGS) $(COBJ)

realclean: clean
	$(RM) $(RMFLAGS) $(CTESTS)
	$(RM) $(RMFLAGS) $(CASM)

