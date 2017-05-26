.PHONY: all
all: client clean

client: client.o
		g++ client.o -g3 -lpthread -lrt -o client
client.o: client.cpp
		g++ -c -std=c++11 -g3 client.cpp

clean:
	rm client.o