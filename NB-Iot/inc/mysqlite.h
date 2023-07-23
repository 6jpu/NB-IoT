/********************************************************************************
 *      Copyright:  (C) 2023 Hu Ben Yuan<2017603273@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  smog_sqlite.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(07/21/2023)
 *         Author:  Hu Ben Yuan <2017603273@qq.com>
 *      ChangeLog:  1, Release initial version on "07/21/2023 07:54:00 PM"
 *                 
 ********************************************************************************/

#ifndef  _MYSQLITE_H
#define  _MYSQLITE_H

#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>

#define table_name "smoke_data"

static sqlite3  *db;

int sqlite_create(void);

int sqlite_insert(float *smoke);

int sqlite_select(float *smokes);

int sqlite_delete(void);

int sqlite_close(void);

#endif
