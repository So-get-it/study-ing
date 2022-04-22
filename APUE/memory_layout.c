#include <stdio.h>
#include <stdlib.h>

int 	g_var1;		//未初始化的全局变量，BSS区
int 	g_var2 = 20; 	//初始化了的全局变量，DATA区

int main(int argc, char *argv[])
{
	static int 	s_var1;		//未初始化的静态变量
	static int 	s_var2 = 10;	//初始化了的静态变量

	char 		*str = "hello";
	char  		*ptr;

	ptr = malloc(100);

	printf("cmd args 【参数区】\n");
	printf("argv address:%p\n\n",argv);


	printf("stack 【栈区】\n");
	printf("str address:%p\n",&str);
	printf("ptr address:%p\n\n",&ptr);

	printf("heap 【堆区】\n");
	printf("malloc address:%p\n\n",ptr);

	printf("bss 【数据段；未初始化全局数据】\n");
	printf("s_var1 address:%p value:%d\n",&s_var1, s_var1);
	printf("g_var1 address:%p value:%d\n\n",&g_var1, g_var1);

	printf("data 【数据段；已初始化全局数据】\n");
	printf("s_var2 address:%p value:%d\n",&s_var2, s_var2);
	printf("g_var2 address:%p value:%d\n\n",&g_var2, g_var2);

	printf("rodata 【文字常量区】\n");
	printf("\"%s\" address:%p\n\n",str, str);

	printf("text  【文本段】\n");
	printf("main() address:%p\n\n",main);

	return 0;
}

