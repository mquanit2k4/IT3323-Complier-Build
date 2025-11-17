CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

TARGET = Bai1
SRC = Bai1.c
OBJ = Bai1.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)

clean:
	rm -f $(OBJ) $(TARGET)
