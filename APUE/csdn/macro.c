#include <stdio.h>

void test()
{
	printf("Another function is: %s\n", __FUNCTION__);
}

int main()
{
	printf("The file is %s\n", __FILE__);
	printf("The date is %s\n", __DATE__);
	printf("The time is %s\n", __TIME__);
	printf("This line is %d\n", __LINE__);
	printf("This function is %s\n", __FUNCTION__);

	test();
	return 0;
}


