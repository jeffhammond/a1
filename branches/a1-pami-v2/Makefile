
#DRV=/bgsys/drivers/ppcfloor
DRV=/bgsys/drivers/V1R1M2/ppc64

CC=$(DRV)/comm/gcc/bin/mpicc
LD=$(DRV)/comm/gcc/bin/mpicc
AR=$(DRV)/gnu-linux/bin/powerpc64-bgq-linux-ar
#CC=$(DRV)/comm/xl/bin/mpicc 
#LD=$(DRV)/comm/xl/bin/mpicc
CFLAGS=-O3 -g -I.
LDFLAGS= -L$(DRV)/comm/sys-fast/lib
ARFLAGS= q
#LDFLAGS= -L$(HOME)/bgq/work/comm/sys/lib -lpami
TARGET_A=accumulate
TARGET_G=get
TARGET_P=put
TARGET_R=rmw
TARGET_LIB=liba1

all : $(TARGET_A).cnk $(TARGET_G).cnk $(TARGET_P).cnk $(TARGET_R).cnk 

$(TARGET_A).cnk : $(TARGET_A).o $(TARGET_LIB).a 
	echo "Linking Executable $ ..."
	$(LD) -o $(TARGET_A).cnk $(TARGET_A).o $(TARGET_LIB).a $(LDFLAGS)

$(TARGET_A).o : $(TARGET_A).c 
	$(CC) $(CFLAGS) -c $(TARGET_A).c 

$(TARGET_G).cnk : $(TARGET_G).o $(TARGET_LIB).a 
	echo "Linking Executable $ ..."
	$(LD) -o $(TARGET_G).cnk $(TARGET_G).o common.o a1-pami.o cmath.o $(LDFLAGS)

$(TARGET_G).o : $(TARGET_G).c 
	$(CC) $(CFLAGS) -c $(TARGET_G).c 

$(TARGET_P).cnk : $(TARGET_P).o $(TARGET_LIB).a 
	echo "Linking Executable $ ..."
	$(LD) -o $(TARGET_P).cnk $(TARGET_P).o common.o a1-pami.o cmath.o $(LDFLAGS)

$(TARGET_P).o : $(TARGET_P).c 
	$(CC) $(CFLAGS) -c $(TARGET_P).c 


$(TARGET_R).cnk : $(TARGET_R).o $(TARGET_LIB).a
	echo "Linking Executable $ ..."
	$(LD) -o $(TARGET_R).cnk $(TARGET_R).o common.o a1-pami.o cmath.o $(LDFLAGS)

$(TARGET_R).o : $(TARGET_R).c 
	$(CC) $(CFLAGS) -c $(TARGET_R).c 

$(TARGET_LIB).a : common.o cmath.o a1-pami.o
	$(AR) $(ARFLAGS) $(TARGET_LIB).a common.o cmath.o a1-pami.o 

common.o : common.c
	$(CC) $(CFLAGS) -c common.c

cmath.o : cmath.c
	$(CC) $(CFLAGS) -c cmath.c

a1-pami.o : a1-pami.c
	$(CC) $(CFLAGS) -c a1-pami.c

clean:
	rm -f *.o *.a *.cnk *~
