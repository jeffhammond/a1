CC      = /software/mpich/mpich2-trunk/install-gnu/bin/mpicc
CFLAGS  = -g -O0 -Wall -pedantic -std=c99

all: serial.x parallel.x

parallel.x: parallel.o matrix.o
	$(CC) $(CFLAGS) $< matrix.o -lm -o $@

serial.x: serial.o matrix.o
	$(CC) $(CFLAGS) $< matrix.o -lm -o $@

parallel.o: parallel.c matrix.h
	$(CC) $(CFLAGS) -c $< -o $@

serial.o: serial.c matrix.h
	$(CC) $(CFLAGS) -c $< -o $@

matrix.o: matrix.c matrix.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(RMFLAGS) serial.o matrix.o

realclean: clean
	$(RM) $(RMFLAGS) serial.x

