LIBPATH=/home/nbiot/chenzhenyu/4G_pppd/lib/
LD_LIBRARY_PATH=/home/nbiot/chenzhenyu/4G_pppd/lib/
FNAME=4g_dial

all: clean
	gcc -W -Wall -I${LIBPATH} ${FNAME}.c -o ${FNAME} -L ${LIBPATH} -ldial-up -lxml2 -lpthread
	./${FNAME} -f 0 -b 115200 -d 8 -p n -s 1 

run:
	./${FNAME} -f 0 -b 115200 -d 8 -p n -s 1 

clean:
	rm -f ${FNAME}
