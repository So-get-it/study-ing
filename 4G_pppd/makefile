LIBPATH=/home/nbiot/chenzhenyu/4G_pppd/lib/
LD_LIBRARY_PATH=/home/nbiot/chenzhenyu/4G_pppd/lib/
FNAME=4g_dial

all: clean
	gcc -W -Wall -I${LIBPATH} ${FNAME}.c -o ${FNAME} -L ${LIBPATH} -ldial-up -lxml2 -lpthread
	./${FNAME} -d 

gcc:
	gcc -W -Wall -I${LIBPATH} ${FNAME}.c -o ${FNAME} -L ${LIBPATH} -ldial-up -lxml2 -lpthread


clean:
	rm -f ${FNAME}
