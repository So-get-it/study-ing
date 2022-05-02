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

#include "server.h"


static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;
	for(i=0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

void sqlite_create_table()
{
	sqlite3 	*db;
	char 		*zErrMsg = 0;
	int  		rc;
	char 		*sql;

	/* Open database */
	rc = sqlite3_open("get_temp.db", &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		exit(0);
	}else{
		fprintf(stdout, "Opened database successfully\n");
	}

	/* Create SQL statement */
	sql = "CREATE TABLE temperature_Msg("  \
		   "SERIAL NUMBER  KEY     NOT NULL," \
		   "DATE           CHAR    NOT NULL," \
		   "TIME           CHAR    NOT NULL," \
		   "TEMPERATURE    REAL 	 NOT NULL);";

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}else{
		fprintf(stdout, "Table created successfully\n");
	}
	sqlite3_close(db);
	return ;
}

void sqlite_drop_table()
{
	sqlite3 	*db;
	char 		*zErrMsg = 0;
	int  		rc;
	char 		*sql;

	/* Open database */
	rc = sqlite3_open("get_temp.db", &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		exit(0);
	}else{
		fprintf(stdout, "Opened database successfully\n");
	}

	/* Create SQL statement */
	sql = "drop table temperature_Msg;";

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}else{
		fprintf(stdout, "Table drop successfully\n");
	}
	sqlite3_close(db);
	return ;
}

void sqlite_insert(char *serial_num, char *date, char *time, float temp)
{
	sqlite3	 	*db;
	char		*zErrMsg = 0;
	int 		rc;
	char 		*sql;
	char 		*buf;

	/* Open database */
	rc = sqlite3_open("get_temp.db", &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		exit(0);
	}else{
		fprintf(stderr, "Opened database successfully\n");
	}
/*  

	printf("serial_num: %s\n", serial_num);
	printf("      date: %s\n", date);
	printf("      time: %s\n", time);
	printf("      temp: %.2f\n", temp);

	printf("serial_num: %s\n", serial_num);
	printf("      date: %s\n", date);
	printf("      time: %s\n", time);
	printf("      temp: %.2f\n", temp);

	printf("serial_num: %s\n", serial_num);
	printf("      date: %s\n", date);
	printf("      time: %s\n", time);
	printf("      temp: %.2f\n", temp);
*/
   /* Create SQL statement */
//	snprintf(buf, 128, "( %s, %s, %s, %.2f℃);", serial_num, date, time, temp);
//	printf("      buf: %s\n", buf);

	sql = (char *)malloc(128);
	snprintf(sql, 128, "insert into temperature_Msg values('%s', '%s', '%s', '%.2f℃');", serial_num, date, time, temp);
	//   sql = "insert into temperature_Msg values(ID,NAME,AGE,ADDRESS,SALARY);";
//	printf("      sql: %s\n", sql);

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}else{
		fprintf(stdout, "Records created successfully\n");
	}
	free(sql);
	sqlite3_close(db);
	return ;
}


void sqlite_delete()
{
	sqlite3 	*db;
	char 		*zErrMsg = 0;
	int  		rc;
	char 		*sql;

	/* Open database */
	rc = sqlite3_open("get_temp.db", &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		exit(0);
	}else{
		fprintf(stdout, "Opened database successfully\n");
	}

	/* Create SQL statement */
	sql = "delete * from temperature_Msg;";

	/* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}else{
		fprintf(stdout, "Records delete successfully\n");
	}
	sqlite3_close(db);
	return ;
}
