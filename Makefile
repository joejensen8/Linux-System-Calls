CC = gcc
SRC = hw1.c
OUT = main

all: $(SRC)
	$(CC) -o $(OUT) $(SRC)
	chmod 755 $(OUT)

clean: $(OUT)
	rm $(OUT)
