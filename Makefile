CC = gcc 
CFLAGS = -g -O0 -march=native -fomit-frame-pointer -Wall -Wextra
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
TARGET = driver

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o $(TARGET)

.PHONY: all clean