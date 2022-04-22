#include <stdio.h>

void pswap(int *pa,int *pb);
void swap(int a,int b);
int main(int argc,char *argv[])
{
	int a = 10;
	int b = 20;

	printf("Before swap: a = %d  b = %d\n",a,b);
	swap(a,b);
	printf(" After swap: a = %d  b = %d\n",a,b);
	
	printf("Before pswap: a = %d  b = %d\n",a,b);
	pswap(&a,&b);
	printf(" After pswap: a = %d  b = %d\n",a,b);
}

void swap(int a,int b)
{	
	int tmp;
	tmp = a;
	a = b;
	b = tmp;
}

void pswap(int *pa,int *pb)
{	
	int tmp;
	tmp = *pa;
	*pa = *pb;
	*pb = tmp;
}


