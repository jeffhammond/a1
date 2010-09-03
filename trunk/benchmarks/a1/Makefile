# Environment variable A1_INSTALL should point to the install directory of A1 
CC = mpixlc_r 
INCLUDE = -I. -I$(A1_INSTALL)/include 
LIB = -L$(A1_INSTALL)/lib -la1 

all:
	$(CC) $(INCLUDE) -O5 -qstrict -o $(NAME) $(NAME).c $(LIB)
