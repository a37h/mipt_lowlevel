.PHONY: all
all: server clean

server: server.o
		g++ server.o -g3 -lpthread -lrt -o server
server.o: server.cpp
		g++ -c -std=c++11 -g3 server.cpp

clean:
	rm server.o