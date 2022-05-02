/*********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  sql_get_table.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(01/05/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "01/05/22 10:59:22"
 *                 
 ********************************************************************************/


#include <stdio.h>
#include <sqlite3.h>


int main()
{
	sqlite3 	*db;
	int 		result;
	char 		*errmsg = NULL;
	char 		**dbResult; //是 char ** 类型，两个*号
	int 		nRow, nColumn;
	int 		i , j;
	int			index;

	result = sqlite3_open( "get_temp.db", &db );

	if( result != SQLITE_OK )
	{
		//数据库打开失败
		return -1;
	}

	//数据库操作代码
	//假设前面已经创建了 MyTable_1 表
	//开始查询，传入的 dbResult 已经是 char **，这里又加了一个 & 取地址符，传递进去的就成了 char ***
	result = sqlite3_get_table( db, "select * from temperature_Msg", &dbResult, &nRow, &nColumn, &errmsg );
	if( SQLITE_OK == result )
	{
		//查询成功
		index = nColumn; //前面说过 dbResult 前面第一行数据是字段名称，从 nColumn 索引开始才是真正的数据
		printf( "查到%d条记录\n", nRow );

		for(  i = 0; i < nRow ; i++ )
		{
			printf( "第 %d 条记录\n", i+1 );
			for( j = 0 ; j < nColumn; j++ )
			{
				printf( "values:%s\n",  dbResult[j], dbResult [index] );
				++index; // dbResult 的字段值是连续的，从第0索引到第 nColumn - 1索引都是字段名称，从第 nColumn 索引开始，后面都是字段值，它把一个二维的表（传统的行列表示法）用一个扁平的形式来表示
			}
			printf( "-------\n" );
		}
	}

	//到这里，不论数据库查询是否成功，都释放 char** 查询结果，使用 sqlite 提供的功能来释放
	sqlite3_free_table( dbResult );

     //关闭数据库
	sqlite3_close( db );
	return 0;
}
