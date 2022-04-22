#include <stdio.h>
#include <unistd.h>

#define T_FILE "t1.txt"

int main(int argc, char *argv)
{
	if(access(T_FILE,F_OK) != 0)
	{
		printf("file %s not exist!\n",T_FILE);
	}

	printf("file %s exist!\n",T_FILE);
	
	if(access(T_FILE,R_OK) != 0)
	{
		printf("Read not OK\n");
	}
	else
		printf("Read OK\n");
	
	if(access(T_FILE,W_OK) != 0)
	{
		printf("Write not OK\n");
	}
	else
		printf("Write OK\n");
	
	if(access(T_FILE,X_OK) != 0)
	{
		printf("Execute not OK\n");
	}
	else
		printf("Execute OK!\n");

	return 0;
}


	
