/********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  sht20.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(2023年07月10日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年07月10日 19时23分51秒"
 *                 
 ********************************************************************************/

#ifndef _SHT20_H_
#define _SHT20_H_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "logger.h"

#define SOFTRESET						0xFE
#define TRIGGER_TEMPERATURE_NO_HOLD		0xF3
#define TRIGGER_HUMIDITY_NO_HOLD		0xF5


static inline void dump_buf(const char *prompt, uint8_t *buf, int size);

int sht2x_init(char *i2c_dev);

int sht2x_get_serialnumber(int fd, uint8_t *serialnumber, int size);

int sht2x_get_temp_humidity(int fd, float *temp, float *rh);

/* sht20 采样后将数据打包成AT 命令所需格式
 * fd: 打开sht20 设备文件描述符
 * temp_str: 温度报文
 * rh_str: 湿度报文
 * len: temp_str与rh_str 长度相同*/
static inline int sht2x_sample(int fd, char *temp_str, char *rh_str, size_t len)
{
	float		temp;
	float		rh;

	if ( sht2x_get_temp_humidity(fd, &temp, &rh) < 0 )
	{
		printf ("SHT2x get get temperature and relative humidity failure\n");
		return -1;
	}

	PARSE_LOG_INFO("Temperature=%lf ℃  Relative humidity=%lf%%\n", temp, rh);

	memset(temp_str, 0, len);
	/* 将温度转化为十六进制 */
	snprintf(temp_str, 9, "%08X", (int32_t)temp);
	PARSE_LOG_DEBUG("temp_str:%s\n", temp_str);

	memset(rh_str, 0, len);
	/* 将湿度转化为十六进制 */
	snprintf(rh_str, 9, "%08X", (int32_t)rh);
	PARSE_LOG_DEBUG("rh_str:%s\n", rh_str);

	return 0;
}

#endif
