#Copyright Dragomir Andrei-Mihai
#compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -g

# define targets
TARGET=main

build: $(TARGET)

main: hashmap.o main.o parse_input.o functions.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^

pack:
	zip -FSr 312CA_DragomirAndrei_Tema1.zip README Makefile *.c *.h

clean:
	rm -f $(TARGETS)

.PHONY: pack clean