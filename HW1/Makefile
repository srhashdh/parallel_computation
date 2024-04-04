CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGETS = algorithm1 algorithm2 algorithm3

all: $(TARGETS)

program1: algorithm1.c
	$(CC) $(CFLAGS) -o $@ $<

program2: algorithm2.c
	$(CC) $(CFLAGS) -o $@ $<

program3: algorithm3.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

run: $(TARGETS)
	./algorithm1
	./algorithm2
	./algorithm3

