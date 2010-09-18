FC      = ftn 
CC      = cc 
COPT    = -g
FOPT    = -g

INCLUDE =
LIBS    = 

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

