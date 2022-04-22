#include <stdio.h>

int main(int argc,char *argv[])
{
	int a = 0x12345678;
	char *p = NULL;
	p = (char *)&a;

	printf("a: %x &a: %p\n",a,&a);
	printf("p: %p *p: %x &p: %p\n",p,*p,&p);
	*p = 0x00;
	printf("a: %x &a: %p\n",a,&a);
	printf("p: %p *p: %x &p: %p\n",p,*p,&p);

	return 0;
}
