all: client server

client:Client.c
	gcc -o client Client.c -lsocket -lnsl -lresolv

server:Server.c
	gcc -o server Server.c -lsocket -lnsl -lresolv
