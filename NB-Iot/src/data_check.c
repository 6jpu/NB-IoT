/* ********************************************************************************
 *  *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *  *                  All rights reserved.
 *  *
 *  *       Filename:  data_check.c
 *  *    Description:  This file 
 *  *                 
 *  *        Version:  1.0.0(2023年06月09日)
 *  *         Author:  Kun_ <1433729173@qq.com>
 *  *      ChangeLog:  1, Release initial version on "2023年06月09日 21时36分09秒"
 *  *                 
 *  ********************************************************************************/

#include "data_check.h"
#include "logger.h"

int data_check(float *smoke)
{
	float	diff;
	float	pre_smoke;

	if(*smoke < 0)
	{
		PARSE_LOG_ERROR ("data error\n");
		return -1;
	}

	if(sqlite_select(&pre_smoke) < 0)
	{
			PARSE_LOG_WARN ("database get data failure or has no data\n");
			goto INIT;
	}

	else
	{
			diff = abs(*smoke - pre_smoke);

			if(diff > 50.0)
			{
					PARSE_LOG_ERROR ("The data is wrong, discard it.\n");
					return -2;
			}

			else
			{
					PARSE_LOG_DEBUG ("The data is ok\n");

INIT:
					if(sqlite_insert(smoke) < 0)
					{
							PARSE_LOG_ERROR ("Insert data failed\n");
							return -3;
					}
			}
	}

	return 0;
}
