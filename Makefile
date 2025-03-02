
CC = gcc
CFLAGS = -Wall -I/home/jovyan/include -L/home/jovyan/lib  -lcjson -lm
COMPILED = plouf


default: all clean

all: $(COMPILED)

$(COMPILED): ./src/main.o ./src/read_parse.o ./src/forward_inversion.o
	$(CC) ./src/main.o ./src/read_parse.o ./src/forward_inversion.o -o $(COMPILED) $(CFLAGS)

main.o: ./src/main.c ./src/read_parse.h ./src/forward_inversion.h
	$(CC) -c ./src/main.c -o ./src/main.o $(CFLAGS)

forward_inversion.o: ./src/forward_inversion.c ./src/forward_inversion.h
	$(CC) -c ./src/forward_inversion.c -o ./src/forward_inversion.o $(CFLAGS)

read_parse.o: ./src/read_parse.c ./src/read_parse.h
	$(CC) -c ./src/read_parse.c -o ./src/read_parse.o $(CFLAGS)

clean:
	rm -f ./src/*.o
	rm -f *.o