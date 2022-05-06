/*********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  sqlite3.c
 *    Description:  This file is sqlite3's function.
 *                 
 *        Version:  1.0.0(30/04/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "30/04/22 11:48:58"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sqlite3.h>

#include "sqlite_server.h"


static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;
	for(i=0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

void sqlite_create_table(sqlite3 *db)
{
	char 		*zErrMsg = 0;
	int  		rc;
	char 		*sql;

	/* Create SQL statement */
	sql = "CREATE TABLE temp_database("  \
		   "SERIALNUMBER   KEY 		NOT NULL," \
		   "DATE           CHAR 	NOT NULL," \
		   "TIME           CHAR 	NOT NULL," \
		   "TEMPERATURE    CHAR 	NOT NULL);";

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		printf("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		printf("Table created successfully\n");
	}
	return ;
}


void sqlite_insert(sqlite3 *db, char *serial_num, char *date, char *time, float temp)
{
	char		*zErrMsg = 0;
	int 		rc;
	char 		*sql;

	sql = (char *)malloc(128);
	snprintf(sql, 128, "INSERT INTO temp_database VALUES ('%s', '%s', '%s', '%.2f');", serial_num, date, time, temp);

	/* Execute SQL statement */
//	printf("sqlite insert start running...\n%s\n", sql);
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		printf("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		printf("Records created successfully\n");
	}
	free(sql);
	return ;
}

int sqlite_data_row(sqlite3 *db)
{
	int             result;
	char            *errmsg = NULL;
	char            **dbResult; //是 char ** 类型，两个*号
	int             nRow, nColumn;

	result = sqlite3_get_table( db, "select * from temp_database", &dbResult, &nRow, &nColumn, &errmsg );

	return nRow;
}
