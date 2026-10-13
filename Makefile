CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -Iinclude
OBJS = src/main.o src/sort.o src/format.o src/traverse.o
TARGET = ls

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

.PHONY: all clean
