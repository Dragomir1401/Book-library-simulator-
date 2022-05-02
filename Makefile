#Copyright Dragomir Andrei-Mihai
#compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -g -m32
LDFLAGS=-m32
# define targets
TARGET=main

# tester: main
# 	ls
# 	pwd
# 	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./main < in/test20.in > /dev/null &> error.valgrind && cat error.valgrind
# 	gcc -v
# 	valgrind --version
# 	sleep 180 && cat error.valgrind
# 	make clean

build: main

main: hashmap.o main.o parse_input.o functions.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^

pack:
	zip -FSr 312CA_DragomirAndrei_Tema2.zip README Makefile *.c *.h

clean:
	rm -rf *.o

.PHONY: pack clean