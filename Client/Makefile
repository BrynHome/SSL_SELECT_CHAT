# make for client
LD=-lssl -lcrypto

CC=gcc

client: clnt.o common.o
	$(CC) clnt.o common.o -o client $(LD)

clean:
	rm -f *.o core.* client
	
clnt.o: clnt.c
	gcc -O -c clnt.c

common.o: common.c
	gcc -O -c common.c
