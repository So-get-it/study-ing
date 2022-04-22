#include <stdio.h>
#include <string.h>

int main(int argc,char *argv[])
{
	char str1[128] = "hello";
	char *str2 = "worlddddddd";

	printf("str1:%s  str2:%s\n",str1,str2);
	strncat(str1,str2,5);
	printf("str1:%s  str2:%s\n",str1,str2);
}

