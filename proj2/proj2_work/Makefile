CC = gcc
CFLAGS = -g -Wall

all: project2

project2: project2.o
	$(CC) $(CFLAGS) -o project2 project2.o student2.o

project2.o: project2.c
	$(CC) $(CFLAGS) -c project2.c student2.c

clean:
	rm -rf project2 student2
	rm -rf *.o
