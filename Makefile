default:prog3.11d prog3.1_histo_dist prog3.5 prog3.6
ifeq ($(shell uname), Darwin)
CC = mpicc
CXX = mpic++
else
CC = mpiicc
CXX = mpiicpc
endif
CFLAGS = -O3
CLIBS = 
prog3.11d:prog3.11d.cpp
	$(CC) $(CFLAGS) prog3.11d.cpp -o prog3.11d
prog3.1_histo_dist:prog3.1_histo_dist.c
	$(CC) $(CFLAGS) prog3.1_histo_dist.c -o prog3.1_histo_dist
prog3.5:prog3.5.cpp
	$(CXX) $(CFLAGS) prog3.5.cpp -o prog3.5
prog3.6:prog3.6.cpp
	$(CXX) $(CFLAGS) prog3.6.cpp -o prog3.6
clean:
	rm -rf prog3.11d prog3.1_histo_dist prog3.5 prog3.6 matvectmul
