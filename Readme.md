# MAXINT

Maxint is a c written program that implements maxint protocol that works as follows 
 - server recieves requests from clients sending ints and takes max 
 - keeps the max int, the pseudo of the client and the ip adress
 - when another client sens MAX asking for the max int, the serves answers with pseudo, ip, val of the max int


## Main components 
- max_int_server.c
- client1.c
- client2.c

## Compilation 
``` make ``` or ``` make <server><client1><client2>```
## Running
``` ./server [port] ``` port is optional, set to 2525 as default\
to run clients :\
``` ./client1 <port>``` to run client 1
 ``` ./client2 <port>``` to run client 2


```make clean``` to clean your folder from executable files

by Chems-eddine BENAZIZA :)
