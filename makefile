CC = g++
CFLAGS = -O3 -mcx16 -march=native -DCILK -fcilkplus -std=c++17

all:	qsort

qsort:	qsort.cpp
	$(CC) $(CFLAGS) -DNDEBUG qsort.cpp -o qsort
	
	
clean:
	rm -f qsort
