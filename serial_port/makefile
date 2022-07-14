LIBPATH=/home/nbiot/chenzhenyu/serial_port/lib
FNAME=ttyUSBN

all: clean
	gcc -I${LIBPATH} ${FNAME}.c -o ${FNAME} -L${LIBPATH} -lserial_port -static
	./${FNAME} -f 0 -b 9600 -d 8 -p n -s 1 

run:
	./${FNAME} -f 0 -b 9600 -d 8 -p n -s 1

clean:
	rm -f ${FNAME}
