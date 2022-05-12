LIBPATH=/home/chenzhenyu/ds18b20_program/lib


all:
	gcc -I${LIBPATH} client.c -o client -lds18b20 -lsqlite3
	./client -p 7033 -h 127.0.0.1 -t 5  

run:
	./client -p 7033 -h master.iot-yun.club -t 5 
