
CC=/bgsys/drivers/ppcfloor/comm/xl/bin/mpicc
LD=/bgsys/drivers/ppcfloor/comm/xl/bin/mpicc
CFLAGS=-O3 -g
#LDFLAGS= #-L$(HOME)/bgq/work/comm/sys-fast/lib -lpami
LDFLAGS= -L/bgusr/sameerk/sandbox/bgq/work/comm/sys-fast/lib -lpami
TARGET_C=accumulate
TARGET_S=strided-accumulate

all : $(TARGET_C).cnk $(TARGET_S).cnk

$(TARGET_C).cnk : common.o armcix.o cmath.o $(TARGET_C).o 
	echo "Linking Executable $ ..."
	$(LD) -o $(TARGET_C).cnk $(TARGET_C).o common.o armcix.o cmath.o $(LDFLAGS)

$(TARGET_C).o : $(TARGET_C).c 
	$(CC) $(CFLAGS) -c $(TARGET_C).c 

$(TARGET_S).cnk : common.o armcix.o $(TARGET_S).o 
	echo "Linking Executable $ ..."
	$(LD) -o $(TARGET_S).cnk $(TARGET_S).o common.o armcix.o cmath.o $(LDFLAGS)

$(TARGET_S).o : $(TARGET_S).c 
	$(CC) $(CFLAGS) -c $(TARGET_S).c 

common.o : common.c
	$(CC) $(CFLAGS) -c common.c

cmath.o : cmath.c
	$(CC) $(CFLAGS) -c cmath.c

armcix.o : armcix.c
	$(CC) $(CFLAGS) -c armcix.c

clean:
	rm -f *.o accumulate.cnk strided-accumulate.cnk *~
