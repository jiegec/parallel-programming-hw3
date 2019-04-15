default:matvectmul
ifeq ($(shell uname), Darwin)
CC = mpicc
else
CC = mpiicc
endif
CFLAGS = -O3
CLIBS = 
matvectmul:prog3_56.o
	$(CC) $(CLIBS) prog3_56.o -o matvectmul
prog3_56.o:prog3_56.cpp
	$(CC) $(CFLAGS) -c prog3_56.cpp -o prog3_56.o
prog3.11d:prog3.11d.cpp
	$(CC) $(CFLAGS) prog3.11d.cpp -o prog3.11d
prog3.1_histo_dist:prog3.1_histo_dist.c
	$(CC) $(CFLAGS) prog3.1_histo_dist.c -o prog3.1_histo_dist
clean:
	rm -rf prog3_56.o matvectmul
