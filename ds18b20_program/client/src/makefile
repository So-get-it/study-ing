LIBPATH=`pwd`/../lib/


all:
	gcc -I${LIBPATH} client.c -o client -L${LIBPATH} -static -lclient
	./client -p 7077 -i 127.0.0.1 -t 5

run:
	./client -p 7077 -i 127.0.0.1
