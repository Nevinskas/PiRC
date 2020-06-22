CC=gcc
CFLAGS=-I. -O2
#DEPS = hellomake.h
OBJ = main.o
LIBS = -lbcm2835

%.o: %.c #$(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
