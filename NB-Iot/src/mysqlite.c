/*********************************************************************************
 *      Copyright:  (C) 2023 Hu Ben Yuan<2017603273@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  mysqlite.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(07/21/2023)
 *         Author:  Hu Ben Yuan <2017603273@qq.com>
 *      ChangeLog:  1, Release initial version on "07/21/2023 03:30:37 PM"
 *                 
 ********************************************************************************/

#include "mysqlite.h"
#include "logger.h"

/* 回调函数 */
static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int    i;

	for(i=0; i<argc; i++)
    {
        PARSE_LOG_INFO ("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    
	PARSE_LOG_INFO  ("\n");
    return 0;
}


/* 创建连接数据库并且创建名为smoke_data 的表 */
int sqlite_create(void)
{
    char              create_buf[128];
    int               rv;
    char             *sql;
    char             *zErrMsg;

    rv = sqlite3_open("smoke.db", &db);

    if( rv )
    {
        PARSE_LOG_ERROR ("Can't open database: %s\n", strerror(errno));
        return -1;
    }
    else
    {
        PARSE_LOG_INFO ("Opened database successfully.\n");
    }

	memset(create_buf,0,sizeof(create_buf));
	sprintf(create_buf,"CREATE TABLE IF NOT EXISTS %s(ID INTEGER PRIMARY KEY, RH CHAR(50));", table_name);
	rv = sqlite3_exec(db,create_buf,callback,0,&zErrMsg);

	if( rv != SQLITE_OK )
	{
		PARSE_LOG_ERROR ("failure to create %s: %s\n", table_name,zErrMsg);
		sqlite3_free(zErrMsg);
		return -1;
	}
	else
	{
		PARSE_LOG_INFO ("create %s successfully\n", table_name);
	}

	return 0;
}

/*向数据库表里面插入数据*/
int sqlite_insert(float *smoke)
{
    int        rv;
    char       insert_buf[256];
    char      *zErrMsg;
    
    memset(insert_buf, 0, sizeof(insert_buf));
    sprintf(insert_buf,"INSERT INTO %s VALUES( NULL, '%f' );", table_name, *smoke);

    rv = sqlite3_exec(db, insert_buf, callback, 0, &zErrMsg);

    if(rv != SQLITE_OK)
    {
        
        PARSE_LOG_ERROR ("insert data failure: %s\n",zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }

    PARSE_LOG_INFO ("Insert  data successfully\n");
    return 0;
}

/* 获取数据库表里面最后一条数据 */ 
int sqlite_select(float *pre_smoke)
{
    char            select_buf[128];
    int             rv;
    int             counts;
    int             rownum;
    int             colnum;
    char           *zErrMsg;
    char          **result;

    memset(select_buf,0,sizeof(select_buf));
    sprintf(select_buf,"SELECT * FROM %s ORDER BY id DESC LIMIT 1;", table_name);
    rv = sqlite3_get_table(db, select_buf, &result, &rownum, &colnum, &zErrMsg);
    if( rownum <=0 )
	{
		PARSE_LOG_INFO ("database has no data: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return -2;
	}
	else
	{
		if(rv != SQLITE_OK)
		{
			PARSE_LOG_ERROR ("Obtaining data failure: %s\n",zErrMsg);
			sqlite3_free(zErrMsg);
		}

		*pre_smoke = atof(result[1*colnum+1]);
		
	}
    return rownum;
}

/* 删除数据库表里面第一条数据 */
int sqlite_delete(void)
{
    int           rv;
    char          delete_buf[128];
    char         *zErrMsg;
    
    memset(delete_buf,0,sizeof(delete_buf));
    sprintf(delete_buf,"DELETE FROM %s WHERE ID=(SELECT ID FROM %s LIMIT 1);", table_name, table_name);

    rv = sqlite3_exec(db, delete_buf, callback, 0, &zErrMsg);

    if( rv != SQLITE_OK )
    {
        PARSE_LOG_ERROR ("delete packaged_data failure: %s\n",zErrMsg);
        sqlite3_free(zErrMsg);
    }
    PARSE_LOG_INFO ("delete packaged_data successfully\n");
    return 0;
}

/* 关闭数据库 */
int sqlite_close(void)
{
    int         rv;
    char       *zErrMsg;

    rv = sqlite3_close(db);
    if(rv != SQLITE_OK)
    {
        PARSE_LOG_ERROR ("close database failure: %s\n",zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }
    PARSE_LOG_INFO ("close database successfully\n");
    return 0;
}
