CC = gcc
CFLAGS = -Wall -g -std=c11 -lpthread
TARGET = A2checker
SRC = A2checker.c  # Change file1.c and file2.c to your actual source files
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJ)
