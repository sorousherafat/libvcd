SRC=libvcd.c
OBJECTS=libvcd.o
CC=gcc

build: $(SRC)
	$(CC) -o $(OBJECTS) -c $(SRC)

clean:
	rm $(OBJECTS)

.PHONY: build
