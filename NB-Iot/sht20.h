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

//#define CONFIG_PRINT_STDOUT

#if ( defined CONFIG_PRINT_STDOUT )
#define dbg_print(format,args...) printf(format, ##args)

#else
#define dbg_print(format,args...) do{} while(0);
#endif


#define SOFTRESET						0xFE
#define TRIGGER_TEMPERATURE_NO_HOLD		0xF3
#define TRIGGER_HUMIDITY_NO_HOLD		0xF5
#define TEMP_ID							"0019" //云平台温度服务ID
#define TEMP_LEN						"0004" //云平台温度服务数据长度
#define RH_ID							"001A" //云平台湿度服务ID
#define RH_LEN							"0004" //云平台湿度服务数据长度

static inline void msleep(unsigned long ms);

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

	printf ("Temperature=%lf ℃  Relative humidity=%lf%%\n", temp, rh);

	memset(temp_str, 0, len);
	/* 将温度转化为十六进制并转换为相应AT 报文格式 */
	snprintf(temp_str, 19, "02%s%s%08X", TEMP_ID, TEMP_LEN, (int32_t)temp);
	dbg_print ("temp_str:%s\n", temp_str);

	memset(rh_str, 0, len);
	/* 将湿度转化为十六进制并转换为相应AT 报文格式 */
	snprintf(rh_str, 19, "02%s%s%08X", RH_ID, RH_LEN, (int32_t)rh);
	dbg_print ("rh_str:%s\n", rh_str);

	return 0;
}

#endif
