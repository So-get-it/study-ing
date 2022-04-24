LIBNAME=client
INSTPATH=`pwd`/../src

all: d_lib s_lib
	@make clear

d_lib:
	gcc -shared -fPIC *.c -o lib${LIBNAME}.so

s_lib:
	gcc -c *.c
	ar -rcs lib${LIBNAME}.a *.o

install:
	cp -rf lib${LIBNAME}.* ${INSTPATH}
	cp -rf *.h ${INSTPATH}

uninstall:
	rm -f ${INSTPATH}/lib${LIBNAME}.*
	rm -f ${INSTPATH}/*.h

clear:
	rm -f *.o

clean: clear
	rm -f lib${LIBNAME}.*
