/*********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  adc_mq2.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(2023年07月08日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年07月08日 16时18分54秒"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "adc_mq2.h"
#include "logger.h"
#include "data_check.h"

#define SMOKE_ID						"001B" //云平台烟雾浓度服务ID
#define SMOKE_LEN						"0004" //云平台烟雾浓度服务数据长度

//需要读取的mq2 采样数据文件绝对路径
static char *file_path[] = {
	"/sys/bus/iio/devices/iio:device0/in_voltage_scale",
	"/sys/bus/iio/devices/iio:device0/in_voltage1_raw",
};


static int file_data_read(char *filename, char *buf, size_t buf_size)
{
	int   rv = 0;
	int   fd = -1;

	if (!filename || !buf || !buf_size)
	{
		PARSE_LOG_ERROR ("Error filename or str \n");
		return -1;
	}

	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		PARSE_LOG_ERROR ("Open file '%s' failure:%s\n", filename, strerror(errno));
		rv = -2;
		goto CleanUp;
	}

	memset(buf, 0, buf_size);
	if (read(fd, buf, buf_size) < 0)
	{
		PARSE_LOG_ERROR ("Read data from '%s'failure:%s\n", filename, strerror(errno));
		rv = -3;
		goto CleanUp;
	}

CleanUp:
	if (fd >= 0)
		close(fd);

	return rv;
}

/*  mq2 采样后将数据打包成AT 命令所需格式
 *  mq2: 存放mq2 数据的结构体
 *  size: mq2 结构体adc_dev_t 的大小
 *  smoke_str: 烟雾浓度报文
 *  len: smoke_str 长度*/
int mq2_sample(adc_dev_t *mq2, int size, char *smoke_str, size_t len)
{
	char       adc_buf[30] = {};

    if( !mq2 )
    {
        PARSE_LOG_ERROR("invalid input arugments\n");
        return -1;
    }

	memset(mq2, 0, size);
	if (file_data_read(file_path[IN_VOLTAGE_SCALE], adc_buf, sizeof(adc_buf)) < 0)
	{
		PARSE_LOG_ERROR ("Error:Read %s failure.\n", file_path[0]);
		return -2;
	}

	mq2->scale = atof(adc_buf);
	PARSE_LOG_DEBUG ("Read ADC scale = %f\n", mq2->scale);
	
	if(file_data_read(file_path[IN_VOLTAGE1_RAW], adc_buf, sizeof(adc_buf)) < 0)
	{
		PARSE_LOG_ERROR ("Error : Read %s failure.\n", file_path[1]);
		return -3;
	}
	
	mq2->raw = atoi(adc_buf);//将字符串转换为整型
	PARSE_LOG_DEBUG ("Read ADC raw = %d\n", mq2->raw);
	
	mq2->act = (mq2->raw * mq2->scale) / 1000.f; //计算实际电压
	mq2->conc = ((float)mq2->raw / 4096.f) * 100.f; //计算实际浓度
	PARSE_LOG_INFO ("MQ-2 实际电压为:%.3fV,浓度为:%.1f%%.\n", mq2->act, mq2->conc);

	/* 烟雾浓度自检 */
	if(data_check( &mq2->conc ) < 0)
	{
		PARSE_LOG_ERROR ("Abnormal smoke concentration!\n");
		return -4;
	}

	/* 当烟雾浓度超过80%时，自动触发警报 ,点亮LED 灯并打开蜂鸣器*/
	if ( (int32_t)mq2->conc > 80 )
	{
		led_control(&led[LED_RED], ON);
//		pwm_config("enable", "1");
	}

	memset(smoke_str, 0, len);
	/*  将烟雾浓度转化为十六进制并转换为相应AT 报文格式 */
	snprintf(smoke_str, 19, "02%s%s%08X", SMOKE_ID, SMOKE_LEN, (int32_t)mq2->conc);
	PARSE_LOG_DEBUG ("smoke_str:%s\n", smoke_str);
				
	return 0;
} 
