SRC=libvcd.c
OBJECTS=libvcd.o
CC=gcc

build: $(SRC)
	$(CC) -g -Wall -o $(OBJECTS) -c $(SRC)

test: build
	$(CC) -g -Wall -o test/test test/test.c $(OBJECTS)

clean:
	rm $(OBJECTS) test/test

.PHONY: build
