LIBPATH=/home/chenzhenyu/ds18b20_program/client/lib


all:
	gcc -I${LIBPATH} client.c -o client -lclient -lsqlite3
	./client -p 7033 -i 127.0.0.1 -t 30 -b 

