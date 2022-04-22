#include <stdio.h>

//#define DEBUG
#ifdef DEBUG
#define dbg_print(format,args...) printf(format,##args)
#else
#define dbg_print(format,args...) do{} while(0)
#endif

int main(int argc,char *argv[])
{
	dbg_print("Hello world!\n");
	return 0;
}

