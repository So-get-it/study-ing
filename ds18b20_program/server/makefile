
LIBPATH=/home/chenzhenyu/ds18b20_program/lib

all:
	gcc -I${LIBPATH} server.c -o server -lds18b20 -lsqlite3
	./server -p 7033  

