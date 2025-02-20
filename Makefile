LD_LIBRARY_PATH=$(shell export LD_LIBRARY_PATH=/home/jovyan/lib:$LD_LIBRARY_PATH; echo $$LD_LIBRARY_PATH)

CC = gcc
CFLAGS = -Wall -I/home/jovyan/include -L/home/jovyan/lib  -lcjson

default: all clean

all: test2

test2: ./src/main.o ./src/read_parse.o
	$(CC) ./src/main.o ./src/read_parse.o -o test2 $(CFLAGS)

main.o: ./src/main.c ./src/read_parse.h
	$(CC) -c ./src/main.c -o ./src/main.o $(CFLAGS)

read_parse.o: ./src/read_parse.c ./src/read_parse.h
	$(CC) -c ./src/read_parse.c -o ./src/read_parse.o $(CFLAGS)

clean:
	rm -f ./src/*.o
	rm -f *.o