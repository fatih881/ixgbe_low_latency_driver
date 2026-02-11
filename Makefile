CC = gcc 
CFLAGS = -g -O0 -march=native -fomit-frame-pointer -Wall -Wextra
SRC = $(shell find . -name '*.c')
OBJ = $(SRC:.c=.o)
TARGET = driver

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	find . -name '*.o' -delete
	rm -f $(TARGET)

.PHONY: all clean