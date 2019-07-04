CC=gcc
CCFLAGS = -Wall -O3

gol: game-of-life.o
	$(CC) -o gol game-of-life.o

clean: 
	-rm -f game-of-life.o
	-rm -f gol