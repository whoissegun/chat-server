CC=gcc
CFLAGS=-Wall -I./include
OBJ=./src/main.o ./src/client/client.o ./src/server/server.o ./src/utils.o
DEPS=./include/defs.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: chatapp

chatapp: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJ) chatapp

