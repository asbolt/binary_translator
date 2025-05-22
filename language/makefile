CC = g++
FLAGS ?= -I./include

CSRC = backend.c recursive_descent.c token.c main.c
COBJ = $(CSRC:%.c=./build/%.o)

.PHONY: all
all: $(COBJ)
	$(CC) $(FLAGS) $(COBJ) -o assembler

./build/%.o: source/%.c
	$(CC) $(FLAGS) -c $^ -o $@

.PHONY: clean
clean:
	rm -r ./build/*.o
	rm -r assembler