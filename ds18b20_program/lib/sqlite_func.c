/*********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  sqlite_func.c
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

#include "sqlite_func.h"
#include "logger.h"


static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;
	for(i=0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}


/*
 * =====================================================================================
 *         Name:  sqlite_create_table
 *  Description:  To create a table in the file(basename).
 *   Input args:
 *                  basename: the file name will open or create
 *                        db: Declared a pointer object db of sqlite3
 * return value:  NULL
 * =====================================================================================
 */
void sqlite_create_table(char *basename, sqlite3 **db)
{
	char 		*zErrMsg = 0;
	int  		rc;
	char 		*sql;

	rc = sqlite3_open(basename, db);
	if( rc )
	{
		log_warn("Can't open database: %s\n", sqlite3_errmsg(*db));
		return ;
	}

	/* Create SQL statement */
	sql = "CREATE TABLE temp_database("  \
		   "SERIALNUMBER   KEY 		NOT NULL," \
		   "TIME           CHAR 	NOT NULL," \
		   "TEMPERATURE    CHAR 	NOT NULL);";

	/* Execute SQL statement */
	rc = sqlite3_exec(*db, sql, callback, 0, &zErrMsg);
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


/*
 * =====================================================================================
 *         Name:  sqlite_insert
 *  Description:  insert sample message into the table
 *   Input args:
 *                          db: Declared a pointer object db of sqlite3
 *                  serial_num: the serial name by self
 *                        time: sample time
 *                        temp: temperature while sample
 *
 * return value:  NULL
 * =====================================================================================
 */
void sqlite_insert(sqlite3 *db, char *serial_num, char *time, float temp)
{
	char		*zErrMsg = 0;
	int 		rc;
	char 		*sql;

	sql = (char *)malloc(128);
	snprintf(sql, 128, "INSERT INTO temp_database VALUES ('%s', '%s', '%.2f');", serial_num, time, temp);

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		log_error("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		log_info("Records created successfully\n");
	}
	free(sql);
	return ;
}


/*
 * =====================================================================================
 *         Name:  sqlite_data_row
 *  Description:  return the row of the table
 *   Input args:
 *                   db: Declared a pointer object db of sqlite3
 *
 * return value:  the number of the row
 * =====================================================================================
 */
int sqlite_data_row(sqlite3 *db)
{
	int             result;
	char            *errmsg = NULL;
	char            **dbResult; //是 char ** 类型，两个*号
	int             nRow, nColumn;

	result = sqlite3_get_table( db, "select * from temp_database", &dbResult, &nRow, &nColumn, &errmsg );

	return nRow;
}


/*
 * =====================================================================================
 *         Name:  sqlite_delete
 *  Description:  To delete the message where HEADNAME = $arg
 *   Input args:
 *                    db: Declared a pointer object db of sqlite3
 *                   arg: HEADNAME value
 *
 * return value:  NULL
 * =====================================================================================
 */
void sqlite_delete(sqlite3 *db, char *arg)
{
	char 		*zErrMsg = 0;
	int  		rc;
	char 		*sql = NULL;

	sql = (char *)malloc(64);
	snprintf(sql, 64, "delete from temp_database where TIME = '%s';", arg);

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK )
	{
		log_error("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
	{
		log_info("Records delete successfully\n");
	}
	return ;
}



/*
 * =====================================================================================
 *         Name:  sqlite_sql_table_firstvalue
 *  Description:  To get the first values from the table
 *   Input args:
 *                  db: Declared a pointer object db of sqlite3
 *                 num: address to save serial_name from table
 *                time: address to save time from table
 *                temp: address to save temp from table
 *  Output args:
 * return value:  NULL
 * =====================================================================================
 */
void get_sql_table_firstvalue(sqlite3 *db, char *num, char *time, float *temp)
{
	int             result;
	char            *errmsg = NULL;
	char            **dbResult; //是 char ** 类型，两个*号
	int             nRow, nColumn;
	int             i , j;
	int             index;

	memset(num, 0, 32);
	memset(time, 0, 64);

	result = sqlite3_get_table( db, "select * from temp_database", &dbResult, &nRow, &nColumn, &errmsg );

	if( SQLITE_OK == result )
	{
		index = nColumn;
		for(  i = 0; i < 1 ; i++ )  	//只取第一组数据
		{
			for( j = 0 ; j < nColumn; j++ )
			{
				if(0 == j)
				{
					strncpy(num, dbResult[index], strlen(dbResult[index]));
				}
				else if(1 == j)
				{
					strncpy(time, dbResult[index], strlen(dbResult[index]));
				}
				else
				{
					*temp = atof(dbResult[index]);
				}
				++index;
			}
		}
	}

	log_debug("time: %s\n", num);

	sqlite3_free_table( dbResult );

	return ;
}


/*
 * =====================================================================================
 *         Name:  sqlite_drop_table
 *  Description:  drop a table
 *   Input args:
 *                  db: Declared a pointer object db of sqlite3
 * return value:  NULL
 * =====================================================================================
 */
void sqlite_drop_table(sqlite3 *db)
{
	char 		*zErrMsg = 0;
	int  		rc;
	char 		*sql;

	sql = "drop table temp_database;";

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		log_error("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}else{
		log_info("Table drop successfully\n");
	}
	return ;
}
