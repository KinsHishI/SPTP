### Makefile for Mine Find Project

.PHONY: clean

mserver: mine_server.o mine_game.o
	gcc -o mserver mine_server.o mine_game.o

mclient: mine_client.o mine_game.o 
	gcc -o mclient mine_client.o mine_game.o

mine_server.o: mine_server.c mine_game.c mine.h
	gcc -Wall -g -c -D_REENTRANT -lpthread mine_server.c mine_game.c 

mine_client.o: mine_client.c mine_game.c mine.h
	gcc -Wall -g -c -D_REENTRANT -lpthread mine_client.c mine_game.c

clean:
	rm *.o mserver mclient
