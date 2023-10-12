CC=gcc
CFLAGS=-Wall -g -std=c11 -O3 -Isrc/
LDFLAGS=

SRC_DIR=src
TEST_DIR=test
BUILD_DIR=build

LIB_SOURCES=$(wildcard $(SRC_DIR)/*.c)
LIB_OBJECTS=$(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.a, $(LIB_SOURCES))
TEST_SOURCES=$(wildcard $(TEST_DIR)/*.c)
TEST_EXECUTABLE=$(BUILD_DIR)/test

all: $(TEST_EXECUTABLE)

$(TEST_EXECUTABLE): $(LIB_OBJECTS) $(TEST_SOURCES) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(TEST_SOURCES) $(LIB_OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.a: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean

