
LIBPATH=/home/chenzhenyu/ds18b20_program/lib

all: clean
	gcc -I${LIBPATH} server.c -o server -lds18b20 -lsqlite3
	./server -p 7033 -b

clean:
	rm -f *.log
	rm -f *.db
	rm -f server
