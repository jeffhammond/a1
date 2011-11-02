CC      = cc
COPT    = -g -O0 -Wall -std=c99

DMAPP_PATH=/opt/cray/dmapp/default
GNIH_PATH=/opt/cray/gni-headers/default
UGNI_PATH=/opt/cray/ugni/default
UDREG_PATH=/opt/cray/udreg/default
PMI_PATH=/opt/cray/pmi/default

INC     = -I$(DMAPP_PATH)/include -I$(UDREG_PATH)/include -I$(GNIH_PATH)/include -I$(PMI_PATH)/include -I.
LIBS    = -L$(DMAPP_PATH)/lib64   -L$(UDREG_PATH)/lib64   -L$(UGNI_PATH)/lib64   -L$(PMI_PATH)/lib64 -ldmapp -ludreg -lugni -lpmi

LD      = $(CC)
CFLAGS  = $(COPT) $(INC) -DDEBUG_FUNCTION_ENTER_EXIT
LDFLAGS = $(COPT)

TEST_BINARIES := $(patsubst %.c,%.x,$(wildcard test*.c))
TEST_OBJECTS  := $(patsubst %.c,%.o,$(wildcard test*.c))
A1D_OBJECTS   := $(patsubst %.c,%.o,$(wildcard a1d*.c))
A1D_HEADERS   := $(patsubst %.c,%.h,$(wildcard a1d*.c))
OBJECTS       := $(A1D_OBJECTS)
HEADERS       := $(A1D_HEADERS)

all: $(TEST_BINARIES)

%.x: %.o liba1d.a
	$(LD) $(LDFLAGS) $< liba1d.a -o $@

liba1d.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(RMFLAGS) $(OBJECTS) $(TEST_OBJECTS)

realclean: clean
	$(RM) $(RMFLAGS) $(TEST_BINARIES) liba1d.a

