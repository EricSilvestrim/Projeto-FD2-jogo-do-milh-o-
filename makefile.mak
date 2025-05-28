CC = gcc
CFLAGS = -Wall -g
SRC = src
BIN = bin

OBJS = $(SRC)/main.o $(SRC)/pergunta.o $(SRC)/jogo.o

all: run

build: $(OBJS)
	$(CC) $(CFLAGS) -o jogo $(OBJS)

$(SRC)/main.o: $(SRC)/main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC)/pergunta.o: $(SRC)/pergunta.c $(SRC)/pergunta.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC)/jogo.o: $(SRC)/jogo.c $(SRC)/jogo.h
	$(CC) $(CFLAGS) -c $< -o $@

run: build
	./jogo

clean:
	rm -f $(SRC)/*.o jogo
