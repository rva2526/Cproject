
CC = gcc
CFLAGS = -Wall -I/home/jovyan/include -L/home/jovyan/lib  -lcjson
COMPILED = plouf


default: all clean

all: $(COMPILED)

$(COMPILED): ./src/main.o ./src/read_parse.o
	$(CC) ./src/main.o ./src/read_parse.o -o $(COMPILED) $(CFLAGS)

main.o: ./src/main.c ./src/read_parse.h
	$(CC) -c ./src/main.c -o ./src/main.o $(CFLAGS)

read_parse.o: ./src/read_parse.c ./src/read_parse.h
	$(CC) -c ./src/read_parse.c -o ./src/read_parse.o $(CFLAGS)

clean:
	rm -f ./src/*.o
	rm -f *.o