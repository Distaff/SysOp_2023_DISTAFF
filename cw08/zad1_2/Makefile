CC = g++
CFLAGS = -Wall -std=c++20

all: life
	
life: grid.c life.cpp threaded_update.cpp
	$(CC) $(CFLAGS) grid.c life.cpp -o life -lncursesw threaded_update.cpp
	
clean:
	rm -f life
