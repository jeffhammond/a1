# Environment variable A1_INSTALL should point to the install directory of A1 

FC      = mpixlf77_r 
CC      = mpixlc_r 
COPT    = -g -qstrict -O5 -qtune=450 -qarch=450d
FOPT    = -g -qstrict -O5 -qtune=450 -qarch=450d

INCLUDE = -I/bgsys/drivers/ppcfloor/arch/include/
LIBS    = 

CFLAGS  = $(COPT) $(INCLUDE)
FFLAGS  = $(FOPT) $(INCLUDE)

CLD      = $(CC)
CLDFLAGS = $(COPT)

FLD      = $(FC)
FLDFLAGS = $(FOPT)

CTESTS := $(patsubst %.c,%.x,$(wildcard *.c))
FTESTS := $(patsubst %.f,%.fx,$(wildcard *.f))
COBJ := $(patsubst %.c,%.o,$(wildcard *.c))
FOBJ := $(patsubst %.f,%.o,$(wildcard *.f))
CASM := $(patsubst %.c,%.s,$(wildcard *.c))
FASM := $(patsubst %.f,%.s,$(wildcard *.f))

SUBMIT := qsub -n 1 --mode smp -t 5 

all: tests

tests: $(CTESTS) $(FTESTS)
asm: $(CASM) $(FASM)

%.x: %.o
	$(CLD) $(CLDFLAGS) $< $(LIBS) -o $@

%.fx: %.o
	$(FLD) $(FLDFLAGS) $< $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.f
	$(FC) $(FFLAGS) -c $< -o $@

%.s: %.c
	$(CC) $(CFLAGS) -qlist -qsource -qreport -S $< -o $@

%.s: %.f
	$(FC) $(FFLAGS) -qlist -qsource -qreport -S $< -o $@

%.submit: %.x
	$(SUBMIT) $<
    
clean:
	$(RM) $(RMFLAGS) $(COBJ) $(FOBJ)

realclean: clean
	$(RM) $(RMFLAGS) $(CTESTS) $(FTESTS)
	$(RM) $(RMFLAGS) $(CASM) $(FASM)
	$(RM) $(RMFLAGS) *.lst
	$(RM) $(RMFLAGS) core.*

