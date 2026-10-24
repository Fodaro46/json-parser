CC = gcc
CFLAGS = -Wall

build:
	$(CC) $(CFLAGS) -o parser main.c

run: build
	./parser

clean:
	rm -f parser parser.exe
