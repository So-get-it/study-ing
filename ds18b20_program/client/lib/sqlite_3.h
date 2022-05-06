/********************************************************************************
 *      Copyright:  (C) 2022 Chen Zhenyu<2472734278@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  sqlite3.h
 *    Description:  This head file is sqlite function.
 *
 *        Version:  1.0.0(04/05/22)
 *         Author:  Chen Zhenyu <2472734278@qq.com>
 *      ChangeLog:  1, Release initial version on "04/05/22 19:40:41"
 *                 
 ********************************************************************************/

#ifndef _SQLITE3_H_
#define _SQLITE3_H_

#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "get_date_time.h"
#include "ds18b20.h"


#endif



extern void sqlite_create_table(sqlite3 *db);

extern void sqlite_drop_table(sqlite3 *db);

extern void sqlite_insert(sqlite3 *db, char *serial_num, char *date, char *time, float temp);

extern int sqlite_data_row(sqlite3 *db);

extern void sqlite_delete(sqlite3 *db, char *arg);

extern void get_sql_table_firstvalue(sqlite3 *db, temp_msg *msg, get_d_time *dt);

