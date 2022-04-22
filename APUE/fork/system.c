#include <stdio.h>
#include <stdlib.h>

int main()
{
	system("ifconfig ens33 | grep netmask");
	return 0;
}
