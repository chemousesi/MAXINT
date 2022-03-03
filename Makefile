
CC ?= gcc 
CCFLAGS = -Wall  -O1 -g -pedantic 

.phony: all 

all: server client1 client2

server: maxint_server.c
	cc $(CCFLAGS) -pthread -o $@ $^

client1 : client1.c
	cc $(CCFLAGS) -o $@ $^
	
client2 : client2.c
	cc $(CCFLAGS) -o $@ $^

.phony: clean

clean:
	rm -f *.o
	rm server
	rm client1
	rm client2

	
	