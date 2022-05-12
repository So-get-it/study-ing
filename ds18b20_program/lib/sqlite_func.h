/********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  sqlite_func.h
 *    Description:  This head file is sqlite function.
 *
 *        Version:  1.0.0(04/05/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "04/05/22 19:40:41"
 *                 
 ********************************************************************************/

#ifndef _SQLITE3_H_
#define _SQLITE3_H_


/* 
 * =====================================================================================
 *         Name:  sqlite_create_table
 *  Description:  To create a table in the file(basename).
 *   Input args:  
 *   				basename: the file name will open or create
 *   				      db: Declared a pointer object db of sqlite3
 * return value:  NULL
 * =====================================================================================
 */
extern void sqlite_create_table(char *basename, sqlite3 **db);


/* 
 * =====================================================================================
 *         Name:  sqlite_insert
 *  Description:  insert sample message into the table
 *   Input args:
 *   						db: Declared a pointer object db of sqlite3
 *   				serial_num: the serial name by self
 *   					  time: sample time 
 *   					  temp: temperature while sample
 *
 * return value:  NULL
 * =====================================================================================
 */
extern void sqlite_insert(sqlite3 *db, char *serial_num, char *time, float temp);


/* 
 * =====================================================================================
 *         Name:  sqlite_data_row
 *  Description:  return the row of the table 
 *   Input args:
 *   				db: Declared a pointer object db of sqlite3
 *
 * return value:  the number of the row
 * =====================================================================================
 */
extern int sqlite_data_row(sqlite3 *db);


/* 
 * =====================================================================================
 *         Name:  sqlite_delete
 *  Description:  To delete the message where HEADNAME = $arg
 *   Input args:
 *   				 db: Declared a pointer object db of sqlite3
 *   				arg: HEADNAME value
 *
 * return value:  NULL
 * =====================================================================================
 */
extern void sqlite_delete(sqlite3 *db, char *arg);


/* 
 * =====================================================================================
 *         Name:  sqlite_sql_table_firstvalue
 *  Description:  To get the first values from the table
 *   Input args:
 *   				  db: Declared a pointer object db of sqlite3
 *   				 num: address to save serial_name from table
 *   				time: address to save time from table
 *   				temp: address to save temp from table
 *  Output args:
 * return value:
 * =====================================================================================
 */
extern void get_sql_table_firstvalue(sqlite3 *db, char *num, char *time, float *temp);

/* 
 * =====================================================================================
 *         Name:  sqlite_drop_table
 *  Description:  drop a table
 *   Input args:
 *   				db: Declared a pointer object db of sqlite3
 * return value:  NULL
 * =====================================================================================
 */
extern void sqlite_drop_table(sqlite3 *db);


#endif


