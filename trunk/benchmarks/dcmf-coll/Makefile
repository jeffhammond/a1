# Environment variable A1_INSTALL should point to the install directory of A1 

CC       = mpixlc_r 
COPT     = -g -O3 -qstrict

INCLUDE  =
LIBS     = 

LD       = $(CC)
CFLAGS   = $(COPT) $(INCLUDE)
LDFLAGS  = $(COPT)

TESTS   := $(patsubst %.c,%.x,$(wildcard *.c))
OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))
OUTPUTS := $(patsubst %.c,%.output,$(wildcard *.c))

all: $(TESTS)

submit-all: $(OUTPUTS)

%.output: %.x
	qsub -n 2 --mode smp -t 30 -O $* $<

%.$(NODES).$(MODE).output: %.x
	qsub -n $(NODES) --mode $(MODE) -t 30 -O $*.$(NODES).$(MODE) $<

%.x: %.o
	$(LD) $(LDFLAGS) $< $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

submit-clean:
	$(RM) $(RMFLAGS) *.output *.error *.cobaltlog

clean:
	$(RM) $(RMFLAGS) $(OBJECTS)

realclean: clean
	$(RM) $(RMFLAGS) $(TESTS)

.PHONY: 

