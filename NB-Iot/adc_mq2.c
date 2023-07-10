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

//需要读取的文件绝对路径
static char *file_path[] = {
	"/sys/bus/iio/devices/iio:device0/in_voltage_scale",
	"/sys/bus/iio/devices/iio:device0/in_voltage1_raw",
};

int main (int argc, char **argv)
{
	char       adc_buf[30] = {};
	adc_dev_t  mq2;

	memset(&mq2, 0,sizeof(mq2));
	while (1)
	{
		if (file_data_read(file_path[IN_VOLTAGE_SCALE], adc_buf, sizeof(adc_buf)) < 0)
		{
			printf ("Error:Read %s failure.\n", file_path[0]);
		}
		mq2.scale = atof(adc_buf);
		//printf ("Read ADC scale = %f\n", mq2.scale);
		
		if(file_data_read(file_path[IN_VOLTAGE1_RAW], adc_buf, sizeof(adc_buf)) < 0)
		{
			printf("Error : Read %s failure.\n", file_path[1]);
		}
		mq2.raw = atoi(adc_buf);//将字符串转换为整型
        // printf("Read ADC raw = %d\n", mq2.raw);
		
		mq2.act = (mq2.raw * mq2.scale) / 1000.f; //计算实际电压
		mq2.conc = ((float)mq2.raw / 4096.f) * 100.f; //计算实际浓度
		printf ("MQ-2 实际电压为:%.3fV,浓度为:%.1f%%.\n", mq2.act, mq2.conc);
		sleep(1);
	}
	return 0;
} 

static int file_data_read(char *filename, char *buf, size_t buf_size)
{
	int   rv = 0;
	int   fd = -1;

	if (!filename || !buf || !buf_size)
	{
		printf ("Error filename or str \n");
		return -1;
	}

	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		printf ("Open file '%s' failure:%s\n", filename, strerror(errno));
		rv = -2;
		goto CleanUp;
	}

	memset(buf, 0, buf_size);
	if (read(fd, buf, buf_size) < 0)
	{
		printf ("Read data from '%s'failure:%s\n", filename, strerror(errno));
		rv = -3;
		goto CleanUp;
	}

CleanUp:
	if (fd >= 0)
		close(fd);

	return rv;
}
