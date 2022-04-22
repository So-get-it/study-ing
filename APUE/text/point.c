#include <stdio.h>

int main(int argc,char *argv[])
{
	int 	a = 0x12345678;

	int *pi = &a;
	char *pc = (char *)&a;
	short *ps = (short *)&a;

	printf("&a: %p\n",&a);
	printf("pi: 0x%x\n",*pi);
	printf("pc: 0x%x\n",*pc);
	printf("ps: 0x%x\n",*ps);
	
	pi++;
	pc++;
	ps++;

	printf("&pi: 0x%x\n",*pi);
	printf("&pc: 0x%x\n",*pc);
	printf("&ps: 0x%x\n",*ps);

	return 0;
}

