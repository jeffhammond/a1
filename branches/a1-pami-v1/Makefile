
CC=/bgsys/drivers/ppcfloor/comm/gcc/bin/mpicc
LD=/bgsys/drivers/ppcfloor/comm/gcc/bin/mpicc
CFLAGS=-O3 -g
#LDFLAGS=
LDFLAGS= -L$(HOME)/bgq/work/comm/sys/lib -lpami
#LDFLAGS= -L/bgusr/sameerk/sandbox/bgq/work/comm/sys-fast/lib -lpami
TARGET_A=accumulate
TARGET_G=get
TARGET_P=put
TARGET_R=rmw

all : $(TARGET_A).cnk $(TARGET_G).cnk $(TARGET_P).cnk $(TARGET_R).cnk 

$(TARGET_A).cnk : common.o a1-pami.o cmath.o $(TARGET_A).o 
	echo "Linking Executable $ ..."
	$(LD) -o $(TARGET_A).cnk $(TARGET_A).o common.o a1-pami.o cmath.o $(LDFLAGS)

$(TARGET_A).o : $(TARGET_A).c 
	$(CC) $(CFLAGS) -c $(TARGET_A).c 

$(TARGET_G).cnk : common.o a1-pami.o $(TARGET_G).o 
	echo "Linking Executable $ ..."
	$(LD) -o $(TARGET_G).cnk $(TARGET_G).o common.o a1-pami.o cmath.o $(LDFLAGS)

$(TARGET_G).o : $(TARGET_G).c 
	$(CC) $(CFLAGS) -c $(TARGET_G).c 

$(TARGET_P).cnk : common.o a1-pami.o $(TARGET_P).o 
	echo "Linking Executable $ ..."
	$(LD) -o $(TARGET_P).cnk $(TARGET_P).o common.o a1-pami.o cmath.o $(LDFLAGS)

$(TARGET_P).o : $(TARGET_P).c 
	$(CC) $(CFLAGS) -c $(TARGET_P).c 


$(TARGET_R).cnk : common.o a1-pami.o $(TARGET_R).o 
	echo "Linking Executable $ ..."
	$(LD) -o $(TARGET_R).cnk $(TARGET_R).o common.o a1-pami.o cmath.o $(LDFLAGS)

$(TARGET_R).o : $(TARGET_R).c 
	$(CC) $(CFLAGS) -c $(TARGET_R).c 

common.o : common.c
	$(CC) $(CFLAGS) -c common.c

cmath.o : cmath.c
	$(CC) $(CFLAGS) -c cmath.c

a1-pami.o : a1-pami.c
	$(CC) $(CFLAGS) -c a1-pami.c

clean:
	rm -f *.o *.cnk *~
