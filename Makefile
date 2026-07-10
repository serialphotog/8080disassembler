CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wshadow

.PHONY: all test sanitize clean

all: 8080disassembler

8080disassembler: 8080disassembler.c
	$(CC) $(CFLAGS) $< -o $@

tests/test_disassembler: tests/test_disassembler.c 8080disassembler.c
	$(CC) $(CFLAGS) $< -o $@

test: tests/test_disassembler
	./tests/test_disassembler

sanitize:
	$(CC) $(CFLAGS) -fsanitize=address,undefined tests/test_disassembler.c -o tests/test_disassembler-sanitize
	./tests/test_disassembler-sanitize

clean:
	rm -f 8080disassembler tests/test_disassembler tests/test_disassembler-sanitize
