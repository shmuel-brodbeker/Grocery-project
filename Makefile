compile: input.o input_processing.o db_operations.o
	gcc -g input.o input_processing.o db_operations.o -o grocery

Wall: input.o input_processing.o db_operations.o
	gcc -Wall -g input.o input_processing.o db_operations.o -o grocery

run: input.o input_processing.o db_operations.o
	@gcc -g input.o input_processing.o db_operations.o -o grocery && ./grocery db3.csv

run_server: server.o input_processing.o db_operations.o
	@gcc -g server.o input_processing.o db_operations.o -o server && ./server db3.csv

server: server.o input_processing.o db_operations.o
	gcc -g server.o input_processing.o db_operations.o -o server

server.o: server.c
	gcc -g -c server.c 

client: client.o
	gcc -g client.o 

client.o: client.c
	gcc -g client.c 

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

clean:
	rm -f *.o grocery server client