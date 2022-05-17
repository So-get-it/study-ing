LIBPATH=/home/chenzhenyu/ds18b20_program/lib
FNAME=client

all: clean
	gcc -I${LIBPATH} ${FNAME}.c -o ${FNAME} -lds18b20 -lsqlite3
	./${FNAME} -p 7033 -h 127.0.0.1 -t 600 -b

run:
	./${FNAME} -p 7033 -h master.iot-yun.club -t 5

clean:
	rm -f *.log
	rm -f *.db
	rm -f ${FNAME}
