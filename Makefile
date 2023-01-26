compile: input.o input_processing.o db_operations.o
	gcc -g input.o input_processing.o db_operations.o -o grocery

Wall: input.o input_processing.o db_operations.o
	gcc -Wall -g input.o input_processing.o db_operations.o -o grocery

run: input.o input_processing.o db_operations.o
	@gcc -g input.o input_processing.o db_operations.o -o grocery && ./grocery db3.csv

run_server: server.o input_processing.o db_operations.o
	@gcc -g server.o -lpthread input_processing.o db_operations.o -o server && ./server db3.csv 12333

run_client: client.o
	gcc -g client.o -o client && ./client 12333

server: server.o input_processing.o db_operations.o
	gcc -g server.o -lpthread input_processing.o db_operations.o -o server

server.o: server.c
	gcc -g -c -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast server.c 

client: client.o
	gcc -g client.o -o client

client.o: client.c
	gcc -g -c client.c 

input.o: input.c
	gcc -g -c input.c 

input_processing.o: input_processing.c
	gcc -g -c input_processing.c

db_operations.o: db_operations.c
	gcc -g -c db_operations.c

gdb:
	gdb --args ./grocery db3.csv

valgrind:
	valgrind --leak-check=yes ./grocery db3.csv

clear:
	rm -f *.o grocery server client a.out